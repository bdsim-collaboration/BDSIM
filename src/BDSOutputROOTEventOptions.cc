#include "BDSOutputROOTEventOptions.hh"

ClassImp(BDSOutputROOTEventOptions)

BDSOutputROOTEventOptions::BDSOutputROOTEventOptions() : OptionsBase(), BDSExecOptions()
{
}

BDSOutputROOTEventOptions::BDSOutputROOTEventOptions(const GMAD::OptionsBase *ob, const BDSExecOptions *eo) : OptionsBase(*ob), BDSExecOptions(*eo)
{

}

BDSOutputROOTEventOptions::~BDSOutputROOTEventOptions()
{
}

void BDSOutputROOTEventOptions::Fill()
{
}
