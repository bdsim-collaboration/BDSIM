//
// Created by Eliott Ramoisiaux on 05/06/2020.
//

#include "BDSBH4D.hh"
#include "globals.hh"

#include <string>
#include "TFile.h"

ClassImp(BDSBH4D)

BDSBH4D::BDSBH4D()
{
    h = boost::histogram::make_histogram_with(std::vector<double>(),boost::histogram::axis::regular<double> {3, 0.0, 1.0, "x"},
                                              boost::histogram::axis::regular<double> {3, 0.0, 1.0, "y"},
                                              boost::histogram::axis::regular<double> {3, 0.0, 1.0, "z"},
                                              boost::histogram::axis::regular<double, boost::histogram::axis::transform::log> {3, 1.0, 230.0, "Energy_log"});

    h_err = boost::histogram::make_histogram_with(std::vector<double>(),boost::histogram::axis::regular<double> {3, 0.0, 1.0, "x"},
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

BDSBH4D::BDSBH4D(const std::string name, const std::string title,
                 unsigned int nxbins, G4double xmin, G4double xmax,
                 unsigned int nybins, G4double ymin, G4double ymax,
                 unsigned int nzbins, G4double zmin, G4double zmax,
                 unsigned int nebins, G4double emin, G4double emax)
{
    h = boost::histogram::make_histogram_with(std::vector<double>(),boost::histogram::axis::regular<double> {nxbins, xmin, xmax, "x"},
                                              boost::histogram::axis::regular<double> {nybins, ymin, ymax, "y"},
                                              boost::histogram::axis::regular<double> {nzbins, zmin, zmax, "z"},
                                              boost::histogram::axis::regular<double, boost::histogram::axis::transform::log> {nebins, emin, emax, "Energy_log"});

    h_err = boost::histogram::make_histogram_with(std::vector<double>(),boost::histogram::axis::regular<double> {nxbins, xmin, xmax, "x"},
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
    h_name = name;
    h_title = title;

}

void BDSBH4D::to_PyROOT(std::string filename, std::string histo_name) {

    const char* filename_char = filename.c_str();

    std::string path = "Event/MergedHistograms/" + histo_name;
    const char* path_char = path.c_str();

    TFile *_file0 = TFile::Open(filename_char);
    TTree* tree = (TTree*) _file0->Get(path_char);
    BDSBH4D* Boost_histogram = new BDSBH4D();
    tree->SetBranchAddress("BDSBH4D",&Boost_histogram);
    tree->GetEntry(0);

    this->h = Boost_histogram->h;
    this->h_err = Boost_histogram->h_err;
    this->h_nxbins = Boost_histogram->h_nxbins;
    this->h_nybins = Boost_histogram->h_nybins;
    this->h_nzbins = Boost_histogram->h_nzbins;
    this->h_nebins = Boost_histogram->h_nebins;
    this->h_xmin = Boost_histogram->h_xmin;
    this->h_xmax = Boost_histogram->h_xmax;
    this->h_ymin = Boost_histogram->h_ymin;
    this->h_ymax = Boost_histogram->h_ymax;
    this->h_zmin = Boost_histogram->h_zmin;
    this->h_zmax = Boost_histogram->h_zmax;
    this->h_emin = Boost_histogram->h_emin;
    this->h_emax = Boost_histogram->h_emax;
    this->h_name = Boost_histogram->h_name;
    this->h_title = Boost_histogram->h_title;


}

int BDSBH4D::GetNbinsX() {
    return h_nxbins;
}

int BDSBH4D::GetNbinsY() {
    return h_nybins;
}

int BDSBH4D::GetNbinsZ() {
    return h_nzbins;
}

int BDSBH4D::GetNbinsE() {
    return h_nebins;
}

const std::string BDSBH4D::GetName() {
    return h_name;
}

const std::string BDSBH4D::GetTitle() {
    return h_title;
}

void BDSBH4D::SetName(const std::string name) {
    h_name = name;
}

void BDSBH4D::SetTitle(const std::string title) {
    h_title = title;
}

void BDSBH4D::SetEntries(unsigned long i){
    entries = i;
}

void BDSBH4D::Reset() {
    auto v = HistogramResetVisitor();
    boost::apply_visitor(v,h);
    //h.reset();
}

BDSBH4D* BDSBH4D::Clone(std::string newname) {

    BDSBH4D* clone = new BDSBH4D(*this);
    clone->SetName(newname);
    return clone;

}
