//
// Created by Eliott Ramoisiaux on 05/06/2020.
//

#include "BDSBH4D.hh"
#include "globals.hh"

ClassImp(BDSBH4D)

BDSBH4D::BDSBH4D()
{
    h = boost::histogram::make_histogram_with(std::vector<double>(),boost::histogram::axis::regular<double> {3, 0.0, 1.0, "x"},
                                              boost::histogram::axis::regular<double> {3, 0.0, 1.0, "y"},
                                              boost::histogram::axis::regular<double> {3, 0.0, 1.0, "z"},
                                              boost::histogram::axis::regular<double, boost::histogram::axis::transform::log> {3, 1.0, 230.0, "Energy_log"});

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

BDSBH4D::BDSBH4D(unsigned int nxbins, G4double xmin, G4double xmax,
                 unsigned int nybins, G4double ymin, G4double ymax,
                 unsigned int nzbins, G4double zmin, G4double zmax,
                 unsigned int nebins, G4double emin, G4double emax)
{
    h = boost::histogram::make_histogram_with(std::vector<double>(),boost::histogram::axis::regular<double> {nxbins, xmin, xmax, "x"},
                                               boost::histogram::axis::regular<double> {nybins, ymin, ymax, "y"},
                                               boost::histogram::axis::regular<double> {nzbins, zmin, zmax, "z"},
                                               boost::histogram::axis::regular<double, boost::histogram::axis::transform::log> {nebins, emin, emax, "Energy_log"});

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

}