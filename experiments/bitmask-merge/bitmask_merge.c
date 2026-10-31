//  name:
//      bitmask_merge
//
//  purpose:
//      Experimental, stand-alone re-implementation of the East-West merge in
//      SuitC (merge_quirk + check_quirk), to measure a faster algorithm.
//      It is not part of SuitC and does not change any SuitC result.
//
//      SuitC merges the strategy lists of two East-West plays: every pair
//      (a, b) gives the strategy min(a, b), board by board, and every
//      strategy that is dominated by another one is removed.
//
//      Here a strategy is stored bit-sliced: mask[v] holds the boards on
//      which North-South take more than v tricks. Then
//          min(a, b)      = mask-wise AND
//          x <= y         = every mask of x is a subset of the mask of y
//      Boards that occur on one side only get every bit set on the other
//      side, so the AND keeps the value of the side that has the board.
//
//      The dominated strategies are removed in three steps:
//        1. all pairs that share one strategy of the smaller side form a
//           group; each group is filtered on its own (in parallel);
//        2. the survivors are sorted by their total number of tricks:
//           a strategy can only be dominated by one with a larger total,
//           or by an identical one;
//        3. the sorted list is cut into chunks; each chunk is filtered on
//           its own, then each survivor is checked against the survivors
//           of the earlier chunks (both in parallel).
//
//  usage:
//      bitmask_merge a.txt b.txt [out.txt]
//
//      a.txt and b.txt are the two strategy lists, one strategy per line
//      as "board:tricks board:tricks ...", as written by the suitc_dump
//      program in this directory. If out.txt (SuitC's own result of the
//      merge) is given, the result is compared with it.
//
//  Robert Salita, 2026. MIT licence, see LICENSE in the repository root.

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _OPENMP
#include <omp.h>
#endif

#define MAXBOARDS   192             // SuitC has at most 162 boards (NBOARDS)
#define WORDS       ( MAXBOARDS / 64 )
#define MAXLEVELS   13              // at most 13 tricks
#define NCHUNKS     256

typedef struct {
    uint64_t mask[MAXLEVELS][WORDS];
    int      sum;                   // total number of tricks over all boards
} VEC;

typedef struct {
    long     n;
    int      *len;                  // number of boards of each strategy
    int      (*board)[MAXBOARDS];
    int      (*tricks)[MAXBOARDS];
} LIST;

static int nlevels;                 // levels in use: the highest trick count
static int nunion;                  // boards in the union of both lists
static int bitpos[256];             // board number -> bit position

static int popcount64( uint64_t x )
{
#if defined( _MSC_VER ) && defined( _M_X64 )
    return (int) __popcnt64( x );
#elif defined( __GNUC__ ) || defined( __clang__ )
    return __builtin_popcountll( x );
#else
    int n = 0;
    for ( ; x; x &= x - 1 )
        n++;
    return n;
#endif
}

static double seconds( void )
{
#ifdef _OPENMP
    return omp_get_wtime();
#else
    struct timespec ts;
    timespec_get( &ts, TIME_UTC );
    return ts.tv_sec + 1e-9 * ts.tv_nsec;
#endif
}

static void *xmalloc( size_t size )
{
    void *p = malloc( size ? size : 1 );
    if ( ! p ) {
        fprintf( stderr, "out of memory (%zu bytes)\n", size );
        exit( 1 );
    }
    return p;
}

static void read_list( const char *name, LIST *list )
{
    static char line[1 << 16];
    long cap = 1024;
    FILE *f = fopen( name, "r" );

    if ( ! f ) {
        fprintf( stderr, "cannot open %s\n", name );
        exit( 1 );
    }
    list->n = 0;
    list->len = xmalloc( cap * sizeof( int ) );
    list->board = xmalloc( cap * sizeof *list->board );
    list->tricks = xmalloc( cap * sizeof *list->tricks );
    while ( fgets( line, sizeof line, f ) ) {
        char *p = line;
        int b, t, used, k = 0;
        if ( list->n == cap ) {
            cap *= 2;
            list->len = realloc( list->len, cap * sizeof( int ) );
            list->board = realloc( list->board, cap * sizeof *list->board );
            list->tricks = realloc( list->tricks, cap * sizeof *list->tricks );
            if ( ! list->len || ! list->board || ! list->tricks ) {
                fprintf( stderr, "out of memory reading %s\n", name );
                exit( 1 );
            }
        }
        while ( k < MAXBOARDS && sscanf( p, "%d:%d%n", &b, &t, &used ) == 2 ) {
            if ( b < 0 || b > 255 || t < 0 || t > MAXLEVELS ) {
                fprintf( stderr, "%s: bad entry %d:%d\n", name, b, t );
                exit( 1 );
            }
            list->board[list->n][k] = b;
            list->tricks[list->n][k] = t;
            if ( t > nlevels )
                nlevels = t;
            k++;
            p += used;
        }
        if ( k )
            list->len[list->n++] = k;
    }
    fclose( f );
}

static int vec_sum( const VEC *x )
{
    int v, w, s = 0;
    for ( v = 0; v < nlevels; v++ )
        for ( w = 0; w < WORDS; w++ )
            s += popcount64( x->mask[v][w] );
    return s;
}

static void to_vec( const LIST *list, long i, VEC *x )
{
    int k, v;

    // boards that are absent from this strategy do not limit the minimum
    memset( x, 0, sizeof *x );
    for ( k = 0; k < nunion; k++ )
        for ( v = 0; v < nlevels; v++ )
            x->mask[v][k >> 6] |= (uint64_t) 1 << ( k & 63 );
    for ( k = 0; k < list->len[i]; k++ ) {
        int bit = bitpos[list->board[i][k]];
        for ( v = list->tricks[i][k]; v < nlevels; v++ )
            x->mask[v][bit >> 6] &= ~( (uint64_t) 1 << ( bit & 63 ) );
    }
    x->sum = vec_sum( x );
}

// x <= y on every board
static int dominated( const VEC *x, const VEC *y )
{
    int v, w;
    for ( v = 0; v < nlevels; v++ )
        for ( w = 0; w < WORDS; w++ )
            if ( x->mask[v][w] & ~y->mask[v][w] )
                return 0;
    return 1;
}

// sort order for the filters: more tricks first; ties in any order
static int by_total( const void *a, const void *b )
{
    return ( (const VEC *) b )->sum - ( (const VEC *) a )->sum;
}

// a total order, for comparing two lists of strategies
static int by_sum( const void *a, const void *b )
{
    const VEC *x = a, *y = b;
    if ( x->sum != y->sum )
        return y->sum - x->sum;
    // only the levels in use are set; the others are not initialised
    return memcmp( x->mask, y->mask, nlevels * sizeof x->mask[0] );
}

// sort-filter of a list that is sorted by_total; returns the number kept in out
static long filter_sorted( const VEC *in, long n, VEC *out )
{
    long i, k, nout = 0;
    for ( i = 0; i < n; i++ ) {
        int dom = 0;
        for ( k = nout - 1; k >= 0 && ! dom; k-- )
            dom = dominated( &in[i], &out[k] );
        if ( ! dom )
            out[nout++] = in[i];
    }
    return nout;
}

static long merge( const VEC *a, long na, const VEC *b, long nb, VEC **result, double *t_local )
{
    // group by the strategies of the smaller side, so that each group is large
    const VEC *g = na < nb ? a : b, *o = na < nb ? b : a;
    long ng = na < nb ? na : nb, no = na < nb ? nb : na;
    long j, p, ncand = 0, nkeep = 0, *gcount = xmalloc( ng * sizeof( long ) );
    long start[NCHUNKS+1], count[NCHUNKS];
    VEC **gout = xmalloc( ng * sizeof( VEC * ) ), *cand, *surv, *keep;
    char *dead;
    double t0 = seconds();

    // step 1: every group of pairs is filtered on its own
    #pragma omp parallel for schedule( dynamic, 1 )
    for ( j = 0; j < ng; j++ ) {
        long i, w, v;
        VEC *pairs = xmalloc( no * sizeof( VEC ) );
        for ( i = 0; i < no; i++ ) {
            for ( v = 0; v < nlevels; v++ )
                for ( w = 0; w < WORDS; w++ )
                    pairs[i].mask[v][w] = o[i].mask[v][w] & g[j].mask[v][w];
            pairs[i].sum = vec_sum( &pairs[i] );
        }
        qsort( pairs, no, sizeof( VEC ), by_total );
        gout[j] = xmalloc( no * sizeof( VEC ) );
        gcount[j] = filter_sorted( pairs, no, gout[j] );
        free( pairs );
    }
    for ( j = 0; j < ng; j++ )
        ncand += gcount[j];
    cand = xmalloc( ncand * sizeof( VEC ) );
    for ( ncand = 0, j = 0; j < ng; j++ ) {
        memcpy( &cand[ncand], gout[j], gcount[j] * sizeof( VEC ) );
        ncand += gcount[j];
        free( gout[j] );
    }
    free( gout );
    free( gcount );

    // step 2: sort the candidates by their number of tricks
    qsort( cand, ncand, sizeof( VEC ), by_total );
    *t_local = seconds() - t0;
    printf( "  candidates after the group filters: %ld\n", ncand );

    // step 3: filter each chunk, then check the survivors against the earlier chunks
    surv = xmalloc( ncand * sizeof( VEC ) );
    dead = calloc( ncand ? ncand : 1, 1 );
    for ( p = 0; p <= NCHUNKS; p++ )
        start[p] = ncand * p / NCHUNKS;
    #pragma omp parallel for schedule( dynamic, 1 )
    for ( p = 0; p < NCHUNKS; p++ )
        count[p] = filter_sorted( &cand[start[p]], start[p+1] - start[p], &surv[start[p]] );
    #pragma omp parallel for schedule( dynamic, 1 )
    for ( p = 1; p < NCHUNKS; p++ ) {
        long i, k, q;
        for ( i = start[p]; i < start[p] + count[p]; i++ )
            for ( q = p - 1; q >= 0 && ! dead[i]; q-- )
                for ( k = start[q]; k < start[q] + count[q] && ! dead[i]; k++ )
                    if ( dominated( &surv[i], &surv[k] ) )
                        dead[i] = 1;
    }
    keep = xmalloc( ( ncand ? ncand : 1 ) * sizeof( VEC ) );
    for ( p = 0; p < NCHUNKS; p++ ) {
        long i;
        for ( i = start[p]; i < start[p] + count[p]; i++ )
            if ( ! dead[i] )
                keep[nkeep++] = surv[i];
    }
    free( cand );
    free( surv );
    free( dead );
    *result = keep;
    return nkeep;
}

int main( int argc, char **argv )
{
    LIST la, lb, lo;
    VEC *a, *b, *r;
    long i, k, nr;
    int seen[256] = { 0 };
    double t0, t_local, t_total;

    if ( argc < 3 ) {
        fprintf( stderr, "usage: bitmask_merge a.txt b.txt [out.txt]\n" );
        return 2;
    }
    read_list( argv[1], &la );
    read_list( argv[2], &lb );
    if ( argc > 3 )
        read_list( argv[3], &lo );

    for ( i = 0; i < la.n; i++ )
        for ( k = 0; k < la.len[i]; k++ )
            seen[la.board[i][k]] = 1;
    for ( i = 0; i < lb.n; i++ )
        for ( k = 0; k < lb.len[i]; k++ )
            seen[lb.board[i][k]] = 1;
    for ( i = 0; i < 256; i++ )
        if ( seen[i] ) {
            if ( nunion == MAXBOARDS ) {
                fprintf( stderr, "more than %d boards\n", MAXBOARDS );
                return 1;
            }
            bitpos[i] = nunion++;
        }

    a = xmalloc( la.n * sizeof( VEC ) );
    b = xmalloc( lb.n * sizeof( VEC ) );
    for ( i = 0; i < la.n; i++ )
        to_vec( &la, i, &a[i] );
    for ( i = 0; i < lb.n; i++ )
        to_vec( &lb, i, &b[i] );

#ifdef _OPENMP
    printf( "threads: %d\n", omp_get_max_threads() );
#else
    printf( "threads: 1 (built without OpenMP)\n" );
#endif
    printf( "strategies: %ld x %ld = %ld pairs, %d boards, %d levels\n",
            la.n, lb.n, la.n * lb.n, nunion, nlevels );

    t0 = seconds();
    nr = merge( a, la.n, b, lb.n, &r, &t_local );
    t_total = seconds() - t0;
    printf( "  strategies kept: %ld\n", nr );
    printf( "  time: %.3f s (group filters and sort %.3f s, final filter %.3f s)\n",
            t_total, t_local, t_total - t_local );

    // compare with SuitC: both lists sorted the same way must be identical
    if ( argc > 3 ) {
        VEC *o = xmalloc( ( lo.n ? lo.n : 1 ) * sizeof( VEC ) );
        int same = lo.n == nr;
        for ( i = 0; i < lo.n; i++ ) {
            for ( k = 0; k < lo.len[i]; k++ )
                if ( ! seen[lo.board[i][k]] ) {
                    fprintf( stderr, "%s has a board that is in neither input\n", argv[3] );
                    return 1;
                }
            to_vec( &lo, i, &o[i] );
        }
        qsort( o, lo.n, sizeof( VEC ), by_sum );
        qsort( r, nr, sizeof( VEC ), by_sum );
        for ( i = 0; same && i < nr; i++ )
            same = by_sum( &o[i], &r[i] ) == 0;
        printf( "  SuitC kept %ld strategies: %s\n", lo.n, same ? "IDENTICAL" : "DIFFERENT" );
        if ( ! same ) {
            // walk both sorted lists and report what only one of them has
            long x = 0, y = 0, only_o = 0, only_r = 0, dup = 0;
            for ( i = 1; i < lo.n; i++ )
                dup += by_sum( &o[i-1], &o[i] ) == 0;
            while ( x < lo.n || y < nr ) {
                int c = x == lo.n ? 1 : y == nr ? -1 : by_sum( &o[x], &r[y] );
                if ( c == 0 ) { x++; y++; continue; }
                if ( c < 0 ) {
                    long dominator = -1;
                    for ( k = 0; k < nr && dominator < 0; k++ )
                        if ( dominated( &o[x], &r[k] ) )
                            dominator = k;
                    if ( only_o++ < 3 )
                        printf( "  only in SuitC: strategy with %d tricks, %s\n", o[x].sum,
                                dominator >= 0 ? "dominated by a kept strategy" : "not dominated" );
                    x++;
                }
                else {
                    if ( only_r++ < 3 )
                        printf( "  only here: strategy with %d tricks\n", r[y].sum );
                    y++;
                }
            }
            printf( "  only in SuitC: %ld, only here: %ld, duplicates in SuitC: %ld\n", only_o, only_r, dup );
        }
        return same ? 0 : 1;
    }
    return 0;
}
