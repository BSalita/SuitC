//  name:
//      dump_merge
//
//  purpose:
//      dump_merge calls merge_quirk( d ). For large merges it first writes
//      the two QUIRK trees that are merged, and afterwards the result, one
//      strategy per line as "board:tricks board:tricks ...".
//
//      Only the suitc_dump program of experiments/bitmask-merge uses this
//      routine; there it replaces the call of merge_quirk in
//      convert_tree_to_quirk. SuitC itself is not changed.
//
//      Environment variables:
//          SUITC_DUMP_MIN_PAIRS    dump a merge when the product of the number
//                                  of strategies is at least this (default 1000000)
//          SUITC_DUMP_MAX          the maximum number of merges dumped (default 3)
//
//      The files dump<n>_a.txt, dump<n>_b.txt and dump<n>_out.txt are written
//      in the current directory.
//
//  parameters:
//      d               in      the depth in the NODE tree
//
//  Robert Salita, 2026. MIT licence, see LICENSE in the repository root.

#include "header.h"

static void write_quirk( int d, const char *name )
{
    int           k, l = 0;
    struct TRICKS v[NBOARDS];
    struct QUIRKD *lquirkd[NBOARDS], *lquirkdl[NBOARDS];
    FILE          *f = fopen( name, "w" );

    if ( ! f ) {
        printf( "dump_merge: cannot write %s\n", name );
        exit( 1 );
    }

    //  walk the QUIRK tree as count_quirk does; each leaf is one strategy
    lquirkdl[0] = &quirkdl[d];
    lquirkd[0] = lquirkdl[0]->h ? lquirkdl[0]->h->d : NIL;
    while ( lquirkd[l] ) {
        for ( k = lquirkd[l]->il; k <= lquirkd[l]->ih; k++ )
            v[k] = lquirkdl[l]->h->t[k];
        if ( lquirkd[l]->d ) {
            l++;
            lquirkdl[l] = lquirkdl[l-1];
            lquirkd[l] = lquirkd[l-1]->d;
        }
        else {
            for ( k = 0; k <= lquirkd[l]->ih; k++ )
                fprintf( f, "%d:%d ", v[k].i, v[k].ns );
            fprintf( f, "\n" );
            while ( l && ! lquirkd[l]->h )
                l--;
            lquirkdl[l] = lquirkd[l];
            lquirkd[l] = lquirkdl[l]->h ? lquirkdl[l]->h->d : NIL;
        }
    }
    fclose( f );
}

void dump_merge( int d )
{
    static int ndumped = 0;
    char       name[64], *env;
    long       minpairs = 1000000L, maxdumps = 3, na, nb;
    int        dump;

    if ( ( env = getenv( "SUITC_DUMP_MIN_PAIRS" ) ) != NIL )
        minpairs = atol( env );
    if ( ( env = getenv( "SUITC_DUMP_MAX" ) ) != NIL )
        maxdumps = atol( env );

    na = count_quirk( d-1 );
    nb = count_quirk( d );
    dump = ndumped < maxdumps && na * nb >= minpairs;
    if ( dump ) {
        printf( "dump_merge %d: depth %d, %ld x %ld strategies\n", ndumped, d, na, nb );
        sprintf( name, "dump%d_a.txt", ndumped );
        write_quirk( d-1, name );
        sprintf( name, "dump%d_b.txt", ndumped );
        write_quirk( d, name );
    }

    //  the merged tree replaces quirkdl[d-1]
    merge_quirk( d );

    if ( dump ) {
        sprintf( name, "dump%d_out.txt", ndumped );
        write_quirk( d-1, name );
        ndumped++;
    }
}
