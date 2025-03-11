#[================================================================[.rst:

Name:    Findlarcv.cmake

Purpose: find_package module for ups product larcv.

Created: 30-Aug-2023  H. Greenlee

------------------------------------------------------------------

The larcv ups product defines the following environment variables,
which are used by this module.

LARCV_INCDIR - Include path
LARCV_LIBDIR - Library path

This module creates two targets, corresponding to the two libraries
in the library directory.

larcv::larcv - liblarcv.so
larcv::ANN   - libANN.so

#]================================================================]

# Don't do anything of this package has already been found.

if(NOT larcv_FOUND)

  # First hunt for the larcv include directory.

  message("Finding package larcv")
  find_file(_larcv_h NAMES DataFormat HINTS ENV LARCV_INCDIR NO_CACHE)
  if(_larcv_h)
    get_filename_component(_larcv_include_dir ${_larcv_h} DIRECTORY)
    message("Found larcv include directory ${_larcv_include_dir}")
    set(larcv_FOUND TRUE)
  else()
    message("Could not find larcv include directory")
  endif()

  # Next hunt for the larcv libraries.

  if(larcv_FOUND)

    if(NOT TARGET larcv::larcv)

      # Hunt for this library.

      find_library(_larcv_lib_path LIBRARY NAMES larcv HINTS ENV LARCV_LIBDIR REQUIRED NO_CACHE)
      message("Found larcv library ${_larcv_lib_path}")

      # Make taret.

      message("Making target larcv::larcv")
      add_library(larcv::larcv SHARED IMPORTED)
      set_target_properties(larcv::larcv PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_larcv_include_dir}"
        IMPORTED_LOCATION "${_larcv_lib_path}"
      )
      unset(_larcv_lib_path)
    endif()

    if(NOT TARGET larcv::ANN)

      # Hunt for this library.

      find_library(_larcv_lib_path LIBRARY NAMES larcv HINTS ENV LARCV_LIBDIR REQUIRED NO_CACHE)
      message("Found larcv library ${_larcv_lib_path}")

      # Make taret.

      message("Making target larcv::ANN")
      add_library(larcv::ANN SHARED IMPORTED)
      set_target_properties(larcv::ANN PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_larcv_include_dir}"
        IMPORTED_LOCATION "${_larcv_lib_path}"
      )
      unset(_larcv_lib_path)
    endif()
  endif()
endif()
