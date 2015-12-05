#ifndef BDSOUTPUTROOTEVENTSAMPLER_H
#define BDSOUTPUTROOTEVENTSAMPLER_H

#include "TROOT.h"
#include <vector>
#ifndef __MAKECINT__ 
#include "BDSSamplerHit.hh"
#endif

class BDSOutputROOTEventSampler {
  
protected:
  std::string samplerName; //|| Don't split the header
  
  int                n;    
  std::vector<float> energy;
  std::vector<float> x;
  std::vector<float> y;
  float              z;
  std::vector<float> xp;
  std::vector<float> yp;
  std::vector<float> zp;
  std::vector<float> t;

  std::vector<float> weight;
  std::vector<int>   partID;
  std::vector<int>   parentID;
  std::vector<int>   turnNumber;
  
  float              S;   // Will not need this when have global transforms
  
public:
  BDSOutputROOTEventSampler();
  BDSOutputROOTEventSampler(std::string samplerNameIn);
  virtual ~BDSOutputROOTEventSampler();
#ifndef __MAKECINT__
  void Fill(G4double E,
	    G4double x0, G4double y0, G4double z0,
	    G4double xp, G4double yp, G4double zp,
	    G4double t,
	    G4double weight, G4int    PDGType, G4int    nEvent, G4int    TurnsTaken);
  void Fill(BDSSamplerHit *hit);
#endif
  /// Clean Sampler
  void Flush();

  ClassDef(BDSOutputROOTEventSampler,1);
};

#endif
