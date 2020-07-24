//
// Created by Eliott Ramoisiaux on 05/06/2020.
//

#include "BDSBH4D.hh"

#include <string>
#include "TFile.h"

ClassImp(BDSBH4D)

int BDSBH4D::GetNbinsX() const {
    return static_cast<int>(h_nxbins);
}

int BDSBH4D::GetNbinsY() const {
    return static_cast<int>(h_nybins);
}

int BDSBH4D::GetNbinsZ() const {
    return static_cast<int>(h_nzbins);
}

int BDSBH4D::GetNbinsE() const {
    return static_cast<int>(h_nebins);
}

const char* BDSBH4D::GetName() const {
    return h_name.c_str();
}

const char* BDSBH4D::GetTitle() const {
    return h_title.c_str();
}

void BDSBH4D::SetName(const char* name) {
    h_name = std::string(name);
}

void BDSBH4D::SetTitle(const char* title) {
    h_title = std::string(title);
}

void BDSBH4D::SetEntries(double i){
    entries = static_cast<unsigned int>(i);
}
