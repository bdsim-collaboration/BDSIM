#ifndef BDSOUTPUTROOTEVENTSAMPLER_H
#define BDSOUTPUTROOTEVENTSAMPLER_H

#include "TROOT.h"
#include <vector>
#ifndef __MAKECINT__ 
#include "BDSSamplerHit.hh"
#endif

class BDSOutputROOTEventSampler : public TObject { 
  
protected:
  std::string samplerName;
  
  int                n;    
  std::vector<float> energy;
  std::vector<float> x;
  std::vector<float> y;
  float              z;
  std::vector<float> xp;
  std::vector<float> yp;
  std::vector<float> zp;
  
  std::vector<float> X;
  std::vector<float> Y;
  float              Z;
  std::vector<float> Xp;
  std::vector<float> Yp;
  std::vector<float> Zp;
  
  std::vector<float> weight;
  std::vector<int>   partID;
  std::vector<int>   parentID;
  std::vector<int>   turnNumber;
  
  float              S;   
  
public:
  BDSOutputROOTEventSampler();
  BDSOutputROOTEventSampler(std::string samplerNameIn);
  ~BDSOutputROOTEventSampler();  
#ifndef __MAKECINT__
  void Fill(BDSSamplerHit *hit);
#endif
  void Clear();

  ClassDef(BDSOutputROOTEventSampler,1);
};

#endif
