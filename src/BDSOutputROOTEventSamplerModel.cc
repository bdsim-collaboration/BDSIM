#include "BDSOutputROOTEventSamplerModel.hh"

#ifndef __ROOTBUILD__
#include "BDSSamplerInfo.hh"
#include "BDSSamplerRegistry.hh"

#include "globals.hh" // geant4 types / globals
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#endif

#include "TRotation.h"
#include "TVector3.h"

#include <vector>

ClassImp(BDSOutputROOTEventSamplerModel)

BDSOutputROOTEventSamplerModel::BDSOutputROOTEventSamplerModel()
{;}

BDSOutputROOTEventSamplerModel::~BDSOutputROOTEventSamplerModel()
{;}

#ifndef __ROOTBULD__
void BDSOutputROOTEventSamplerModel::Fill()
{
  const BDSSamplerRegistry* registry = BDSSamplerRegistry::Instance();

  for (const auto& info : *registry)
    {
      // Name
      samplerName.push_back(info.Name());

      // Break apart transform into tranlsation and rotation and construct root versions
      G4Transform3D transform = info.Transform();

      // Tranlsation
      G4ThreeVector t = transform.getTranslation();
      position.push_back(TVector3(t.getX(), t.getY(), t.getZ()));

      // Rotation
      G4RotationMatrix r  = transform.getRotation();
      TRotation        tr = TRotation();
      tr.SetXEulerAngles(r.getTheta(), r.getPhi(), r.getPsi());
      rotation.push_back(tr);

      // S position of sampler
      sPosition.push_back(info.SPosition());
    }
#endif
}
