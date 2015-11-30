#include "BDSOutputROOTEventSampler.hh"

ClassImp(BDSOutputROOTEventSampler)

BDSOutputROOTEventSampler::BDSOutputROOTEventSampler() {
  samplerName = "sampler";
  this->Clear();
}

BDSOutputROOTEventSampler::BDSOutputROOTEventSampler(std::string samplerNameIn) 
{
  samplerName = samplerNameIn;
  this->Clear();
}

BDSOutputROOTEventSampler::~BDSOutputROOTEventSampler() {};

void BDSOutputROOTEventSampler::Fill(BDSSamplerHit *hit) 
{
  // get single values
  this->n++;
  this->z = hit->GetZ();
  this->Z = hit->GetGlobalZ();
  this->S = hit->GetS();
  
  this->energy.push_back(hit->GetTotalEnergy());
  this->x.push_back(hit->GetX());
  this->y.push_back(hit->GetY());
  
  this->xp.push_back(hit->GetXPrime());
  this->yp.push_back(hit->GetYPrime());
  this->zp.push_back(hit->GetZPrime());
  
  this->X.push_back(hit->GetGlobalX());
  this->Y.push_back(hit->GetGlobalY());
  
  this->Xp.push_back(hit->GetGlobalXPrime());
  this->Yp.push_back(hit->GetGlobalYPrime());
  this->Zp.push_back(hit->GetGlobalZPrime());
  
  this->weight.push_back(hit->GetWeight());
  this->partID.push_back(hit->GetPDGtype());
  this->parentID.push_back(hit->GetParentID());
  this->turnNumber.push_back(hit->GetTurnsTaken());

} 
  
void BDSOutputROOTEventSampler::Clear() 
{
  this->n = 0;
  this->energy.clear();
  this->x.clear();
  this->y.clear();
  this->z = 0.0;
  this->xp.clear();
  this->yp.clear();
  this->zp.clear();
  this->X.clear();
  this->Y.clear();
  this->Z = 0.0;      
  this->Xp.clear();
  this->Yp.clear();
  this->Zp.clear();
  this->weight.clear();
  this->partID.clear();
  this->parentID.clear();
  this->turnNumber.clear();
  this->S = 0.0;  
}
