#[================================================================[.rst:

Name:    Findlarlite.cmake

Purpose: find_package module for ups product larlite.

Created: 31-Aug-2023  H. Greenlee

------------------------------------------------------------------

There is a larlite ups product and a larlite build from a ubdl ups product.
This module is currently configured to use the larlite build from ubdl.
To use the larlite ups product, you would need to change the dependency of this
package from ubdl to larlite and use the commented out lines of code in this module.

The larlite ups product defines the following environment variables,
which are used by this module.

LARLITE_COREDIR - Include path
LARLITE_LIBDIR  - Library path

The ubdl ups product defines the following environment variables,
which are used by this module.

LARLITE_INCDIR  - Include path
LARLITE_LIBDIR  - Library path

This module creates the following targets, corresponding to the libraries
in the library directory.
In the larlite build from ubdl, the shared library names start with
liblarlite instead of libLArLite.

larlite::Base       - libLArLite_Base.so
larlite::DataFormat - libLArLite_DataFormat.so
larlite::Analysis   - libLArLite_Analysis.so
larlite::LArUtil    - libLArLite_LArUtil.so

The larlite build in ubdl has additional libraries for which targets have
not yet been built here

#]================================================================]

# Don't do anything of this package has already been found.

if(NOT larlite_FOUND)

  # First hunt for the larlite include directory.

  message("Finding package larlite")
  #find_file(_larlite_h NAMES DataFormat HINTS ENV LARLITE_COREDIR NO_CACHE)
  find_file(_larlite_h NAMES larlite HINTS ENV LARLITE_INCDIR NO_CACHE)
  if(_larlite_h)
    get_filename_component(_larlite_include_dir ${_larlite_h} DIRECTORY)
    message("Found larlite include directory ${_larlite_include_dir}")
    set(larlite_FOUND TRUE)
  else()
    message("Could not find larlite include directory")
  endif()

  # Next hunt for the larlite libraries.

  if(larlite_FOUND)

    if(NOT TARGET larlite::Base)

      # Hunt for this library.

      #find_library(_larlite_lib_path LIBRARY NAMES LArLite_Base HINTS ENV LARLITE_LIBDIR REQUIRED NO_CACHE)
      find_library(_larlite_lib_path LIBRARY NAMES larlite_Base HINTS ENV LARLITE_LIBDIR REQUIRED NO_CACHE)
      message("Found larlite library ${_larlite_lib_path}")

      # Make target.

      message("Making target larlite::Base")
      add_library(larlite::Base SHARED IMPORTED)
      set_target_properties(larlite::Base PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_larlite_include_dir}"
        IMPORTED_LOCATION "${_larlite_lib_path}"
      )
      unset(_larlite_lib_path)
    endif()

    if(NOT TARGET larlite::DataFormat)

      # Hunt for this library.

      #find_library(_larlite_lib_path LIBRARY NAMES LArLite_DataFormat HINTS ENV LARLITE_LIBDIR REQUIRED NO_CACHE)
      find_library(_larlite_lib_path LIBRARY NAMES larlite_DataFormat HINTS ENV LARLITE_LIBDIR REQUIRED NO_CACHE)
      message("Found larlite library ${_larlite_lib_path}")

      # Make target.

      message("Making target larlite::DataFormat")
      add_library(larlite::DataFormat SHARED IMPORTED)
      set_target_properties(larlite::DataFormat PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_larlite_include_dir}"
        IMPORTED_LOCATION "${_larlite_lib_path}"
        INTERFACE_LINK_LIBRARIES "larlite::Base"
      )
      unset(_larlite_lib_path)
    endif()

    if(NOT TARGET larlite::Analysis)

      # Hunt for this library.

      #find_library(_larlite_lib_path LIBRARY NAMES LArLite_Analysis HINTS ENV LARLITE_LIBDIR REQUIRED NO_CACHE)
      find_library(_larlite_lib_path LIBRARY NAMES larlite_Analysis HINTS ENV LARLITE_LIBDIR REQUIRED NO_CACHE)
      message("Found larlite library ${_larlite_lib_path}")

      # Make target.

      message("Making target larlite::Analysis")
      add_library(larlite::Analysis SHARED IMPORTED)
      set_target_properties(larlite::Analysis PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_larlite_include_dir}"
        IMPORTED_LOCATION "${_larlite_lib_path}"
        INTERFACE_LINK_LIBRARIES "larlite::DataFormat"
      )
      unset(_larlite_lib_path)
    endif()

    if(NOT TARGET larlite::LArUtil)

      # Hunt for this library.

      #find_library(_larlite_lib_path LIBRARY NAMES LArLite_LArUtil HINTS ENV LARLITE_LIBDIR REQUIRED NO_CACHE)
      find_library(_larlite_lib_path LIBRARY NAMES larlite_LArUtil HINTS ENV LARLITE_LIBDIR REQUIRED NO_CACHE)
      message("Found larlite library ${_larlite_lib_path}")

      # Make target.

      message("Making target larlite::LArUtil")
      add_library(larlite::LArUtil SHARED IMPORTED)
      set_target_properties(larlite::LArUtil PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_larlite_include_dir}"
        IMPORTED_LOCATION "${_larlite_lib_path}"
      )
      unset(_larlite_lib_path)
    endif()
  endif()
endif()
