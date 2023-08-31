#[================================================================[.rst:

Name:    Findlarlite.cmake

Purpose: find_package module for ups product larlite.

Created: 31-Aug-2023  H. Greenlee

------------------------------------------------------------------

The larlite ups product defines the following environment variables,
which are used by this module.

LARLITE_COREDIR - Include path
LARLITE_LIBDIR  - Library path

This module creates the following targets, corresponding to the libraries
in the library directory.

larlite::Base       - libLArLite_Base.so
larlite::DataFormat - libLArLite_DataFormat.so
larlite::Analysis   - libLArLite_Analysis.so
larlite::LArUtil    - libLArLite_LArUtil.so


#]================================================================]

# First hunt for the larlite include directory.

if(NOT larlite_FOUND)
  message("Finding package larlite")
  find_file(_larlite_h NAMES DataFormat HINTS ENV LARLITE_COREDIR NO_CACHE)
  if(_larlite_h)
    get_filename_component(_larlite_include_dir ${_larlite_h} DIRECTORY)
    message("Found larlite include directory ${_larlite_include_dir}")
    set(larlite_FOUND TRUE)
  else()
    message("Could not find larlite include directory")
  endif()
endif()

# Next hunt for the larlite libraries.

if(larlite_FOUND)

  # Loop over libraries.

  foreach(_larlite_lib_name IN ITEMS Base DataFormat Analysis LArUtil )
    if(NOT TARGET larlite::${_larlite_lib_name})

      # Hunt for this library.

      find_library(_larlite_lib_path LIBRARY NAMES LArLite_${_larlite_lib_name} HINTS ENV LARLITE_LIBDIR REQUIRED NO_CACHE)
      message("Found larlite library ${_larlite_lib_path}")

    # Maybe make taret.

      message("Making target larlite::${_larlite_lib_name}")
      add_library(larlite::${_larlite_lib_name} SHARED IMPORTED)
      set_target_properties(larlite::${_larlite_lib_name} PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_larlite_include_dir}"
        IMPORTED_LOCATION "${_larlite_lib_path}"
      )
    endif()

    # End of loop over libraries.

    unset(_larlite_lib_path)
  endforeach()
endif()
