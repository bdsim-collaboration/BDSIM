#ifndef BDSOUTPUTROOTEVENTSAMPLER_H
#define BDSOUTPUTROOTEVENTSAMPLER_H

#include "TROOT.h"
#include <vector>
#ifndef __ROOTBUILD__ 
#include "globals.hh"
class BDSSamplerHit;
#endif

#include "TObject.h"
#include "TTree.h"

/**
 * @brief Information stored per sampler per event.
 * 
 * @author Stewart Boogert
 */

template<class T> class BDSOutputROOTEventSampler: public TObject
{  
public:
  std::string samplerName; //|| Don't split the header
  
  int                n;
  std::vector<T>     energy;
  std::vector<T>     x;
  std::vector<T>     y;
  T                  z;
  std::vector<T>     xp;
  std::vector<T>     yp;
  std::vector<T>     zp;
  std::vector<T>     t;

  std::vector<T>     weight;
  std::vector<int>   partID;
  std::vector<int>   parentID;
  std::vector<int>   trackID;
  int                modelID;
  std::vector<int>   turnNumber;
  
  T                  S;   // Will not need this when have global transforms

  BDSOutputROOTEventSampler();
  explicit BDSOutputROOTEventSampler(std::string samplerNameIn);
  virtual ~BDSOutputROOTEventSampler();
#ifndef __ROOTBUILD__
  void Fill(G4double E,
            G4double x0, G4double y0, G4double z0,
            G4double xp, G4double yp, G4double zp,
            G4double t,
            G4double weight, G4int    PDGType, G4int    nEvent, G4int    TurnsTaken,
            G4int beamlineIndex);
  void Fill(const BDSSamplerHit* hit);
#endif
  void SetBranchAddress(TTree *);
  /// Clean Sampler
  void Flush();

  ClassDef(BDSOutputROOTEventSampler,1);
};

#endif
