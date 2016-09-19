#ifndef BDSGEOMETTRYINSPECTOR_H
#define BDSGEOMETTRYINSPECTOR_H

#include "BDSExtent.hh"

#include "globals.hh"
#include "G4CutTubs.hh"

#include <utility>

class G4VSolid;

namespace BDS
{
  std::pair<BDSExtent, BDSExtent> DetermineExtents(const G4VSolid* solid);

  std::pair<BDSExtent, BDSExtent> InspectDisplacedSolid(const G4VSolid* solidIn);
  std::pair<BDSExtent, BDSExtent> InspectSubtractionSolid(const G4VSolid* solidIn);
  std::pair<BDSExtent, BDSExtent> InspectIntersectionSolid(const G4VSolid* solidIn);
  std::pair<BDSExtent, BDSExtent> InspectUnionSolid(const G4VSolid* solidIn);
  std::pair<BDSExtent, BDSExtent> InspectBox(const G4VSolid* solidIn);
  std::pair<BDSExtent, BDSExtent> InspectTubs(const G4VSolid* solidIn);
  std::pair<BDSExtent, BDSExtent> InspectCutTubs(const G4VSolid* solidv); 

  class BDSCutTubsTemp: public G4CutTubs
  {
  public:
    BDSCutTubsTemp(const G4CutTubs* solid):
      G4CutTubs(*solid){;}
    virtual ~BDSCutTubsTemp() {;}
    virtual void GetMaxMinZ(G4double& zmin, G4double& zmax) const {return G4CutTubs::GetMaxMinZ(zmin,zmax);};
  };
}

#endif
