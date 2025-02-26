## ---------------------------------------------------------------------
##
## Copyright (C) 2012 - 2022 by the deal.II authors
##
## This file is part of the deal.II library.
##
## The deal.II library is free software; you can use it, redistribute
## it, and/or modify it under the terms of the GNU Lesser General
## Public License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
## The full text of the license can be found in the file LICENSE.md at
## the top level directory of deal.II.
##
## ---------------------------------------------------------------------


##                                                                    ##
#            The cmake build system for the deal.II project            #
#                                                                      #
#   See doc/readme.html and doc/developers/cmake-internals.html for    #
#   further details on how to use the cmake build system of deal.II.   #
##                                                                    ##

########################################################################
#                                                                      #
#                            Configuration:                            #
#                                                                      #
########################################################################

#
# General configuration for cmake:
#
message(STATUS "This is CMake ${CMAKE_VERSION}")
message(STATUS "")

cmake_minimum_required(VERSION 3.13.4)
#
# We support all policy changes up to version 3.11.
#
cmake_policy(VERSION 3.13.4)

if(POLICY CMP0075)
  # Introduced in CMake 3.12: Use CMAKE_REQUIRED_LIBRARIES also in include
  # file checks. We set the policy to NEW explicitly in order to avoid
  # spurious configure warnings.
  cmake_policy(SET CMP0075 NEW)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules/)

#
# Load all macros:
#
file(GLOB _macro_files "cmake/macros/*.cmake")
message(STATUS "Include ${CMAKE_SOURCE_DIR}/cmake/setup_external_macros.cmake")
include(${CMAKE_SOURCE_DIR}/cmake/setup_external_macros.cmake)
foreach(_file ${_macro_files})
  message(STATUS "Include ${_file}")
  include(${_file})
endforeach()

#
# Avoid verbose "Up-to-date" status information during installation:
#
set_if_empty(CMAKE_INSTALL_MESSAGE "LAZY")

#
# Check for the existence of various optional folders:
#
if(EXISTS ${CMAKE_SOURCE_DIR}/bundled/CMakeLists.txt)
  set(DEAL_II_HAVE_BUNDLED_DIRECTORY TRUE)
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/doc/CMakeLists.txt)
  set(DEAL_II_HAVE_DOC_DIRECTORY TRUE)
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/tests/CMakeLists.txt)
  set(DEAL_II_HAVE_TESTS_DIRECTORY TRUE)
endif()

#
# We have to initialize some cached variables before PROJECT is called, so
# do it at this point:
#
verbose_include(${CMAKE_SOURCE_DIR}/cmake/setup_cached_variables.cmake)

#
# Now, set the project and set up the rest:
#
project(deal.II CXX C)
enable_language_optional(Fortran)

verbose_include(${CMAKE_SOURCE_DIR}/cmake/setup_deal_ii.cmake)

verbose_include(${CMAKE_SOURCE_DIR}/cmake/setup_compiler_flags.cmake)

#
# Include information about bundled libraries:
#
if(DEAL_II_HAVE_BUNDLED_DIRECTORY)
  verbose_include(${CMAKE_SOURCE_DIR}/bundled/setup_bundled.cmake)
endif()

#
# Run all system checks:
#
file(GLOB _check_files "cmake/checks/*.cmake")
list(SORT _check_files)
foreach(_file ${_check_files})
  verbose_include(${_file})
endforeach()

verbose_include(${CMAKE_SOURCE_DIR}/cmake/setup_finalize.cmake)

#
# Feature configuration:
#
file(GLOB _configure_files "cmake/configure/configure_*.cmake")
list(SORT _configure_files) # make sure to include in alphabetical order
foreach(_file ${_configure_files})
  verbose_include(${_file})
endforeach()

verbose_include(${CMAKE_SOURCE_DIR}/cmake/setup_cpack.cmake)
verbose_include(${CMAKE_SOURCE_DIR}/cmake/setup_custom_targets.cmake)

########################################################################
#                                                                      #
#                     Compilation and installation:                    #
#                                                                      #
########################################################################

message(STATUS "")
message(STATUS "Configuring done. Proceed to target definitions now.")

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_subdirectory(cmake/scripts)
add_subdirectory(include)

if(DEAL_II_HAVE_DOC_DIRECTORY)
  add_subdirectory(doc) # has to be included after include
endif()

if(DEAL_II_HAVE_BUNDLED_DIRECTORY)
  add_subdirectory(bundled)
endif()
add_subdirectory(source) # has to be included after bundled

add_subdirectory(cmake/config) # has to be included after source
add_subdirectory(examples)

add_subdirectory(contrib/utilities)

if(DEAL_II_HAVE_TESTS_DIRECTORY)
  add_subdirectory(tests)
endif()

add_subdirectory(contrib/python-bindings) # has to be included after tests

verbose_include(${CMAKE_SOURCE_DIR}/cmake/setup_write_config.cmake)

#
# And finally, print the configuration:
#
file(READ ${CMAKE_BINARY_DIR}/summary.log DEAL_II_LOG_SUMMARY)
message("${DEAL_II_LOG_SUMMARY}")
