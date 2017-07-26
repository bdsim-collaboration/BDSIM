#ifndef BDSOUTPUTROOTEVENTMODEL_H
#define BDSOUTPUTROOTEVENTMODEL_H

#include <vector>
#include <string>

#include "TROOT.h"
#include "TObject.h"
#include "TVector3.h"
#include "TRotation.h"

/**
 * @brief Information stored per model representing accelerator.
 * 
 * @author Stewart Boogert
 */

class BDSOutputROOTEventModel: public TObject
{
public:
  std::vector<std::string> samplerNamesUnique;

  std::vector<std::string> componentName;
  std::vector<std::string> placementName;
  std::vector<std::string> componentType;
  std::vector<float>       length;
  std::vector<TVector3>    staPos;
  std::vector<TVector3>    midPos;
  std::vector<TVector3>    endPos;
  std::vector<TRotation>   staRot;
  std::vector<TRotation>   midRot;
  std::vector<TRotation>   endRot;
  std::vector<TVector3>    staRefPos;
  std::vector<TVector3>    midRefPos;
  std::vector<TVector3>    endRefPos;
  std::vector<TRotation>   staRefRot;
  std::vector<TRotation>   midRefRot;
  std::vector<TRotation>   endRefRot;
  std::vector<float>       staS;
  std::vector<float>       midS;
  std::vector<float>       endS;
  std::vector<int>         beamPipeType;
  std::vector<double>      beamPipeAper1;
  std::vector<double>      beamPipeAper2;
  std::vector<double>      beamPipeAper3;
  std::vector<double>      beamPipeAper4;


  /// Default constructor
  BDSOutputROOTEventModel();
  /// Destructor
  virtual ~BDSOutputROOTEventModel();
  /// Find element index closest to vPoint
  int findNearestElement(TVector3 vPoint);

  void Flush();
  
#ifndef __ROOTBUILD__
  /// Fill root output
  virtual void Fill();
#endif

  ClassDef(BDSOutputROOTEventModel,1);
};

#endif
