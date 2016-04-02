#include "BDSOutputROOTEventOptions.hh"

ClassImp(BDSOutputROOTEventOptions)

BDSOutputROOTEventOptions::BDSOutputROOTEventOptions() : OptionsBase(), BDSExecOptions()
{
}

BDSOutputROOTEventOptions::BDSOutputROOTEventOptions(GMAD::OptionsBase *ob, BDSExecOptions *eo) : OptionsBase(*ob), BDSExecOptions(*eo)
{

}

BDSOutputROOTEventOptions::~BDSOutputROOTEventOptions()
{
}

void BDSOutputROOTEventOptions::Fill()
{
}
