# Experimental bit-mask merge; see README.md. Not part of the SuitC build.
#
#   cmake -S experiments/bitmask-merge -B build-bitmask
#   cmake --build build-bitmask --config Release
cmake_minimum_required(VERSION 3.21)
project(SuitCBitmaskMerge LANGUAGES C)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(SUITC_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")

# ---------------------------------------------------------------- bitmask_merge
add_executable(bitmask_merge bitmask_merge.c)
set_target_properties(bitmask_merge PROPERTIES C_STANDARD 11 C_STANDARD_REQUIRED ON)
if(MSVC)
  target_compile_definitions(bitmask_merge PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
find_package(OpenMP COMPONENTS C)
if(OpenMP_C_FOUND)
  target_link_libraries(bitmask_merge PRIVATE OpenMP::OpenMP_C)
else()
  message(WARNING "OpenMP not found: bitmask_merge runs on one thread")
endif()

# ---------------------------------------------------------------- suitc_dump
# SuitC with convert_tree_to_quirk calling dump_merge instead of merge_quirk.
file(GLOB SUITC_SOURCES CONFIGURE_DEPENDS "${SUITC_ROOT}/src/*.c")
list(FILTER SUITC_SOURCES EXCLUDE REGEX "convert_tree_to_quirk\\.c$")

set(convert_in "${SUITC_ROOT}/src/convert_tree_to_quirk.c")
set(convert_out "${CMAKE_CURRENT_BINARY_DIR}/generated/convert_tree_to_quirk.c")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${convert_in}")
file(READ "${convert_in}" convert)
string(REPLACE "merge_quirk( d );" "dump_merge( d );" patched "${convert}")
if(patched STREQUAL convert)
  message(FATAL_ERROR "merge_quirk( d ); not found in ${convert_in}")
endif()
string(REPLACE "#include \"header.h\"" "#include \"header.h\"\nvoid dump_merge( int d );" patched "${patched}")
file(WRITE "${convert_out}" "${patched}")

set(extern_h "${CMAKE_CURRENT_BINARY_DIR}/generated/extern.h")
add_custom_command(
  OUTPUT "${extern_h}"
  COMMAND "${CMAKE_COMMAND}" -DIN=${SUITC_ROOT}/src/global.h -DOUT=${extern_h}
          -P "${SUITC_ROOT}/cmake/generate_extern.cmake"
  DEPENDS "${SUITC_ROOT}/src/global.h" "${SUITC_ROOT}/cmake/generate_extern.cmake"
  COMMENT "Generating extern.h")

add_executable(suitc_dump ${SUITC_SOURCES} "${convert_out}" dump_merge.c "${extern_h}")
set_target_properties(suitc_dump PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED ON)
target_include_directories(suitc_dump PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated" "${SUITC_ROOT}/src")
target_compile_definitions(suitc_dump PRIVATE SUITCAPP=NO BLANK=NO)
if(MSVC)
  target_compile_definitions(suitc_dump PRIVATE _CRT_SECURE_NO_WARNINGS)
  target_compile_options(suitc_dump PRIVATE /W1 /wd4996)
  target_link_options(suitc_dump PRIVATE /STACK:8388608)
elseif(MINGW)
  target_link_options(suitc_dump PRIVATE -Wl,--stack,8388608)
endif()
if(NOT MSVC)
  target_link_libraries(suitc_dump PRIVATE m)
endif()
