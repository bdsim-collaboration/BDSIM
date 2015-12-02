#include "CLHEP/Units/PhysicalConstants.h"

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

BDSOutputROOTEventSampler::~BDSOutputROOTEventSampler() {}

void BDSOutputROOTEventSampler::Fill(BDSSamplerHit *hit) 
{
  // get single values
  this->n++;
  this->z = hit->GetZ() / CLHEP::m;
  this->S = hit->GetS() / CLHEP::m;
  
  this->energy.push_back(hit->GetTotalEnergy()  / CLHEP::GeV);
  this->x.push_back(hit->GetX()                 / CLHEP::m);
  this->y.push_back(hit->GetY()                 / CLHEP::m);
  
  this->xp.push_back(hit->GetXPrime()           / CLHEP::radian);
  this->yp.push_back(hit->GetYPrime()           / CLHEP::radian);
  this->zp.push_back(hit->GetZPrime()           / CLHEP::radian);
  
#if 0
  this->X.push_back(hit->GetGlobalX()           / CLHEP::m);
  this->Y.push_back(hit->GetGlobalY()           / CLHEP::m);
  this->Z = hit->GetGlobalZ()                   / CLHEP::m;  
  this->Xp.push_back(hit->GetGlobalXPrime()     / CLHEP::radian);
  this->Yp.push_back(hit->GetGlobalYPrime()     / CLHEP::radian);
  this->Zp.push_back(hit->GetGlobalZPrime()     / CLHEP::radian);
#endif
  
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
#if 0
  this->X.clear();
  this->Y.clear();
  this->Z = 0.0;      
  this->Xp.clear();
  this->Yp.clear();
  this->Zp.clear();
#endif
  this->weight.clear();
  this->partID.clear();
  this->parentID.clear();
  this->turnNumber.clear();
  this->S = 0.0;  
}
