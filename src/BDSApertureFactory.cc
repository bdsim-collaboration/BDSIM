/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#include "BDSApertureEllpse.hh"
#include "BDSApertureFactory.hh"
#include "BDSApertureOctagon.hh"
#include "BDSApertureRaceTrack.hh"
#include "BDSApertureRectangle.hh"
#include "BDSApertureRectCircle.hh"
#include "BDSApertureRectEllipse.hh"
#include "BDSApertureType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSPolygon.hh"
#include "BDSTube.hh"
#include "BDSUtilities.hh"

#include "parser/aperture.h"
#include "parser/samplerplacement.h"

#include "G4Box.hh"
#include "G4CutTubs.hh"
#include "G4IntersectionSolid.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4TwoVector.hh"
#include "G4Types.hh"
#include "G4VSolid.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <algorithm>
#include <map>
#include <utility>
#include <vector>


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
		     {MakePair(BDSApertureType::circle,
			       BDSApertureType::circle), &BDSApertureFactory::CircleToCircle}
  };
}

BDSApertureFactory::~BDSApertureFactory()
{;}

BDSAperture* BDSApertureFactory::CreateAperture(const GMAD::Aperture& ap) const
{
  return CreateAperture(BDS::DetermineApertureType(ap.apertureType),
			ap.aper1 * CLHEP::m,
			ap.aper2 * CLHEP::m,
			ap.aper3 * CLHEP::m,
			ap.aper4 * CLHEP::m,
			ap.tilt  * CLHEP::rad,
			ap.offsetX * CLHEP::m,
			ap.offsetY * CLHEP::m,
			ap.nPoints);
}

BDSAperture* BDSApertureFactory::CreateAperture(const GMAD::SamplerPlacement& sp) const
{
  return CreateAperture(BDS::DetermineApertureType(sp.shape),
			sp.aper1 * CLHEP::m,
			sp.aper2 * CLHEP::m,
			sp.aper3 * CLHEP::m,
			sp.aper4 * CLHEP::m,
			sp.tilt  * CLHEP::rad,
			0,
			0,
			sp.nPoints);
}

BDSAperture* BDSApertureFactory::CreateAperture(BDSApertureType at,
						G4double        a1,
						G4double        a2,
						G4double        a3,
						G4double        a4,
						G4double        tilt,
						G4double        offsetX,
						G4double        offsetY,
						unsigned int    nPoints) const
{
  BDSAperture* result = nullptr;
  switch (at.underlying())
    {
    case BDSApertureType::circle:
      {result = new BDSApertureCircle(a1, nPoints);       break;}
    case BDSApertureType::ellipse:
      {result = new BDSApertureEllpse(a1, a2, nPoints); break;}
    case BDSApertureType::rectangle:
      {result = new BDSApertureRectangle(a1, a2);         break;}
    case BDSApertureType::rectcircle:
      {result = new BDSApertureRectCircle(a1, a2, a3);      break;}
    case BDSApertureType::rectellipse:
      {result = new BDSApertureRectEllipse(a1, a2, a3, a4); break;}
    case BDSApertureType::racetrack:
      {result = new BDSApertureRaceTrack(a1, a2, a3);   break;}
    case BDSApertureType::octagon:
      {result = new BDSApertureOctagon(a1, a2, a3, a4);   break;}
    default:
      {break;}
    }
  if (result)
    {result->SetTiltOffset(BDSTiltOffset(tilt, offsetX, offsetY));}
  return result;
}

G4VSolid* BDSApertureFactory::CreateSolid(const G4String&    name,
					  G4double           length,
					  const BDSAperture* apertureIn,
					  const BDSAperture* apertureOut,
					  G4double           lengthExtraForBoolean)
{
  productNormalIn  = G4ThreeVector();
  productNormalOut = G4ThreeVector();
  angledFaces      = false;

  return CommonConstruction(name, length, apertureIn, apertureOut, lengthExtraForBoolean);
}

G4VSolid* BDSApertureFactory::CreateSolid(const G4String&      name,
					  G4double             length,
					  const BDSAperture*   apertureIn,
					  const G4ThreeVector& normalIn,
					  const G4ThreeVector& normalOut,
					  const BDSAperture*   apertureOut,
					  G4double             lengthExtraForBoolean)
{
  productNormalIn  = normalIn;
  productNormalOut = normalOut;
  angledFaces      = true;

  return CommonConstruction(name, length, apertureIn, apertureOut, lengthExtraForBoolean);
}

G4VSolid* BDSApertureFactory::CommonConstruction(const G4String&    name,
						 G4double           length,
						 const BDSAperture* apertureIn,
						 const BDSAperture* apertureOut,
						 G4double           lengthExtraForBoolean)
{
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
      {product = CreateCircle();    break;}
    case BDSApertureType::rectangle:
      {product = CreateRectangle(); break;}
    case BDSApertureType::ellipse:
      {product = CreateEllipse();  break;}
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
    default:
      {break;}
    }
  return product;
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
      G4double intersectionRadius = intersectionRadiusRatio * maxRadius;
      G4VSolid* cut = new G4CutTubs(productName + "_angled",
				    0,
				    intersectionRadius,
				    0.5 * productLength + productLengthExtra,
				    0,
				    CLHEP::twopi,
				    productNormalIn,
				    productNormalOut);
      G4VSolid* product = new G4IntersectionSolid(productName, box, cut);
      return product;
    }
}

G4VSolid* BDSApertureFactory::CreateEllipse() const
{return CreateCircle();}
G4VSolid* BDSApertureFactory::CreateRectCircle() const
{return CreateCircle();}
G4VSolid* BDSApertureFactory::CreateRectEllipse() const
{return CreateCircle();}
G4VSolid* BDSApertureFactory::CreateRaceTrack() const
{return CreateCircle();}
G4VSolid* BDSApertureFactory::CreateOctagonal() const
{return CreateCircle();}
G4VSolid* BDSApertureFactory::CreateClicPCL() const
{return CreateCircle();}

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
  G4int nPointsIn  = productApertureIn->MinimumNumberOfPoints();
  G4int nPointsOut = productApertureOut->MinimumNumberOfPoints();

  G4bool isAMultiple = (nPointsIn % nPointsOut == 0) || (nPointsOut % nPointsIn == 0);
  if (!isAMultiple)
    {
      G4int lowestCommonMultiple = BDS::LowestCommonMultiple(nPointsIn, nPointsOut);
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

G4VSolid* BDSApertureFactory::CircleToCircle() const
{return nullptr;}

std::pair<BDSApertureType,BDSApertureType> BDSApertureFactory::MakePair(BDSApertureType a1,
									BDSApertureType a2) const
{
  return std::make_pair(std::min(a1, a2), std::max(a1, a2));
}
