#include "BDSOutputROOTEventBeam.hh"

ClassImp(BDSOutputROOTEventBeam)

BDSOutputROOTEventBeam::BDSOutputROOTEventBeam() : BeamBase()
{;}

BDSOutputROOTEventBeam::BDSOutputROOTEventBeam(const GMAD::BeamBase* ob): BeamBase(*ob)
{;}

BDSOutputROOTEventBeam::~BDSOutputROOTEventBeam()
{;}

#ifndef __ROOTBUILD__
void BDSOutputROOTEventBeam::Fill()
{;}
#endif
