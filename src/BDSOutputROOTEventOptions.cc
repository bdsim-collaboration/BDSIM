#include "BDSOutputROOTEventOptions.hh"

ClassImp(BDSOutputROOTEventOptions)

BDSOutputROOTEventOptions::BDSOutputROOTEventOptions() : OptionsBase()
{;}

BDSOutputROOTEventOptions::BDSOutputROOTEventOptions(const GMAD::OptionsBase* ob): OptionsBase(*ob)
{;}

BDSOutputROOTEventOptions::~BDSOutputROOTEventOptions()
{
}

#ifndef __ROOTBUILD__
void BDSOutputROOTEventOptions::Fill()
{
}
#endif
