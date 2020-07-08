//
// Created by Eliott Ramoisiaux on 05/06/2020.
//

#include "BDSBH4D.hh"

ClassImp(BDSBH4D)

BDSBH4D::BDSBH4D()
{
    h = boost::histogram::make_histogram_with(std::vector<double>(),boost::histogram::axis::regular<double> {3, 0.0, 1.0, "x"},
                                              boost::histogram::axis::regular<double> {3, 0.0, 1.0, "y"},
                                              boost::histogram::axis::regular<double> {3, 0.0, 1.0, "z"},
                                              boost::histogram::axis::regular<double, boost::histogram::axis::transform::log> {3, 1.0, 230.0, "Energy_log"});
}

