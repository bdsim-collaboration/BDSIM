/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2019.

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
#include "BDSApertureFactory.hh"
#include "BDSApertureInfo.hh"
#include "BDSApertureType.hh"

#include "globals.hh"
#include "G4Box.hh"
#include "G4CutTubs.hh"
#include "G4IntersectionSolid.hh"
#include "G4Tubs.hh"
#include "G4VSolid.hh"

#include "CLHEP/Units/SystemOfUnits.h"


BDSApertureFactory::BDSApertureFactory():
  intersectionRadiusRatio(1.3),
  productName(""),
  productLength(0),
  productShapeIn(nullptr),
  productShapeOut(nullptr),
  productLengthExtra(0),
  angledFaces(false)
{;}

BDSApertureFactory::~BDSApertureFactory()
{;}

G4VSolid* CreateAperture(G4String name,
			 G4double length,
			 BDSApertureInfo* shapeIn,
			 BDSApertureInfo* shapeOut,
			 G4double lengthExtraForBoolean)
{
  G4bool tapered = shapeOut; // ie valid pointer for shape out.
  G4bool sameShape = true;
  if (shapeOut)
    {sameShape = shapeIn->apertureType == shapeOut->apertureType;}

  productName        = name;
  productLength      = length;
  productShapeIn     = shapeIn;
  productSahpeOut    = shapeOut;
  productLengthExtra = lengthExtraForBoolean;
  productNormalIn    = G4ThreeVector();
  productNormalOut   = G4ThreeVector();
  angledFaces        = false;

  if (!sameShape)
    {return CreateDifferentEnds();}

  // else create non-tapering piece of geometry
  G4VSolid* product = nullptr;
  if (!tapered)
    {
      switch (shapeIn->apertureType.underlying())
	{
	case BDSApertureType::circular:
	  {product = CreateCircular(); break;}
	case BDSApertureType::rectangular:
	  {product = CreateRectangular(); break;}
	case BDSApertureType::elliptical:
	  {product = CreateElliptical(); break;}
	case BDSApertureType::lhc:
	  {product = CreateLHC(); break;}
	case BDSApertureType::rectellipse:
	  {product = CreateRectEllipse(); break;}
	case BDSApertureType::racetrack:
	  {product = CreateRaceTrack(); break;}
	case BDSApertureType::octagonal:
	  {product = CreateOctagonal(); break;}
	case BDSApertureType::clicpcl:
	  {product = CreateClicPCL(); break;}
	default:
	  {break;}
	}
    }
  else
    {
      switch (shapeIn->apertureType.underlying())
	{
	case BDSApertureType::circular:
	  {product = CreateCircularTapered(); break;}
	case BDSApertureType::rectangular:
	  {product = CreateRectangularTapered(); break;}
	case BDSApertureType::elliptical:
	  {product = CreateEllipticalTapered(); break;}
	case BDSApertureType::lhc:
	  {product = CreateLHCTapered(); break;}
	case BDSApertureType::rectellipse:
	  {product = CreateRectEllipseTapered(); break;}
	case BDSApertureType::racetrack:
	  {product = CreateRaceTrackTapered(); break;}
	case BDSApertureType::octagonal:
	  {product = CreateOctagonalTapered(); break;}
	case BDSApertureType::clicpcl:
	  {product = CreateClicPCLTapered(); break;}
	default:
	  {break;}
	}
    }
  return product;
}

G4VSolid* BDSApertureFactory::CreateCircular() const
{
  if (!angledFaces)
    {
      G4VSolid* product = new G4Tubs(productName,
				     0
				     productShapeIn->aper1,
				     0.5 * productLength + productLengthExtra,
				     0,
				     CLHEP::twopi);
      return product;
    }
  else
    {
      G4VSolid* product = new G4CutTubs(productName,
					0,
					productShapeIn->aper1,
					0.5 * productLength + productLengthExtra,
					0,
					CLHEP::twopi,
					productNormalIn,
					productNormalOut);
      return product;
    }
}

G4VSolid* BDSApertureFactory::CreateRectangular() const
{
  if (!angledFaces)
    {
      G4VSolid* product = new G4Box(productName,
				    productShapeIn->aper1,
				    prodcutShapeIn->aper2,
				    0.5 * productLength + productLengthExtra);
      return product;
    }
  else
    {
      G4VSolid* box = new G4Box(productName + "_square",
				productShapeIn->aper1,
				productShapeIn->aper2,
				productLength + productLengthExtra); // factor 2 here
      /// TBC - get more accurate maximum radius from axis here
      G4double maxRadius = productShapeIn->IndicativeRadius();
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

G4VSolid* BDSApertureFactory::CreateElliptical() const
{return CreateCircular();}
G4VSolid* BDSApertureFactory::CreateLHC() const
{return CreateCircular();}
G4VSolid* BDSApertureFactory::CreateRectEllipse() const
{return CreateCircular();}
G4VSolid* BDSApertureFactory::CreateRaceTrack() const
{return CreateCircular();}
G4VSolid* BDSApertureFactory::CreateOctagonal() const
{return CreateCircular();}
G4VSolid* BDSApertureFactory::CreateClicPCL() const
{return CreateCircular();}
G4VSolid* BDSApertureFactory::CreateCircularTapered() const
{return CreateCircular();}
G4VSolid* BDSApertureFactory::CreateRectangularTapered() const
{return CreateCircular();}
G4VSolid* BDSApertureFactory::CreateEllipticalTapered() const
{return CreateCircular();}
G4VSolid* BDSApertureFactory::CreateLHCTapered() const
{return CreateCircular();}
G4VSolid* BDSApertureFactory::CreateRectEllipseTapered() const
{return CreateCircular();}
G4VSolid* BDSApertureFactory::CreateRaceTrackTapered() const
{return CreateCircular();}
G4VSolid* BDSApertureFactory::CreateOctagonalTapered() const
{return CreateCircular();}
G4VSolid* BDSApertureFactory::CreateClicPCLTapered() const
{return CreateCircular();}
G4VSolid* BDSApertureFactory::CreateDifferentEnds() const
{return CreateCircular();}
