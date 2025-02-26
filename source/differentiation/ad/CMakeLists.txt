## ---------------------------------------------------------------------
##
## Copyright (C) 2017 - 2019 by the deal.II authors
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
  ad_drivers.cc
  ad_helpers.cc
  adolc_number_types.cc
  sacado_number_types.cc
  )

set(_inst
  ad_drivers.inst.in
  ad_drivers.inst1.in
  ad_drivers.inst2.in
  ad_helpers.inst.in
  ad_helpers.inst1.in
  ad_helpers.inst2.in
  adolc_number_types.inst.in
  sacado_number_types.inst1.in
  sacado_number_types.inst2.in
  )

file(GLOB _header
  ${CMAKE_SOURCE_DIR}/include/deal.II/differentiation/ad/*.h
  )

define_object_library(object_differentiation_ad OBJECT ${_src} ${_header} ${_inst})
expand_instantiations(object_differentiation_ad "${_inst}")
