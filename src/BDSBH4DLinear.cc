//
// Created by Cedric Hernalsteens on 23/07/2020.
//

#include "BDSBH4DLinear.hh"

#include <string>
#include "TFile.h"

ClassImp(BDSBH4DLinear)

BDSBH4DLinear::BDSBH4DLinear()
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

BDSBH4DLinear::BDSBH4DLinear(std::string& name, std::string& title, const std::string& eScale,
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

void BDSBH4DLinear::Reset() {
    h.reset();
}

BDSBH4DLinear* BDSBH4DLinear::Clone(const char* newname) const {
    auto clone = new BDSBH4DLinear(*this);
    clone->SetName(newname);
    return clone;

}

void BDSBH4DLinear::Fill(double xValue,
                         double yValue,
                         double zValue,
                         double eValue) {
    h(xValue, yValue, zValue, eValue);
}

void BDSBH4DLinear::Set(int x,
                        int y,
                        int z,
                        int e,
                        double value) {
    h.at(x, y, z, e) = value;
}

void BDSBH4DLinear::SetError(int x,
                        int y,
                        int z,
                        int e,
                        double value) {
    h_err.at(x, y, z, e) = value;
}

double BDSBH4DLinear::At(int x, int y, int z, int e) {
    return h.at(x, y, z, e);
}

double BDSBH4DLinear::AtError(int x, int y, int z, int e) {
    return h_err.at(x, y, z, e);
}
