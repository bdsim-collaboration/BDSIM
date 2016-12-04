#include "BDSMagnetOuterInfo.hh"

#include "BDSMagnetGeometryType.hh"
#include "globals.hh"             // geant4 globals / types
#include "G4Material.hh"

BDSMagnetOuterInfo::BDSMagnetOuterInfo():
  name("not_specified"),
  geometryType(BDSMagnetGeometryType::cylindrical),
  outerDiameter(0),
  outerMaterial(nullptr),
  angleIn(0),
  angleOut(0),
  yokeOnLeft(false),
  buildEndPieces(true),
  geometryTypeAndPath("")
{;}

BDSMagnetOuterInfo::BDSMagnetOuterInfo(G4String              nameIn,
				       BDSMagnetGeometryType geometryTypeIn,
				       G4double              outerDiameterIn,
				       G4Material*           outerMaterialIn,
				       G4double              angleInIn,
				       G4double              angleOutIn,
				       G4bool                yokeOnLeftIn,
				       G4bool                buildEndPiecesIn,
				       G4String              geometryTypeAndPathIn):
  name(nameIn),
  geometryType(geometryTypeIn),
  outerDiameter(outerDiameterIn),
  outerMaterial(outerMaterialIn),
  angleIn(angleInIn),
  angleOut(angleOutIn),
  yokeOnLeft(yokeOnLeftIn),
  buildEndPieces(buildEndPiecesIn),
  geometryTypeAndPath(geometryTypeAndPathIn)
{;}
  
std::ostream& operator<< (std::ostream& out, BDSMagnetOuterInfo const& info)
{
  out << "Magnet Outer Info: \"" << info.name << "\""             << G4endl;
  out << "Geometry Type:      "  << info.geometryType             << G4endl;
  out << "Diameter:           "  << info.outerDiameter            << G4endl;
  out << "Material:           "  << info.outerMaterial->GetName() << G4endl;
  out << "Angle In:           "  << info.angleIn                  << G4endl;
  out << "Angle Out:          "  << info.angleOut                 << G4endl;
  out << "Geometry:           "  << info.geometryTypeAndPath      << G4endl;
  return out;
}
