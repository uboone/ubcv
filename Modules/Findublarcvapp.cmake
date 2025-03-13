#[================================================================[.rst:

Name:    Findublarcvapp.cmake

Purpose: find_package module for ups product ublarcvapp.

Created: 12-March-2025  M. Rosenberg

------------------------------------------------------------------

The ublarcvapp build in the ubdl ups product defines the following
environment variables, which are used by this module.

UBLARCVAPP_INCDIR  - Include path
UBLARCVAPP_LIBDIR  - Library path

This module creates the following targets, corresponding to the libraries
in the library directory.

ublarcvapp::MCTools - libLArCVApp_MCTools.so
ublarcvapp::dbscan  - libLArCVApp_dbscan.so

There are additional ublarcvapp libraries for which targets have
not yet been built here

#]================================================================]

# Don't do anything of this package has already been found.

if(NOT ublarcvapp_FOUND)

  # First hunt for the ublarcvapp include directory.

  message("Finding package ublarcvapp")
  find_file(_ublarcvapp_h NAMES ublarcvapp HINTS ENV UBLARCVAPP_INCDIR NO_CACHE)
  if(_ublarcvapp_h)
    get_filename_component(_ublarcvapp_include_dir ${_ublarcvapp_h} DIRECTORY)
    message("Found ublarcvapp include directory ${_ublarcvapp_include_dir}")
    set(ublarcvapp_FOUND TRUE)
  else()
    message("Could not find ublarcvapp include directory")
  endif()

  # Next hunt for the ublarcvapp libraries.

  if(ublarcvapp_FOUND)

    if(NOT TARGET ublarcvapp::MCTools)

      # Hunt for this library.

      find_library(_ublarcvapp_lib_path LIBRARY NAMES LArCVApp_MCTools HINTS ENV UBLARCVAPP_LIBDIR REQUIRED NO_CACHE)
      message("Found ublarcvapp library ${_ublarcvapp_lib_path}")

      # Make target.

      message("Making target ublarcvapp::MCTools")
      add_library(ublarcvapp::MCTools SHARED IMPORTED)
      set_target_properties(ublarcvapp::MCTools PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_ublarcvapp_include_dir}"
        IMPORTED_LOCATION "${_ublarcvapp_lib_path}"
      )
      unset(_ublarcvapp_lib_path)
    endif()

    if(NOT TARGET ublarcvapp::dbscan)

      # Hunt for this library.

      find_library(_ublarcvapp_lib_path LIBRARY NAMES LArCVApp_dbscan HINTS ENV UBLARCVAPP_LIBDIR REQUIRED NO_CACHE)
      message("Found ublarcvapp library ${_ublarcvapp_lib_path}")

      # Make target.

      message("Making target ublarcvapp::dbscan")
      add_library(ublarcvapp::dbscan SHARED IMPORTED)
      set_target_properties(ublarcvapp::dbscan PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_ublarcvapp_include_dir}"
        IMPORTED_LOCATION "${_ublarcvapp_lib_path}"
      )
      unset(_ublarcvapp_lib_path)
    endif()

  endif()
endif()
