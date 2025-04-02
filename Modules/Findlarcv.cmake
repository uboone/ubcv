#[================================================================[.rst:

Name:    Findlarcv.cmake

Purpose: find_package module for ups product larcv.

Created: 30-Aug-2023  H. Greenlee

------------------------------------------------------------------

The larcv and ubdl ups products defines the following environment variables,
which are used by this module.

LARCV_INCDIR - Include path
LARCV_LIBDIR - Library path

The commented out code would create two targets from the larcv ups product,
corresponding to the two libraries in the library directory.
To use these targets, you would need to change the depedency of this package
from ubdl to larcv and uncomment out the corresponding code here

larcv::larcv - liblarcv.so
larcv::ANN   - libANN.so

This module creates ten targets from the larcv build in the ubdl ups product,
corresponding to the ten libraries in the library directory

larcv::ImageMod   - libLArCVAppImageMod.so
larcv::Base       - libLArCVCoreBase.so
larcv::CPPUtil    - libLArCVCoreCPPUtil.so
larcv::CVUtil     - libLArCVCoreCVUtil.so
larcv::DataFormat - libLArCVCoreDataFormat.so
larcv::JSON       - libLArCVCoreJSON.so
larcv::Processor  - libLArCVCoreProcessor.so
larcv::PyUtil     - libLArCVCorePyUtil.so
larcv::ROOTUtil   - libLArCVCoreROOTUtil.so
larcv::TorchUtil  - libLArCVCoreTorchUtil.so

#]================================================================]

# Don't do anything of this package has already been found.

if(NOT larcv_FOUND)

  # First hunt for the larcv include directory.

  message("Finding package larcv")
  #find_file(_larcv_h NAMES DataFormat HINTS ENV LARCV_INCDIR NO_CACHE)
  find_file(_larcv_h NAMES larcv HINTS ENV LARCV_INCDIR NO_CACHE)
  if(_larcv_h)
    get_filename_component(_larcv_include_dir ${_larcv_h} DIRECTORY)
    message("Found larcv include directory ${_larcv_include_dir}")
    set(larcv_FOUND TRUE)
  else()
    message("Could not find larcv include directory")
  endif()

  # Next hunt for the larcv libraries.

  if(larcv_FOUND)

    if(NOT TARGET larcv::ImageMod)

      # Hunt for this library.

      find_library(_larcv_lib_path LIBRARY NAMES LArCVAppImageMod HINTS ENV LARCV_LIBDIR REQUIRED NO_CACHE)
      message("Found larcv library ${_larcv_lib_path}")

      # Make target.

      message("Making target larcv::ImageMod")
      add_library(larcv::ImageMod SHARED IMPORTED)
      set_target_properties(larcv::ImageMod PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_larcv_include_dir}"
        IMPORTED_LOCATION "${_larcv_lib_path}"
      )
      unset(_larcv_lib_path)
    endif()

    if(NOT TARGET larcv::Base)

      # Hunt for this library.

      find_library(_larcv_lib_path LIBRARY NAMES LArCVCoreBase HINTS ENV LARCV_LIBDIR REQUIRED NO_CACHE)
      message("Found larcv library ${_larcv_lib_path}")

      # Make target.

      message("Making target larcv::Base")
      add_library(larcv::Base SHARED IMPORTED)
      set_target_properties(larcv::Base PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_larcv_include_dir}"
        IMPORTED_LOCATION "${_larcv_lib_path}"
      )
      unset(_larcv_lib_path)
    endif()

    if(NOT TARGET larcv::CPPUtil)

      # Hunt for this library.

      find_library(_larcv_lib_path LIBRARY NAMES LArCVCoreCPPUtil HINTS ENV LARCV_LIBDIR REQUIRED NO_CACHE)
      message("Found larcv library ${_larcv_lib_path}")

      # Make target.

      message("Making target larcv::CPPUtil")
      add_library(larcv::CPPUtil SHARED IMPORTED)
      set_target_properties(larcv::CPPUtil PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_larcv_include_dir}"
        IMPORTED_LOCATION "${_larcv_lib_path}"
      )
      unset(_larcv_lib_path)
    endif()

    if(NOT TARGET larcv::CVUtil)

      # Hunt for this library.

      find_library(_larcv_lib_path LIBRARY NAMES LArCVCoreCVUtil HINTS ENV LARCV_LIBDIR REQUIRED NO_CACHE)
      message("Found larcv library ${_larcv_lib_path}")

      # Make target.

      message("Making target larcv::CVUtil")
      add_library(larcv::CVUtil SHARED IMPORTED)
      set_target_properties(larcv::CVUtil PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_larcv_include_dir}"
        IMPORTED_LOCATION "${_larcv_lib_path}"
      )
      unset(_larcv_lib_path)
    endif()

    if(NOT TARGET larcv::DataFormat)

      # Hunt for this library.

      find_library(_larcv_lib_path LIBRARY NAMES LArCVCoreDataFormat HINTS ENV LARCV_LIBDIR REQUIRED NO_CACHE)
      message("Found larcv library ${_larcv_lib_path}")

      # Make target.

      message("Making target larcv::DataFormat")
      add_library(larcv::DataFormat SHARED IMPORTED)
      set_target_properties(larcv::DataFormat PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_larcv_include_dir}"
        IMPORTED_LOCATION "${_larcv_lib_path}"
      )
      unset(_larcv_lib_path)
    endif()

    if(NOT TARGET larcv::JSON)

      # Hunt for this library.

      find_library(_larcv_lib_path LIBRARY NAMES LArCVCoreJSON HINTS ENV LARCV_LIBDIR REQUIRED NO_CACHE)
      message("Found larcv library ${_larcv_lib_path}")

      # Make target.

      message("Making target larcv::JSON")
      add_library(larcv::JSON SHARED IMPORTED)
      set_target_properties(larcv::JSON PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_larcv_include_dir}"
        IMPORTED_LOCATION "${_larcv_lib_path}"
      )
      unset(_larcv_lib_path)
    endif()

    if(NOT TARGET larcv::Processor)

      # Hunt for this library.

      find_library(_larcv_lib_path LIBRARY NAMES LArCVCoreProcessor HINTS ENV LARCV_LIBDIR REQUIRED NO_CACHE)
      message("Found larcv library ${_larcv_lib_path}")

      # Make target.

      message("Making target larcv::Processor")
      add_library(larcv::Processor SHARED IMPORTED)
      set_target_properties(larcv::Processor PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_larcv_include_dir}"
        IMPORTED_LOCATION "${_larcv_lib_path}"
      )
      unset(_larcv_lib_path)
    endif()

    if(NOT TARGET larcv::PyUtil)

      # Hunt for this library.

      find_library(_larcv_lib_path LIBRARY NAMES LArCVCorePyUtil HINTS ENV LARCV_LIBDIR REQUIRED NO_CACHE)
      message("Found larcv library ${_larcv_lib_path}")

      # Make target.

      message("Making target larcv::PyUtil")
      add_library(larcv::PyUtil SHARED IMPORTED)
      set_target_properties(larcv::PyUtil PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_larcv_include_dir}"
        IMPORTED_LOCATION "${_larcv_lib_path}"
      )
      unset(_larcv_lib_path)
    endif()

    if(NOT TARGET larcv::ROOTUtil)

      # Hunt for this library.

      find_library(_larcv_lib_path LIBRARY NAMES LArCVCoreROOTUtil HINTS ENV LARCV_LIBDIR REQUIRED NO_CACHE)
      message("Found larcv library ${_larcv_lib_path}")

      # Make target.

      message("Making target larcv::ROOTUtil")
      add_library(larcv::ROOTUtil SHARED IMPORTED)
      set_target_properties(larcv::ROOTUtil PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_larcv_include_dir}"
        IMPORTED_LOCATION "${_larcv_lib_path}"
      )
      unset(_larcv_lib_path)
    endif()

    if(NOT TARGET larcv::TorchUtil)

      # Hunt for this library.

      find_library(_larcv_lib_path LIBRARY NAMES LArCVCoreTorchUtil HINTS ENV LARCV_LIBDIR REQUIRED NO_CACHE)
      message("Found larcv library ${_larcv_lib_path}")

      # Make target.

      message("Making target larcv::TorchUtil")
      add_library(larcv::TorchUtil SHARED IMPORTED)
      set_target_properties(larcv::TorchUtil PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_larcv_include_dir}"
        IMPORTED_LOCATION "${_larcv_lib_path}"
      )
      unset(_larcv_lib_path)
    endif()

    #if(NOT TARGET larcv::larcv)
    #
    #  # Hunt for this library.
    #
    #  find_library(_larcv_lib_path LIBRARY NAMES larcv HINTS ENV LARCV_LIBDIR REQUIRED NO_CACHE)
    #  message("Found larcv library ${_larcv_lib_path}")
    #
    #  # Make taret.
    #
    #  message("Making target larcv::larcv")
    #  add_library(larcv::larcv SHARED IMPORTED)
    #  set_target_properties(larcv::larcv PROPERTIES
    #    INTERFACE_INCLUDE_DIRECTORIES "${_larcv_include_dir}"
    #    IMPORTED_LOCATION "${_larcv_lib_path}"
    #  )
    #  unset(_larcv_lib_path)
    #endif()
    #
    #if(NOT TARGET larcv::ANN)
    #
    #  # Hunt for this library.
    #
    #  find_library(_larcv_lib_path LIBRARY NAMES larcv HINTS ENV LARCV_LIBDIR REQUIRED NO_CACHE)
    #  message("Found larcv library ${_larcv_lib_path}")
    #
    #  # Make taret.
    #
    #  message("Making target larcv::ANN")
    #  add_library(larcv::ANN SHARED IMPORTED)
    #  set_target_properties(larcv::ANN PROPERTIES
    #    INTERFACE_INCLUDE_DIRECTORIES "${_larcv_include_dir}"
    #    IMPORTED_LOCATION "${_larcv_lib_path}"
    #  )
    #  unset(_larcv_lib_path)
    #endif()

  endif()
endif()
