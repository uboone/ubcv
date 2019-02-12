# ======================================================================
#  larsoft main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/larsoft/ups/setup_for_development <-d|-p>
#  cmake [-DCMAKE_INSTALL_PREFIX=/install/path]
#        -DCMAKE_BUILD_TYPE=$CETPKG_TYPE
#        $CETPKG_SOURCE
#  make
#  make test
#  make install
#  make package (builds distribution tarfile)
# ======================================================================

# use cmake 2.8 or later
cmake_minimum_required (VERSION 2.8)

project(ubcv)

# cetbuildtools contains our cmake modules
SET ( CETBUILDTOOLS_VERSION $ENV{CETBUILDTOOLS_VERSION} )
IF (NOT CETBUILDTOOLS_VERSION)
    MESSAGE (FATAL_ERROR "ERROR: setup cetbuildtools to get the cmake modules")
ENDIF()


set(CMAKE_MODULE_PATH $ENV{CANVAS_ROOT_IO_DIR}/Modules
		      $ENV{CETBUILDTOOLS_DIR}/Modules
		      ${CMAKE_MODULE_PATH})

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS
  WERROR
  NO_UNDEFINED
  ALLOW_DEPRECATIONS
  EXTRA_FLAGS -pedantic -Wno-unused-local-typedefs
)

cet_report_compiler_flags()

# these are minimum required versions, not the actual product versions
find_ups_product( larcorealg )
find_ups_product( larcoreobj )
find_ups_product( lardataobj )
find_ups_product( lardataalg )
find_ups_product( lardata )
find_ups_product( cetbuildtools )
find_ups_boost( v1_53_0 )
find_ups_product( canvas )
find_ups_product( ubevt v1_00_00 )
find_ups_product( larevt v1_00_00 )
#find_ups_product( larcv v1_00_00 )
find_ups_product( art v1_00_00 )
find_ups_product( larcore )
find_ups_product( nutools )

# macros for dictionary and simple_plugin
include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

# load submodules
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
  # Update submodules as needed
  option(GIT_SUBMODULE "Check submodules during build" ON)
  if(GIT_SUBMODULE)
    message(STATUS "Submodule update")
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      RESULT_VARIABLE GIT_SUBMOD_RESULT)
    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
      message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
    endif()
  endif()
endif()

#--- check ubdlintegration submodule
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/ubcv/ubdlintegration/CMakeLists.txt")
  message(FATAL_ERROR "The ubdlintegration submodule was not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

# EXTERNAL non-UPS LIBS
#set( EXTERN_LIBDIR $ENV{HOME}/sl7_ubbuild02_libs )
set( EXTERN_LIBDIR $ENV{MRB_INSTALL} )

# --- ZeroMQ
set( ZeroMQ_DIR "${EXTERN_LIBDIR}/libzmq/install/share/cmake/ZeroMQ" )
find_package(ZeroMQ REQUIRED CONFIG)

set( cppzmq_DIR "${EXTERN_LIBDIR}/cppzmq/install/share/cmake/cppzmq" )
find_package(cppzmq REQUIRED CONFIG)

# --- LArCV
set( larcv_DIR "${EXTERN_LIBDIR}/ubdl/larcv/build/installed/lib/cmake/larcv" )
find_package(larcv REQUIRED CONFIG)

# --- UB LArCV APP
set( ublarcvapp_DIR "${EXTERN_LIBDIR}/ubdl/ublarcvapp/build/lib/cmake/ublarcvapp" )
find_package(ublarcvapp REQUIRED CONFIG)

# --- Torch
set( Torch_DIR "${EXTERN_LIBDIR}/libtorch/share/cmake/Torch" )
find_package( Torch CONFIG )

# ADD SOURCE CODE SUBDIRECTORIES HERE
add_subdirectory(ubcv)

# Unit tests.
add_subdirectory(test)

# ups - table and config files
add_subdirectory(ups)

# packaging utility
include(UseCPack)
