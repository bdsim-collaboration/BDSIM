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

#include "BDSBH4D.hh"

#include <string>

templateClassImp(BDSBH4D)

template <>
BDSBH4D<boost_histogram_linear>::BDSBH4D()
{
    h = boost::histogram::make_histogram_with(std::vector<double>(),boost::histogram::axis::regular<double> {3, 0.0, 1.0, "x"},
                                              boost::histogram::axis::regular<double> {3, 0.0, 1.0, "y"},
                                              boost::histogram::axis::regular<double> {3, 0.0, 1.0, "z"},
                                              boost::histogram::axis::regular<double> {3, 1.0, 230.0, "energy"});

    h_err = boost::histogram::make_histogram_with(std::vector<double>(),boost::histogram::axis::regular<double> {3, 0.0, 1.0, "x"},
                                                  boost::histogram::axis::regular<double> {3, 0.0, 1.0, "y"},
                                                  boost::histogram::axis::regular<double> {3, 0.0, 1.0, "z"},
                                                  boost::histogram::axis::regular<double> {3, 1.0, 230.0, "energy"});

    h_nxbins = 3;
    h_nybins = 3;
    h_nzbins = 3;
    h_nebins = 3;
    h_xmin = 0;
    h_xmax = 1;
    h_ymin = 0;
    h_ymax = 1;
    h_zmin = 0;
    h_zmax = 1;
    h_emin = 1;
    h_emax = 230;

}

template <>
BDSBH4D<boost_histogram_log>::BDSBH4D()
{
    h = boost::histogram::make_histogram_with(std::vector<double>(),boost::histogram::axis::regular<double> {3, 0.0, 1.0, "x"},
                                              boost::histogram::axis::regular<double> {3, 0.0, 1.0, "y"},
                                              boost::histogram::axis::regular<double> {3, 0.0, 1.0, "z"},
                                              boost::histogram::axis::regular<double, boost::histogram::axis::transform::log> {3, 1.0, 230.0, "energy"});

    h_err = boost::histogram::make_histogram_with(std::vector<double>(),boost::histogram::axis::regular<double> {3, 0.0, 1.0, "x"},
                                                  boost::histogram::axis::regular<double> {3, 0.0, 1.0, "y"},
                                                  boost::histogram::axis::regular<double> {3, 0.0, 1.0, "z"},
                                                  boost::histogram::axis::regular<double, boost::histogram::axis::transform::log> {3, 1.0, 230.0, "energy"});

    h_nxbins = 3;
    h_nybins = 3;
    h_nzbins = 3;
    h_nebins = 3;
    h_xmin = 0;
    h_xmax = 1;
    h_ymin = 0;
    h_ymax = 1;
    h_zmin = 0;
    h_zmax = 1;
    h_emin = 1;
    h_emax = 230;

}

template <>
BDSBH4D<boost_histogram_linear>::BDSBH4D(std::string& name, std::string& title, const std::string& eScale,
                 unsigned int nxbins, double xmin, double xmax,
                 unsigned int nybins, double ymin, double ymax,
                 unsigned int nzbins, double zmin, double zmax,
                 unsigned int nebins, double emin, double emax)
{
    h = boost::histogram::make_histogram_with(std::vector<double>(),boost::histogram::axis::regular<double> {nxbins, xmin, xmax, "x"},
                                              boost::histogram::axis::regular<double> {nybins, ymin, ymax, "y"},
                                              boost::histogram::axis::regular<double> {nzbins, zmin, zmax, "z"},
                                              boost::histogram::axis::regular<double> {nebins, emin, emax, "energy"});

    h_err = boost::histogram::make_histogram_with(std::vector<double>(),boost::histogram::axis::regular<double> {nxbins, xmin, xmax, "x"},
                                                  boost::histogram::axis::regular<double> {nybins, ymin, ymax, "y"},
                                                  boost::histogram::axis::regular<double> {nzbins, zmin, zmax, "z"},
                                                  boost::histogram::axis::regular<double> {nebins, emin, emax, "energy"});

    h_nxbins = nxbins;
    h_nybins = nybins;
    h_nzbins = nzbins;
    h_nebins = nebins;
    h_xmin = xmin;
    h_xmax = xmax;
    h_ymin = ymin;
    h_ymax = ymax;
    h_zmin = zmin;
    h_zmax = zmax;
    h_emin = emin;
    h_emax = emax;
    h_name = name;
    h_title = title;
    h_escale = eScale;

}

template <>
BDSBH4D<boost_histogram_log>::BDSBH4D(std::string& name, std::string& title, const std::string& eScale,
                    unsigned int nxbins, double xmin, double xmax,
                    unsigned int nybins, double ymin, double ymax,
                    unsigned int nzbins, double zmin, double zmax,
                    unsigned int nebins, double emin, double emax)
{
    h = boost::histogram::make_histogram_with(std::vector<double>(),boost::histogram::axis::regular<double> {nxbins, xmin, xmax, "x"},
                                              boost::histogram::axis::regular<double> {nybins, ymin, ymax, "y"},
                                              boost::histogram::axis::regular<double> {nzbins, zmin, zmax, "z"},
                                              boost::histogram::axis::regular<double, boost::histogram::axis::transform::log> {nebins, emin, emax, "energy"});

    h_err = boost::histogram::make_histogram_with(std::vector<double>(),boost::histogram::axis::regular<double> {nxbins, xmin, xmax, "x"},
                                                  boost::histogram::axis::regular<double> {nybins, ymin, ymax, "y"},
                                                  boost::histogram::axis::regular<double> {nzbins, zmin, zmax, "z"},
                                                  boost::histogram::axis::regular<double, boost::histogram::axis::transform::log> {nebins, emin, emax, "energy"});

    h_nxbins = nxbins;
    h_nybins = nybins;
    h_nzbins = nzbins;
    h_nebins = nebins;
    h_xmin = xmin;
    h_xmax = xmax;
    h_ymin = ymin;
    h_ymax = ymax;
    h_zmin = zmin;
    h_zmax = zmax;
    h_emin = emin;
    h_emax = emax;
    h_name = name;
    h_title = title;
    h_escale = eScale;

}

template <class T>
void BDSBH4D<T>::Reset() {
    h.reset();
}

template <class T>
BDSBH4D<T>* BDSBH4D<T>::Clone(const char* newname) const {
    auto clone = new BDSBH4D<T>(*this);
    clone->SetName(newname);
    return clone;

}

template <class T>
void BDSBH4D<T>::Fill(double xValue,
                   double yValue,
                   double zValue,
                   double eValue) {
    h(xValue, yValue, zValue, eValue);
}

template <class T>
void BDSBH4D<T>::Set(int x,
                  int y,
                  int z,
                  int e,
                  double value) {
    h.at(x, y, z, e) = value;
}

template <class T>
void BDSBH4D<T>::SetError(int x,
                       int y,
                       int z,
                       int e,
                       double value) {
    h_err.at(x, y, z, e) = value;
}

template <class T>
double BDSBH4D<T>::At(int x, int y, int z, int e) {
    return h.at(x, y, z, e);
}

template <class T>
double BDSBH4D<T>::AtError(int x, int y, int z, int e) {
    return h_err.at(x, y, z, e);
}

template class BDSBH4D<boost_histogram_linear>;
template class BDSBH4D<boost_histogram_log>;
