/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "BDSAperture.hh"
#include "BDSApertureCircle.hh"
#include "BDSApertureClicPCL.hh"
#include "BDSApertureEllipse.hh"
#include "BDSApertureFactory.hh"
#include "BDSApertureOctagon.hh"
#include "BDSAperturePoints.hh"
#include "BDSApertureRaceTrack.hh"
#include "BDSApertureRectangle.hh"
#include "BDSApertureRectCircle.hh"
#include "BDSApertureRectEllipse.hh"
#include "BDSApertureType.hh"
#include "BDSBeamPipeToApertureType.hh"
#include "BDSBeamPipeType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSPolygon.hh"
#include "BDSTube.hh"
#include "BDSUtilities.hh"

#include "parser/aperture.h"
#include "parser/element.h"
#include "parser/samplerplacement.h"

#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4CutTubs.hh"
#include "G4EllipticalTube.hh"
#include "G4ExtrudedSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4TwoVector.hh"
#include "G4Types.hh"
#include "G4VSolid.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <algorithm>
#include <map>
#include <utility>


BDSApertureFactory::BDSApertureFactory():
  intersectionRadiusRatio(1.3),
  productName(""),
  productLength(0),
  productApertureIn(nullptr),
  productApertureOut(nullptr),
  productLengthExtra(0),
  angledFaces(false)
{
  specialisations = {
		     {MakePair(BDSApertureType::circle, BDSApertureType::circle),
		      &BDSApertureFactory::CreateDifferentEndsCircleToCircle}
  };
  
  hollowSpecialisations = {
    {MakePair(BDSApertureType::circle, BDSApertureType::circle), &BDSApertureFactory::HollowCircleToCircle}
  };

  // TBC other specialisations possible given combination of available shapes
}

BDSApertureFactory::~BDSApertureFactory()
{;}

BDSAperture* BDSApertureFactory::CreateAperture(const GMAD::Element& el) const
{
  BDSBeamPipeType bpt = BDS::DetermineBeamPipeType(el.apertureType);
  BDSApertureType apt = BDS::ApertureTypeFromBeamPipeType(bpt);
  return CreateAperture(apt,
			el.aper1 * CLHEP::m,
			el.aper2 * CLHEP::m,
			el.aper3 * CLHEP::m,
			el.aper4 * CLHEP::m,
			0, 0, 0, 0,
			el.apertureType);
}

BDSAperture* BDSApertureFactory::CreateAperture(const GMAD::Aperture& ap) const
{
  CheckNPoints(ap.nPoints, "aperture", ap.name);
  return CreateAperture(BDS::DetermineApertureType(ap.apertureType),
			ap.aper1 * CLHEP::m,
			ap.aper2 * CLHEP::m,
			ap.aper3 * CLHEP::m,
			ap.aper4 * CLHEP::m,
			ap.tilt  * CLHEP::rad,
			ap.offsetX * CLHEP::m,
			ap.offsetY * CLHEP::m,
			(unsigned int)ap.nPoints,
			ap.apertureType);
}

BDSAperture* BDSApertureFactory::CreateAperture(const GMAD::SamplerPlacement& sp) const
{
  CheckNPoints(sp.nPoints, "samplerplacement", sp.name);
  return CreateAperture(BDS::DetermineApertureType(sp.shape),
			sp.aper1 * CLHEP::m,
			sp.aper2 * CLHEP::m,
			sp.aper3 * CLHEP::m,
			sp.aper4 * CLHEP::m,
			sp.tilt  * CLHEP::rad,
			0,
			0,
			(unsigned int)sp.nPoints,
			sp.shape);
}

BDSAperture* BDSApertureFactory::CreateAperture(BDSApertureType at,
						G4double        a1,
						G4double        a2,
						G4double        a3,
						G4double        a4,
						G4double        tilt,
						G4double        offsetX,
						G4double        offsetY,
						unsigned int    nPoints,
						const G4String& pointsFileString) const
{
  BDSAperture* result = nullptr;
  switch (at.underlying())
    {
    case BDSApertureType::circle:
      {result = new BDSApertureCircle(a1, nPoints);                  break;}
    case BDSApertureType::ellipse:
      {result = new BDSApertureEllipse(a1, a2, nPoints);             break;}
    case BDSApertureType::rectangle:
      {result = new BDSApertureRectangle(a1, a2);                    break;}
    case BDSApertureType::rectcircle:
      {result = new BDSApertureRectCircle(a1, a2, a3, nPoints);      break;}
    case BDSApertureType::rectellipse:
      {result = new BDSApertureRectEllipse(a1, a2, a3, a4, nPoints); break;}
    case BDSApertureType::racetrack:
      {result = new BDSApertureRaceTrack(a1, a2, a3, nPoints);       break;}
    case BDSApertureType::octagon:
      {result = new BDSApertureOctagon(a1, a2, a3, a4);              break;}
    case BDSApertureType::clicpcl:
      {result = new BDSApertureClicPCL(a1, a2, a3, a4, nPoints);     break;}
    case BDSApertureType::points:
      {
	G4String pointsFile;
	G4String unitsStr;
	ParsePointsFileAndUnits(pointsFileString, pointsFile, unitsStr);
	result = new BDSAperturePoints(pointsFile, unitsStr);
	break;
      }
    default:
      {break;}
    }
  if (result)
    {
      result->CheckInfoOK();
      result->SetTiltOffset(BDSTiltOffset(tilt, offsetX, offsetY));
    }
  return result;
}

void BDSApertureFactory::CheckNPoints(int nPoints,
				      const G4String& typeName,
				      const G4String& objectName) const
{
  if (nPoints < 0)
    {throw BDSException(__METHOD_NAME__, "negative \"nPoints\" in " + typeName +" definition \"" + objectName + "\"");}
}

void BDSApertureFactory::ParsePointsFileAndUnits(const G4String& beamPipeType,
						G4String& pointsFileName,
						G4String& pointsUnit) const
{
  auto typeAndFileName = BDS::SplitOnColon(beamPipeType); // find first colon
  G4String fname = typeAndFileName.second;
  if (BDS::StrContains(fname, ":"))
    {// optional second colon with units after it
      auto fileNameAndUnit = BDS::SplitOnColon(fname);
      pointsFileName = fileNameAndUnit.first;
      pointsUnit = fileNameAndUnit.second;
    }
  else
    {
      pointsFileName = fname;
      pointsUnit = "mm";
    }
}

G4VSolid* BDSApertureFactory::CreateSolid(const G4String&    name,
                                          G4double           length,
                                          const BDSAperture* apertureIn,
                                          const BDSAperture* apertureOut,
                                          const G4ThreeVector* normalIn,
                                          const G4ThreeVector* normalOut,
                                          G4double           lengthExtraForBoolean)
{
  productNormalIn  = normalIn  ? *normalIn : G4ThreeVector();
  productNormalOut = normalOut ? *normalOut : G4ThreeVector();
  angledFaces      = normalIn || normalOut;
  
  if (!apertureIn)
    {throw BDSException(__METHOD_NAME__, "no aperture specified.");}
  G4bool variedAperture = (G4bool)apertureOut; // ie valid pointer for shape out.

  productName        = name;
  productLength      = length;
  productApertureIn  = apertureIn;
  productApertureOut = apertureOut;
  productLengthExtra = lengthExtraForBoolean;

  if (variedAperture)
    {return CreateDifferentEnds();}

  // else create non-tapering piece of geometry
  G4VSolid* product = nullptr;
  switch (apertureIn->apertureType.underlying())
    {
    case BDSApertureType::circle:
      {product = CreateCircle();      break;}
    case BDSApertureType::rectangle:
      {product = CreateRectangle();   break;}
    case BDSApertureType::ellipse:
      {product = CreateEllipse();     break;}
    case BDSApertureType::rectcircle:
      {product = CreateRectCircle();  break;}
    case BDSApertureType::rectellipse:
      {product = CreateRectEllipse(); break;}
    case BDSApertureType::racetrack:
      {product = CreateRaceTrack();   break;}
    case BDSApertureType::octagon:
      {product = CreateOctagonal();   break;}
    case BDSApertureType::clicpcl:
      {product = CreateClicPCL();     break;}
    case BDSApertureType::points:
      {product = CreateExtrudedSolid(); break;}
    default:
      {break;}
    }
  return product;
}

G4VSolid* BDSApertureFactory::CreateSolidWithInner(const G4String&      name,
                                                   G4double             length,
                                                   const BDSAperture*   apertureInInside,
                                                   const BDSAperture*   apertureOutInside,
                                                   G4double             thickness,
                                                   const G4ThreeVector* normalIn,
                                                   const G4ThreeVector* normalOut,
                                                   G4double             lengthExtraForBoolean)
{
  productNormalIn  = normalIn  ? *normalIn : G4ThreeVector();
  productNormalOut = normalOut ? *normalOut : G4ThreeVector();
  angledFaces      = normalIn || normalOut;
  
  if (!apertureInInside)
    {throw BDSException(__METHOD_NAME__, "no aperture specified.");}
  G4bool variedAperture = (G4bool)apertureOutInside && apertureOutInside != apertureInInside;
  
  productName        = name;
  productLength      = length;
  productApertureIn  = apertureInInside;
  productApertureOut = variedAperture ? apertureOutInside : apertureInInside;
  productLengthExtra = lengthExtraForBoolean;
  
  // check specialisations
  auto key    = MakePair(productApertureIn->apertureType, productApertureOut->apertureType);
  auto search = hollowSpecialisations.find(key);
  if (search != hollowSpecialisations.end())
    {
      auto mem = search->second;
      return (this->*mem)();
    }
  else // no specialisation, so use high number polygons
    {return CreateTubeByPoints();}
}

G4VSolid* BDSApertureFactory::CreateSolidWithInnerVariableThickness(const G4String&      name,
                                                                    G4double             length,
                                                                    const BDSAperture*   apertureInOutside,
                                                                    const BDSAperture*   apertureInInside,
                                                                    const BDSAperture*   apertureOutOutside,
                                                                    const BDSAperture*   apertureOutInside,
                                                                    const G4ThreeVector* normalIn,
                                                                    const G4ThreeVector* normalOut,
                                                                    G4double             lengthExtraForBoolean)
{
  productNormalIn  = normalIn  ? *normalIn : G4ThreeVector();
  productNormalOut = normalOut ? *normalOut : G4ThreeVector();
  angledFaces      = normalIn || normalOut;
  
  // If the apertures are twisted, it would be possible to create a shape where both the inner apertures
  // were smaller than the outer apertures, but part way through they're bigger due to the rotation. This
  // should be handled.
  
  return nullptr;
}

G4VSolid* BDSApertureFactory::CreateCircle() const
{
  const BDSApertureCircle* ap = dynamic_cast<const BDSApertureCircle*>(productApertureIn);
  if (!ap)
    {return nullptr;}
  if (!angledFaces)
    {
      G4VSolid* product = new G4Tubs(productName,
				     0,
				     ap->radius,
				     0.5 * productLength + productLengthExtra,
				     0,
				     CLHEP::twopi);
      return product;
    }
  else
    {
      G4VSolid* product = new G4CutTubs(productName,
					0,
					ap->radius,
					0.5 * productLength + productLengthExtra,
					0,
					CLHEP::twopi,
					productNormalIn,
					productNormalOut);
      return product;
    }
}

G4VSolid* BDSApertureFactory::CreateRectangle() const
{
  const BDSApertureRectangle* ap = dynamic_cast<const BDSApertureRectangle*>(productApertureIn);
  if (!ap)
    {return nullptr;}
  if (!angledFaces)
    {
      G4VSolid* product = new G4Box(productName,
				                    ap->a,
				                    ap->b,
				                    0.5 * productLength + productLengthExtra);
      return product;
    }
  else
    {
      G4VSolid* box = new G4Box(productName + "_square",
				ap->a,
				ap->b,
				productLength + productLengthExtra); // factor 2 here
      G4double maxRadius = ap->RadiusToEncompass();
      G4VSolid* cut = CutSolid(productName + "_angled", maxRadius);
      G4VSolid* product = new G4IntersectionSolid(productName, box, cut);
      return product;
    }
}

G4VSolid* BDSApertureFactory::CutSolid(const G4String& name,
                                       G4double radiusToEncompass) const
{
  G4double intersectionRadius = intersectionRadiusRatio * radiusToEncompass;
  G4VSolid* cut = new G4CutTubs(name,
                                0,
                                intersectionRadius,
                                0.5 * productLength + productLengthExtra,
                                0,
                                CLHEP::twopi,
                                productNormalIn,
                                productNormalOut);
  return cut;
}

G4VSolid* BDSApertureFactory::CreateEllipse() const
{
  const BDSApertureEllipse* ap = dynamic_cast<const BDSApertureEllipse*>(productApertureIn);
  if (!ap)
    {return nullptr;}
  if (!angledFaces)
    {
      G4VSolid* product = new G4EllipticalTube(productName,
                                               ap->a,
                                               ap->b,
                                               0.5 * productLength + productLengthExtra);
      return product;
    }
  else
    {
      G4double maxRadius = ap->RadiusToEncompass();
      G4VSolid* cut = CutSolid(productName + "_angled", maxRadius);
      G4VSolid* tube = new G4EllipticalTube(productName + "_square",
                                            ap->a,
                                            ap->b,
                                            productLength + productLengthExtra);
      G4VSolid* product = new G4IntersectionSolid(productName, tube, cut);
      return product;
    }
}

G4VSolid* BDSApertureFactory::CreateExtrudedSolid() const
{
  BDSPolygon p = productApertureIn->Polygon();
  G4TwoVector zOffsets(0,0); // the transverse offset of each plane from 0,0
  G4double zScale = 1; // the scale at each end of the points = 1
  if (!angledFaces)
    {
      G4VSolid* product = new G4ExtrudedSolid(productName,
                                              p.Points(),
                                              0.5 * productLength + productLengthExtra,
                                              zOffsets, zScale,  // dx,dy offset for each face, scaling
                                              zOffsets, zScale); // dx,dy offset for each face, scaling
      return product;
    }
  else
    {
      G4double maxRadius = productApertureIn->RadiusToEncompass();
      G4VSolid* cut = CutSolid(productName + "_angled", maxRadius);
      G4VSolid* square = new G4ExtrudedSolid(productName + "_square",
                                            p.Points(),
                                            productLength + productLengthExtra,
                                            zOffsets, zScale,  // dx,dy offset for each face, scaling
                                            zOffsets, zScale); // dx,dy offset for each face, scaling
      G4VSolid* product = new G4IntersectionSolid(productName, square, cut);
      return product;
    }
}

G4VSolid* BDSApertureFactory::CreateRectCircle() const
{
  const BDSApertureRectCircle* ap = dynamic_cast<const BDSApertureRectCircle*>(productApertureIn);
  if (!ap)
    {return nullptr;}
  if (!angledFaces)
    {
      G4VSolid* circle = new G4Tubs(productName + "_circle",
                                               0,
                                               ap->radius,
                                               0.5 * productLength + productLengthExtra,
                                               0,
                                               CLHEP::twopi);
      G4VSolid* rect = new G4Box(productName + "_rectangle",
                                 ap->a,
                                 ap->b,
                                 productLength + productLengthExtra);
      G4VSolid* product = new G4IntersectionSolid(productName, circle, rect);
      return product;
    }
  else
    {
      G4double maxRadius = ap->RadiusToEncompass();
      G4VSolid* cut = CutSolid(productName + "_angled", maxRadius);
      G4VSolid* circle = new G4Tubs(productName + "_circle",
                                    0,
                                    ap->radius,
                                    productLength + productLengthExtra,
                                    0,
                                    CLHEP::twopi);
      G4VSolid* rect = new G4Box(productName + "_rectangle",
                                 ap->a,
                                 ap->b,
                                 1.5*productLength + productLengthExtra);
      G4VSolid* ellipseRect = new G4IntersectionSolid(productName, circle, rect);
      G4VSolid* product = new G4IntersectionSolid(productName, ellipseRect, cut);
      return product;
    }
}

G4VSolid* BDSApertureFactory::CreateRectEllipse() const
{
  const BDSApertureRectEllipse* ap = dynamic_cast<const BDSApertureRectEllipse*>(productApertureIn);
  if (!ap)
    {return nullptr;}
  if (!angledFaces)
    {
      G4VSolid* ellipse = new G4EllipticalTube(productName + "_ellipse",
                                               ap->ellipseA,
                                               ap->ellipseB,
                                               0.5 * productLength + productLengthExtra);
      G4VSolid* rect = new G4Box(productName + "_rectangle",
                                 ap->rectangleA,
                                 ap->rectangleB,
                                 productLength + productLengthExtra);
      G4VSolid* product = new G4IntersectionSolid(productName, ellipse, rect);
      return product;
    }
  else
    {
      G4double maxRadius = ap->RadiusToEncompass();
      G4VSolid* cut = CutSolid(productName + "_angled", maxRadius);
      G4VSolid* ellipse = new G4EllipticalTube(productName + "_ellipse",
                                               ap->ellipseA,
                                               ap->ellipseB,
                                               productLength + productLengthExtra);
      G4VSolid* rect = new G4Box(productName + "_rectangle",
                                 ap->rectangleA,
                                 ap->rectangleB,
                                 1.5*productLength + productLengthExtra);
      G4VSolid* ellipseRect = new G4IntersectionSolid(productName, ellipse, rect);
      G4VSolid* product = new G4IntersectionSolid(productName, ellipseRect, cut);
      return product;
    }
}

G4VSolid* BDSApertureFactory::CreateRaceTrack() const
{return CreateExtrudedSolid();} //TBC if this is the best way (not intersection?)

G4VSolid* BDSApertureFactory::CreateOctagonal() const
{return CreateExtrudedSolid();}

G4VSolid* BDSApertureFactory::CreateClicPCL() const
{return CreateExtrudedSolid();}

G4VSolid* BDSApertureFactory::CreateDifferentEnds() const
{
  // check specialisations
  auto key    = MakePair(productApertureIn->apertureType, productApertureOut->apertureType);
  auto search = specialisations.find(key);
  if (search != specialisations.end())
    {
      auto mem = search->second;
      return (this->*mem)();
    }
  else // no specialisation, so use high number polygons
    {return CreateTubeByPoints();}
}

G4VSolid* BDSApertureFactory::CreateTubeByPoints() const
{
  unsigned int nPointsIn  = productApertureIn->MinimumNumberOfPoints();
  unsigned int nPointsOut = productApertureOut->MinimumNumberOfPoints();

  G4bool isAMultiple = (nPointsIn % nPointsOut == 0) || (nPointsOut % nPointsIn == 0);
  if (!isAMultiple)
    {
      unsigned int lowestCommonMultiple = BDS::LowestCommonMultiple(nPointsIn, nPointsOut);
      nPointsIn  = lowestCommonMultiple;
      nPointsOut = lowestCommonMultiple;
    }

  BDSPolygon startingPoints  = productApertureIn->Polygon(nPointsIn);
  BDSPolygon finishingPoints = productApertureOut->Polygon(nPointsOut);

  // choose more z points in tube if it twists - approximately 1 z plane per 10-ish degrees
  unsigned int nZ = 2;
  if (productApertureIn->FiniteTilt() || productApertureOut->FiniteTilt())
    {
      G4double dTilt = productApertureOut->tiltOffset.Tilt() - productApertureIn->tiltOffset.Tilt();
      nZ += std::ceil(dTilt / 0.2*CLHEP::radian);
    }
  
  return new BDSTube(productName,
		     productLength + productLengthExtra,
		     startingPoints,
		     finishingPoints,
		     nZ);
}

G4VSolid* BDSApertureFactory::CreateDifferentEndsCircleToCircle() const
{
  const BDSApertureCircle* ap1 = dynamic_cast<const BDSApertureCircle*>(productApertureIn);
  const BDSApertureCircle* ap2 = dynamic_cast<const BDSApertureCircle*>(productApertureOut);
  if (!ap1 || !ap2)
    {return nullptr;}
  G4VSolid* product;
  if (angledFaces)
  {
    productNormalIn.theta();
    // make longer cone, then intersect with cut tubs
    G4Cons* cons = new G4Cons(productName + "_base_cons",
                              0, ap1->radius,
                              0, ap2->radius,
                              0.5*(productLength + productLengthExtra),
                              0, CLHEP::twopi);
    G4double maxRadius = std::max(ap1->radius, ap2->radius);
    G4VSolid* cut = CutSolid(productName + "_angled", maxRadius);
    product = new G4IntersectionSolid(productName, cons, cut);
    return product;
  }
  else
  {
    product = new G4Cons(productName,
                        0, ap1->radius,
                        0, ap2->radius,
                        0.5*(productLength + productLengthExtra),
                        0, CLHEP::twopi);
  }
  return product;
}

G4VSolid* BDSApertureFactory::HollowCircleToCircle() const
{return nullptr;} // TBC

std::pair<BDSApertureType,BDSApertureType> BDSApertureFactory::MakePair(BDSApertureType a1,
									BDSApertureType a2) const
{
  return std::make_pair(std::min(a1, a2), std::max(a1, a2));
}
