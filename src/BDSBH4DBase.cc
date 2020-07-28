//
// Created by Eliott Ramoisiaux on 05/06/2020.
//

#include "BDSBH4DBase.hh"

#include <string>
#include "TFile.h"

ClassImp(BDSBH4DBase)

int BDSBH4DBase::GetNbinsX() const {
    return static_cast<int>(h_nxbins);
}

int BDSBH4DBase::GetNbinsY() const {
    return static_cast<int>(h_nybins);
}

int BDSBH4DBase::GetNbinsZ() const {
    return static_cast<int>(h_nzbins);
}

int BDSBH4DBase::GetNbinsE() const {
    return static_cast<int>(h_nebins);
}

const char* BDSBH4DBase::GetName() const {
    return h_name.c_str();
}

const char* BDSBH4DBase::GetTitle() const {
    return h_title.c_str();
}

void BDSBH4DBase::SetName(const char* name) {
    h_name = std::string(name);
}

void BDSBH4DBase::SetTitle(const char* title) {
    h_title = std::string(title);
}

void BDSBH4DBase::SetEntries(double i){
    entries = static_cast<unsigned int>(i);
}
