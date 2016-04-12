#include "SamplerAnalysis.hh"

SamplerAnalysis::SamplerAnalysis()
{
  this->CommonCtor();
}

SamplerAnalysis::SamplerAnalysis(BDSOutputROOTEventSampler<float> *samplerIn)
{
  this->s = samplerIn;
  this->CommonCtor();
}

void SamplerAnalysis::CommonCtor()
{
  s_s       = 0;
  npart     = 0; //weight already used in data
  E_s       = 0;
  sh_1_0    = 0;
  sh_0_1    = 0;
  sh_1_1    = 0;
  sh_2_0    = 0;
  sh_0_2    = 0;
  sh_2_1    = 0;
  sh_1_2    = 0;
  sh_3_0    = 0;
  sh_0_3    = 0;
  sh_2_2    = 0;
  sh_3_1    = 0;
  sh_1_3    = 0;
  sh_4_0    = 0;
  sh_0_4    = 0;

  sv_1_0    = 0;
  sv_0_1    = 0;
  sv_1_1    = 0;
  sv_2_0    = 0;
  sv_0_2    = 0;
  sv_2_1    = 0;
  sv_1_2    = 0;
  sv_3_0    = 0;
  sv_0_3    = 0;
  sv_2_2    = 0;
  sv_3_1    = 0;
  sv_1_3    = 0;
  sv_4_0    = 0;
  sv_0_4    = 0;

  //energy-coordinate power sums for dispersion calcs.
  se_1      = 0;
  se_2      = 0;
  se_3      = 0;
  se_4      = 0;

  sxe_1_1   = 0;
  sxe_2_1   = 0;
  sxe_1_2   = 0;
  sxe_2_2   = 0;
  sxe_3_1   = 0;
  sxe_1_3   = 0;

  sye_1_1   = 0;
  sye_2_1   = 0;
  sye_1_2   = 0;
  sye_2_2   = 0;
  sye_3_1   = 0;
  sye_1_3   = 0;

  sxpe_1_1  = 0;
  sxpe_2_1  = 0;
  sxpe_1_2  = 0;
  sxpe_2_2  = 0;
  sxpe_3_1  = 0;
  sxpe_1_3  = 0;

  sype_1_1  = 0;
  sype_2_1  = 0;
  sype_1_2  = 0;
  sype_2_2  = 0;
  sype_3_1  = 0;
  sype_1_3  = 0;
}


SamplerAnalysis::~SamplerAnalysis()
{ }

void SamplerAnalysis::Initialise()
{ }

void SamplerAnalysis::Process()
{ }

void SamplerAnalysis::Terminate()
{ }
