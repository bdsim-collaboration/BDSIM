/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2020.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BDSIM_BDSBHTYPEDEFS_H
#define BDSIM_BDSBHTYPEDEFS_H

#include <boost/histogram.hpp>
#include <boost/variant.hpp>

typedef boost::histogram::axis::regular<double, boost::use_default, boost::use_default, boost::use_default> boost_histogram_linear_axis;
typedef boost::histogram::axis::regular<double, boost::histogram::axis::transform::log, boost::use_default, boost::use_default> boost_histogram_log_axis;

typedef boost::histogram::histogram<std::__1::tuple<boost_histogram_linear_axis, boost_histogram_linear_axis, boost_histogram_linear_axis, boost_histogram_linear_axis >, boost::histogram::storage_adaptor<std::__1::vector<double, std::__1::allocator<double> > > > boost_histogram_linear;
typedef boost::histogram::histogram<std::__1::tuple<boost_histogram_linear_axis, boost_histogram_linear_axis, boost_histogram_linear_axis, boost_histogram_log_axis    >, boost::histogram::storage_adaptor<std::__1::vector<double, std::__1::allocator<double> > > > boost_histogram_log;

typedef boost::variant<boost_histogram_linear_axis, boost_histogram_log_axis> boost_histogram_axes_variant;

class EnergyAxisIndexVisitor : public boost::static_visitor<int> {
public:
    int operator()(const boost_histogram_linear_axis& ax, const double energy) const { return ax.index(energy); }
    int operator()(const boost_histogram_log_axis& ax, const double energy) const { return ax.index(energy); }
};

#endif //BDSIM_BDSBHTYPEDEFS_H
