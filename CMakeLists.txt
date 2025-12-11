# ======================================================================
#  ubcv main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/lardataobj/ups/setup_for_development <-d|-p>
#  buildtool [-I /install/path] -bt[i] -jN
# ======================================================================

cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

find_package(cetmodules REQUIRED)
project(ubcv VERSION 10.15.00 LANGUAGES CXX)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic -Wno-unused-local-typedefs
)

cet_report_compiler_flags(REPORT_THRESHOLD VERBOSE)

cet_cmake_module_directories(Modules)

find_package( ubevt REQUIRED EXPORT )
find_package( larevt REQUIRED EXPORT )
find_package( nusimdata REQUIRED EXPORT )
find_package( ubreco REQUIRED EXPORT )
find_package( Torch QUIET EXPORT )
if(Torch_FOUND)
  find_package( larcv REQUIRED EXPORT )
  find_package( larlite REQUIRED EXPORT )
  find_package( ublarcvapp REQUIRED EXPORT )
  find_package( TorchScatter QUIET EXPORT )
endif()

# macros for dictionary and simple_plugin
include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

# ADD SOURCE CODE SUBDIRECTORIES HERE
add_subdirectory(ubcv)

# Cmake modules.
add_subdirectory(Modules)

# Unit tests.
add_subdirectory(test)

# ups - table and config files

# packaging utility
cet_cmake_config() ### MIGRATE-ADDED (migrate-3.21.01) - required cmd
