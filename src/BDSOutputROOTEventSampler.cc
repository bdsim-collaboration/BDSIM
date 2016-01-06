#include "CLHEP/Units/PhysicalConstants.h"

#include "BDSOutputROOTEventSampler.hh"

ClassImp(BDSOutputROOTEventSampler)

BDSOutputROOTEventSampler::BDSOutputROOTEventSampler() {
  samplerName = "sampler";
  this->Flush();
}

BDSOutputROOTEventSampler::BDSOutputROOTEventSampler(std::string samplerNameIn) 
{
  samplerName = samplerNameIn;
  this->Flush();
}

BDSOutputROOTEventSampler::~BDSOutputROOTEventSampler() {}

void BDSOutputROOTEventSampler::Fill(G4double E,
				     G4double x0,
				     G4double y0,
				     G4double z0,
				     G4double xp,
				     G4double yp,
				     G4double zp,
				     G4double t,
				     G4double weight,
				     G4int    PDGType,
				     G4int    /*nEvent*/,
				     G4int    TurnsTaken) 
{
  this->n++;
  this->z = z0             / CLHEP::m;
  this->S = 0              / CLHEP::m;  
  this->energy.push_back(E / CLHEP::GeV);
  this->x.push_back(     x0/ CLHEP::m);
  this->y.push_back(     y0/ CLHEP::m);
  this->xp.push_back(    xp/ CLHEP::radian);
  this->yp.push_back(    yp/ CLHEP::radian);
  this->zp.push_back(    zp/ CLHEP::radian);
  this->t.push_back(      t/ CLHEP::ns);
  this->weight.push_back(weight);
  this->partID.push_back(PDGType);
  this->parentID.push_back(0);
  this->turnNumber.push_back(TurnsTaken);   
}

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
  
  this->t.push_back(hit->GetT()                 / CLHEP::ns);

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
  
void BDSOutputROOTEventSampler::Flush() 
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
