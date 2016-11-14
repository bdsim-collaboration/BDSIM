#include "BDSExtent.hh"
#include "BDSGeometryInspector.hh"

#include "globals.hh"
#include "G4AffineTransform.hh"
#include "G4Box.hh"
#include "G4CutTubs.hh"
#include "G4DisplacedSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4VSolid.hh"

#include <utility>

// length safety large - done as define as these are all unbound methods
#define LSL 1*CLHEP::um

std::pair<BDSExtent, BDSExtent> BDS::DetermineExtents(const G4VSolid* solid)
{
  G4String className = solid->GetEntityType();
  if (className == "G4DisplacedSolid")
    {return BDS::InspectDisplacedSolid(solid);}
  else if (className == "G4SubtractionSolid")
    {return BDS::InspectSubtractionSolid(solid);}
  else if (className == "G4IntersectionSolid")
    {return BDS::InspectIntersectionSolid(solid);}
  else if (className == "G4UnionSolid")
    {return BDS::InspectUnionSolid(solid);}
  else if (className == "G4Box")
    {return BDS::InspectBox(solid);}
  else if (className == "G4Tubs")
    {return BDS::InspectTubs(solid);}
  else if (className == "G4CutTubs")
    {return BDS::InspectCutTubs(solid);}
  else
    {
      G4cout << "Solid of type \"" << className << "\" is not supported" << G4endl;
      return std::make_pair(BDSExtent(), BDSExtent());
    }
}

std::pair<BDSExtent, BDSExtent> BDS::InspectDisplacedSolid(const G4VSolid* solidIn)
{
  const G4DisplacedSolid* solid = dynamic_cast<const G4DisplacedSolid*>(solidIn);
  if (!solid)
    {return std::make_pair(BDSExtent(), BDSExtent());}

  const G4VSolid* constituentSolid = solid->GetConstituentMovedSolid();

  auto ext = BDS::DetermineExtents(constituentSolid);
  G4AffineTransform transform = solid->GetTransform();

  // only going to shift x,y without any rotation.
  G4ThreeVector translation = transform.NetTranslation();
  BDSExtent outer = ext.first.Shift(translation.x(), translation.y());
  BDSExtent inner = ext.second.Shift(translation.x(), translation.y());
  
  return std::make_pair(outer, inner);
}
  
std::pair<BDSExtent, BDSExtent> BDS::InspectSubtractionSolid(const G4VSolid* solidIn)
{
  const G4SubtractionSolid* solid = dynamic_cast<const G4SubtractionSolid*>(solidIn);
  if (!solid)
    {return std::make_pair(BDSExtent(), BDSExtent());}

  const G4VSolid* a = solid->GetConstituentSolid(0);
  const G4VSolid* b = solid->GetConstituentSolid(1);

  auto extA = BDS::DetermineExtents(a);
  auto extB = BDS::DetermineExtents(b);

  BDSExtent outer = extA.first.TransverselyLessThan(extB.first) ? extB.first : extA.first;

  // Here we make an ASSUMPTION that the b solid is the interior one - convention.
  // Also it's the outer extent of that solid.
  BDSExtent inner = extB.first;
  
  return std::make_pair(outer, inner);
}

std::pair<BDSExtent, BDSExtent> BDS::InspectIntersectionSolid(const G4VSolid* solidIn)
{
  const G4IntersectionSolid* solid = dynamic_cast<const G4IntersectionSolid*>(solidIn);
  if (!solid)
    {return std::make_pair(BDSExtent(), BDSExtent());}

  const G4VSolid* a = solid->GetConstituentSolid(0);
  const G4VSolid* b = solid->GetConstituentSolid(1);
  
  auto extA = BDS::DetermineExtents(a);
  auto extB = BDS::DetermineExtents(b);

  BDSExtent outer = extA.first.TransverselyLessThan(extB.first) ? extB.first : extA.first;
  BDSExtent inner = extA.first.TransverselyLessThan(extB.first) ? extA.second : extB.second;
  
  return std::make_pair(outer, inner);
}

std::pair<BDSExtent, BDSExtent> BDS::InspectUnionSolid(const G4VSolid* solidIn)
{
  const G4UnionSolid* solid = dynamic_cast<const G4UnionSolid*>(solidIn);
  if (!solid)
    {return std::make_pair(BDSExtent(), BDSExtent());}

  const G4VSolid* a = solid->GetConstituentSolid(0);
  const G4VSolid* b = solid->GetConstituentSolid(1);
  
  auto extA = BDS::DetermineExtents(a);
  auto extB = BDS::DetermineExtents(b);

  BDSExtent outer = extA.first.TransverselyLessThan(extB.first) ? extB.first : extA.first;
  BDSExtent inner = extA.first.TransverselyLessThan(extB.first) ? extA.second : extB.second;
  
  return std::make_pair(outer, inner);
}

std::pair<BDSExtent, BDSExtent> BDS::InspectBox(const G4VSolid* solidIn)
{
  const G4Box* solid = dynamic_cast<const G4Box*>(solidIn);
  if (!solid)
    {return std::make_pair(BDSExtent(), BDSExtent());}

  G4double dx = solid->GetXHalfLength();
  G4double dy = solid->GetYHalfLength();
  G4double dz = solid->GetZHalfLength();

  // accurate along z, but margin in x,y
  BDSExtent outer(dx + LSL, dy + LSL ,dz); // symmetric +/-
  BDSExtent inner = BDSExtent();
  return std::make_pair(outer, inner);
}

std::pair<BDSExtent, BDSExtent> BDS::InspectTubs(const G4VSolid* solidIn)
{
  const G4Tubs* solid = dynamic_cast<const G4Tubs*>(solidIn);
  if (!solid)
    {return std::make_pair(BDSExtent(), BDSExtent());}

  G4double innerR = solid->GetInnerRadius();
  G4double outerR = solid->GetOuterRadius();
  G4double zHalfL = solid->GetZHalfLength();

  BDSExtent outer(outerR + LSL, outerR + LSL, zHalfL);
  BDSExtent inner(innerR, innerR, zHalfL);
  return std::make_pair(outer, inner);
}

std::pair<BDSExtent, BDSExtent> BDS::InspectCutTubs(const G4VSolid* solidIn)
{
  const G4CutTubs* solid = dynamic_cast<const G4CutTubs*>(solidIn);
  if (!solid)
    {return std::make_pair(BDSExtent(), BDSExtent());}

  BDS::BDSCutTubsTemp* temp = new BDS::BDSCutTubsTemp(solid);

  // G4CutTubs has a function to calculate the extent along z but it's protected
  // so wrap a dummy class around this to expose this useful calculation.
  G4double zmin;
  G4double zmax;
  temp->GetMaxMinZ(zmin,zmax); // solid does calculation and updates variables
  
  G4double innerR = solid->GetInnerRadius();
  G4double outerR = solid->GetOuterRadius();
  outerR += LSL;
  
  BDSExtent outer(-outerR, outerR,
		  -outerR, outerR,
		  zmin,    zmax);
  BDSExtent inner(-innerR, innerR,
		  -innerR, innerR,
		  zmin,    zmax);
  return std::make_pair(outer, inner);
}
