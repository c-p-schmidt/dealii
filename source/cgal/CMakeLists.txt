## ---------------------------------------------------------------------
##
## Copyright (C) 2022 by the deal.II authors
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

set(_src
  surface_mesh.cc
  intersections.cc
)

set(_inst
surface_mesh.inst.in
intersections.inst.in
)

file(GLOB _header
  ${CMAKE_SOURCE_DIR}/include/deal.II/cgal/*.h
  )

define_object_library(object_cgal OBJECT ${_src} ${_header} ${_inst})
expand_instantiations(object_cgal "${_inst}")
