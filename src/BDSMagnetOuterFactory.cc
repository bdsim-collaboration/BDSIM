/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

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
#include "BDSBeamPipe.hh"
#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSExtent.hh"
#include "BDSGeometryComponent.hh"
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactory.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMagnetOuter.hh"
#include "BDSMagnetOuterFactory.hh"
#include "BDSMagnetOuterFactoryBase.hh"
#include "BDSMagnetOuterFactoryCylindrical.hh"
#include "BDSMagnetOuterFactoryPolesCircular.hh"
#include "BDSMagnetOuterFactoryPolesFacet.hh"
#include "BDSMagnetOuterFactoryPolesFacetCrop.hh"
#include "BDSMagnetOuterFactoryPolesSquare.hh"
#include "BDSMagnetOuterFactoryLHCLeft.hh"
#include "BDSMagnetOuterFactoryLHCRight.hh"
#include "BDSMagnetOuterFactoryNone.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetGeometryType.hh"
#include "BDSMaterials.hh"

#include "globals.hh"         // geant4 globals / types
#include "G4Box.hh"
#include "G4CutTubs.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"

#include <algorithm>
#include <cmath>

BDSMagnetOuterFactory* BDSMagnetOuterFactory::instance = nullptr;

BDSMagnetOuterFactory* BDSMagnetOuterFactory::Instance()
{
  if (instance == nullptr)
    {instance = new BDSMagnetOuterFactory();}
  return instance;
}

BDSMagnetOuterFactory::BDSMagnetOuterFactory()
{;}

BDSMagnetOuterFactory::~BDSMagnetOuterFactory()
{
  delete BDSMagnetOuterFactoryNone::Instance();
  delete BDSMagnetOuterFactoryCylindrical::Instance();
  delete BDSMagnetOuterFactoryPolesCircular::Instance();
  delete BDSMagnetOuterFactoryPolesSquare::Instance();
  delete BDSMagnetOuterFactoryPolesFacet::Instance();
  delete BDSMagnetOuterFactoryPolesFacetCrop::Instance();
  delete BDSMagnetOuterFactoryLHCRight::Instance();
  delete BDSMagnetOuterFactoryLHCLeft::Instance();
  instance = nullptr;
}

BDSMagnetOuterFactoryBase* BDSMagnetOuterFactory::GetAppropriateFactory(BDSMagnetGeometryType magnetTypeIn)
{
  switch(magnetTypeIn.underlying())
    {
    case BDSMagnetGeometryType::none:
      {return BDSMagnetOuterFactoryNone::Instance(); break;}
    case BDSMagnetGeometryType::cylindrical:
      {return BDSMagnetOuterFactoryCylindrical::Instance(); break;}
    case BDSMagnetGeometryType::polescircular:
      {return BDSMagnetOuterFactoryPolesCircular::Instance(); break;}
    case BDSMagnetGeometryType::polessquare:
      {return BDSMagnetOuterFactoryPolesSquare::Instance(); break;}
    case BDSMagnetGeometryType::polesfacet:
      {return BDSMagnetOuterFactoryPolesFacet::Instance(); break;}
    case BDSMagnetGeometryType::polesfacetcrop:
      {return BDSMagnetOuterFactoryPolesFacetCrop::Instance(); break;}
    case BDSMagnetGeometryType::lhcleft:
      {return BDSMagnetOuterFactoryLHCLeft::Instance(); break;}
    case BDSMagnetGeometryType::lhcright:
      {return BDSMagnetOuterFactoryLHCRight::Instance(); break;}
    case BDSMagnetGeometryType::external:
      {return nullptr; break;}
    default:
      {
	G4cerr << __METHOD_NAME__ << "unknown type \"" << magnetTypeIn << G4endl;
	exit(1);
	break;
      }
    }
}

BDSMagnetOuter* BDSMagnetOuterFactory::CreateMagnetOuter(BDSMagnetType       magnetType,
							 BDSMagnetOuterInfo* outerInfo,
							 G4double            outerLength,
							 G4double            containerLength,
							 BDSBeamPipe*        beamPipe)
{
  BDSMagnetOuter* outer = nullptr;

  G4String name                         = outerInfo->name;
  G4double outerDiameter                = outerInfo->outerDiameter;
  G4Material* outerMaterial             = outerInfo->outerMaterial;
  BDSMagnetGeometryType geometryType    = outerInfo->geometryType;
  G4bool yokeOnLeft                     = outerInfo->yokeOnLeft;
  G4bool buildEndPiece                  = outerInfo->buildEndPieces;

  if (geometryType == BDSMagnetGeometryType::external)
    {
      outer = CreateExternal(name, outerInfo, outerLength, beamPipe);
      G4double loadedLength = outer->GetExtent().DZ();
      if (loadedLength > outerLength)
	{
	  G4cerr << "External geometry of length " << loadedLength/CLHEP::m
		 << "m too long for magnet of length " << outerLength/CLHEP::m
		 << "m. Geometry Specification:" << G4endl << *outerInfo;
	  exit(1);
	}
      return outer;
    }

  // Check dimensions
  CheckOuterBiggerThanBeamPipe(name, outerInfo, beamPipe);

  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(geometryType);
  
  switch(magnetType.underlying())
    {
    case BDSMagnetType::decapole:
      {
	outer = factory->CreateDecapole(name, outerLength, beamPipe, outerDiameter,
					containerLength, outerMaterial, buildEndPiece);
	break;
      }
    case BDSMagnetType::vkicker:
      {
	outer = factory->CreateKicker(name, outerLength, beamPipe, outerDiameter,
				      containerLength, true, outerMaterial, buildEndPiece);
	break;
      }
    case BDSMagnetType::hkicker:
      {
	outer = factory->CreateKicker(name, outerLength, beamPipe, outerDiameter,
				      containerLength, false, outerMaterial, buildEndPiece);
	break;
      }
    case BDSMagnetType::muonspoiler:
      {
	outer = factory->CreateMuSpoiler(name, outerLength, beamPipe, outerDiameter,
					 containerLength, outerMaterial, buildEndPiece);
	break;
      }
    case BDSMagnetType::octupole:
      {
	outer = factory->CreateOctupole(name, outerLength, beamPipe, outerDiameter,
					containerLength, outerMaterial, buildEndPiece);
	break;
      }
    case BDSMagnetType::quadrupole:
      {
	outer = factory->CreateQuadrupole(name, outerLength, beamPipe, outerDiameter,
					  containerLength, outerMaterial, buildEndPiece);
	break;
      }
    case BDSMagnetType::rfcavity:
      {
	outer = factory->CreateRfCavity(name, outerLength, beamPipe, outerDiameter,
					containerLength, outerMaterial, buildEndPiece);
	break;
      }
    case BDSMagnetType::sectorbend:
      {
	outer = factory->CreateSectorBend(name, outerLength, beamPipe,
					  outerDiameter, containerLength,
					  outerInfo->angleIn, outerInfo->angleOut,
					  yokeOnLeft, outerMaterial, buildEndPiece);
	break;
      }
    case BDSMagnetType::rectangularbend:
      {
	outer = factory->CreateRectangularBend(name, outerLength, beamPipe,
					       outerDiameter, containerLength,
					       outerInfo->angleIn, outerInfo->angleOut,
					       yokeOnLeft, outerMaterial, buildEndPiece);
	break;
      }
    case BDSMagnetType::sextupole:
      {
	outer = factory->CreateSextupole(name, outerLength, beamPipe, outerDiameter,
					 containerLength, outerMaterial, buildEndPiece);
	break;
      }
    case BDSMagnetType::solenoid:
      {
	outer = factory->CreateSolenoid(name, outerLength, beamPipe, outerDiameter,
					containerLength, outerMaterial, buildEndPiece);
	break;
      }
    case BDSMagnetType::multipole:
      {
	outer = factory->CreateMultipole(name, outerLength, beamPipe, outerDiameter,
					 containerLength, outerMaterial, buildEndPiece);
	break;
      }
    case BDSMagnetType::thinmultipole:
    case BDSMagnetType::dipolefringe:
      {break;} // leave as nullptr - no outer geometry for dipole fringe or thin multipole
    default:
      G4cout << __METHOD_NAME__ << "unknown magnet type " << magnetType << " - no outer volume built" << G4endl;
      break;
    }
  return outer;
}

BDSMagnetOuter* BDSMagnetOuterFactory::CreateExternal(G4String            name,
						      BDSMagnetOuterInfo* info,
						      G4double            length,
						      BDSBeamPipe*        beampipe)
{
  std::map<G4String, G4Colour*> defaultMap = {
    {"coil", BDSColours::Instance()->GetColour("coil")},
    {"bend", BDSColours::Instance()->GetColour("sectorbend")},
    {"yoke", BDSColours::Instance()->GetColour("quadrupole")},
    {"quad", BDSColours::Instance()->GetColour("quadrupole")},
    {"sext", BDSColours::Instance()->GetColour("sextupole")},
    {"oct",  BDSColours::Instance()->GetColour("octupole")},
    {"dec",  BDSColours::Instance()->GetColour("decapole")}
  };
  BDSGeometryExternal* geom = BDSGeometryFactory::Instance()->BuildGeometry(name,
									    info->geometryTypeAndPath,
									    &defaultMap);

  BDSExtent bpExtent = beampipe->GetExtent();
  BDSExtent magInner = geom->GetInnerExtent();
  
  if (magInner.TransverselyLessThan(bpExtent))
    {
      G4cout << info->geometryTypeAndPath
	     << " will not fit around beam pipe in element \"" << name << "\"" <<G4endl;
      G4cout << "Determined extents to be: " << G4endl;
      G4cout << "External geometry inner "   << G4endl;
      G4cout << magInner                     << G4endl;
      G4cout << "Beam pipe outer "           << G4endl;
      G4cout << bpExtent                     << G4endl;
      exit(1);
    }
    
  BDSGeometryComponent* container = CreateContainerForExternal(name, length, geom, beampipe);
  
  BDSMagnetOuter* outer = new BDSMagnetOuter(geom, container);
  return outer;
}

BDSGeometryComponent* BDSMagnetOuterFactory::CreateContainerForExternal(G4String             name,
									G4double             length,
									BDSGeometryExternal* external,
									BDSBeamPipe*         beampipe)
{
  G4ThreeVector  inputFace = beampipe->InputFaceNormal();
  G4ThreeVector outputFace = beampipe->OutputFaceNormal();

  BDSExtent outer = external->GetExtent();
  G4VSolid* containerSolid;
  BDSExtent containerExt;
  if ((inputFace.z() > -1) || (outputFace.z() < 1))
    {// use a cut tubs for angled face
      G4double posR = std::hypot(outer.XPos(),outer.YPos());
      G4double negR = std::hypot(outer.XNeg(),outer.YNeg());
      G4double magnetContainerRadius = std::max(posR, negR) + 1*CLHEP::mm; // generous margin
      containerSolid = new G4CutTubs(name + "_container_solid",   // name
				     0,                           // inner radius
				     magnetContainerRadius,       // outer radius
				     length * 0.5,                // z half length
				     0,                           // starting angle
				     CLHEP::twopi,                // sweep angle
				     inputFace,
				     outputFace);
      containerExt = BDSExtent(magnetContainerRadius, magnetContainerRadius, 0.5*length);
    }
  else
    {// flat faces so use a box
      G4double radius = outer.MaximumAbsTransverse() + 1*CLHEP::mm; // generous margin
      containerSolid = new G4Box(name + "_container_solid", // name
				 radius,
				 radius,
				 length*0.5);
      containerExt = BDSExtent(radius, radius, length*0.5);
    }

  G4Material* emptyMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->EmptyMaterial());
  G4LogicalVolume* containerLV = new G4LogicalVolume(containerSolid,
						     emptyMaterial,
						     name + "_container_lv");

  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetContainerVisAttr());

  BDSGeometryComponent* container = new BDSGeometryComponent(containerSolid,
							     containerLV,
							     containerExt);
  
  return container;
}
						      
void BDSMagnetOuterFactory::CheckOuterBiggerThanBeamPipe(const G4String            name,
							 const BDSMagnetOuterInfo* outerInfo,
							 const BDSBeamPipe*        beamPipe) const
{

  G4double od = outerInfo->outerDiameter;
  BDSExtent bpExtent = beamPipe->GetExtent();
  if (od < bpExtent.DX() || od < bpExtent.DY())
    {
      G4cerr << "Magnet outer dimensions too small to encompass beam pipe for element " << name << G4endl;
      G4cerr << "outerDiameter -> " << od << G4endl;
      G4cerr << "Beam pipe width : " << bpExtent.DX() << ", height : " << bpExtent.DY() << G4endl;
      exit(1);
    }
}
