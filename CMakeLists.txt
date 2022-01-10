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

cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(ubcv VERSION 09.40.00 LANGUAGES CXX)

# cetbuildtools contains our cmake modules
find_package(cetbuildtools REQUIRED)

list(APPEND CMAKE_MODULE_PATH $ENV{CANVAS_ROOT_IO_DIR}/Modules)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic -Wno-unused-local-typedefs
)

cet_report_compiler_flags()

# these are minimum required versions, not the actual product versions
find_ups_product( larcorealg )
find_ups_product( larcoreobj )
find_ups_product( lardataobj )
find_ups_product( lardataalg )
find_ups_product( lardata )
find_ups_boost( )
find_ups_product( canvas )
find_ups_product( ubevt )
find_ups_product( ubcore )
find_ups_product( larevt )
find_ups_product( larcv )
find_ups_product( art )
find_ups_product(art_root_io)
find_ups_product( larcore )
find_ups_product( nutools )
find_ups_product( nusimdata )
find_ups_product( ubobj )
find_ups_root()

# macros for dictionary and simple_plugin
include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

# activate this to build SSNET/DL Integration
# commented out for now until ready
# set( USE_DL_INTEGRATION 1 )

if ( USE_DL_INTEGRATION )

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
  # below is for dev, but we expect these to be imported via UPS product in the near future
  set( EXTERN_LIBDIR $ENV{MRB_INSTALL} )
  #set( UPS_PRODUCT_DIR /uboone/app/users/tmw/ups_dev/products ) # for development with new ups products

  # --- ZeroMQ
  set( ZeroMQ_DIR "${UPS_PRODUCT_DIR}/libzmq/v4_3_1/Linux64bit+3.10-2.17_e17_prof/share/cmake/ZeroMQ" )
  find_package(ZeroMQ REQUIRED CONFIG)

  set( cppzmq_DIR "${UPS_PRODUCT_DIR}/cppzmq/v4_3_0/Linux64bit+3.10-2.17_e17_prof/share/cmake/cppzmq" )
  find_package(cppzmq REQUIRED CONFIG)

  # --- LArCV
  set( larcv_DIR "${UPS_PRODUCT_DIR}/ubdl/v1_0_0/Linux64bit+3.10-2.17_e17_prof/larcv/build/installed/lib/cmake/larcv" )
  find_package(larcv REQUIRED CONFIG)

  # --- UB LArCV APP
  set( ublarcvapp_DIR "${UPS_PRODUCT_DIR}/ubdl/v1_0_0/Linux64bit+3.10-2.17_e17_prof/ublarcvapp/build/lib/cmake/ublarcvapp" )
  find_package(ublarcvapp REQUIRED CONFIG)

  # --- Torch
  #set( Torch_DIR "${EXTERN_LIBDIR}/libtorch/share/cmake/Torch" )
  set( Torch_DIR "${UPS_PRODUCT_DIR}/libtorch/v1_0_1/Linux64bit+3.10-2.17_e17_prof_openblas_nonnpack/share/cmake/Torch" )
  find_package( Torch CONFIG )

endif()

# ADD SOURCE CODE SUBDIRECTORIES HERE
add_subdirectory(ubcv)


# Unit tests.
add_subdirectory(test)

# ups - table and config files
add_subdirectory(ups)

# packaging utility
include(UseCPack)
