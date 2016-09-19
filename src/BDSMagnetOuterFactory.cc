#include "BDSDebug.hh"
#include "BDSExtent.hh"
#include "BDSGeometryComponent.hh"
#include "BDSGeometryFactory.hh"
#include "BDSGeometryExternal.hh"
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
#include "BDSMagnetGeometryType.hh"

#include "globals.hh"                        // geant4 globals / types

#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"

BDSMagnetOuterFactory* BDSMagnetOuterFactory::_instance = nullptr;

BDSMagnetOuterFactory* BDSMagnetOuterFactory::Instance()
{
  if (_instance == nullptr)
    {_instance = new BDSMagnetOuterFactory();}
  return _instance;
}

BDSMagnetOuterFactory::BDSMagnetOuterFactory()
{;}

BDSMagnetOuterFactory::~BDSMagnetOuterFactory()
{
  _instance = nullptr;
}

BDSMagnetOuterFactoryBase* BDSMagnetOuterFactory::GetAppropriateFactory(BDSMagnetGeometryType magnetTypeIn)
{
  switch(magnetTypeIn.underlying()){
    
  case BDSMagnetGeometryType::none:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "'none' magnet factory (no outer geometry)" << G4endl;
#endif
    return BDSMagnetOuterFactoryNone::Instance();
    break;
  case BDSMagnetGeometryType::cylindrical:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "cylindrical magnet factory" << G4endl;
#endif
    return BDSMagnetOuterFactoryCylindrical::Instance();
    break;
  case BDSMagnetGeometryType::polescircular:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "poles with circular yoke factory" << G4endl;
#endif
    return BDSMagnetOuterFactoryPolesCircular::Instance();
    break;
  case BDSMagnetGeometryType::polessquare:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "poles with square yoke factory" << G4endl;
#endif
    return BDSMagnetOuterFactoryPolesSquare::Instance();
    break;
  case BDSMagnetGeometryType::polesfacet:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "poles with faceted yoke factory" << G4endl;
#endif
    return BDSMagnetOuterFactoryPolesFacet::Instance();
    break;
  case BDSMagnetGeometryType::polesfacetcrop:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "poles with faceted and cropped yoke factory" << G4endl;
#endif
    return BDSMagnetOuterFactoryPolesFacetCrop::Instance();
    break;
  case BDSMagnetGeometryType::lhcleft:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "LHC magnet factory - with left offset" << G4endl;
#endif
    return BDSMagnetOuterFactoryLHCLeft::Instance();
    break;
  case BDSMagnetGeometryType::lhcright:
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "LHC magnet factory - with right offset" << G4endl;
#endif
    return BDSMagnetOuterFactoryLHCRight::Instance();
    break;
  case BDSMagnetGeometryType::external:
    return nullptr;
    break;
  default:
    G4cerr << __METHOD_NAME__ << "unknown type \"" << magnetTypeIn << G4endl;
    exit(1);
    break;
  }
}

BDSMagnetOuter* BDSMagnetOuterFactory::CreateMagnetOuter(BDSMagnetType       magnetType,
							 BDSMagnetOuterInfo* outerInfo,
							 G4double            outerLength,
							 G4double            chordLength,
							 BDSBeamPipe*        beampipe)
{
  BDSMagnetOuter* outer = nullptr;

  G4String name                         = outerInfo->name;
  G4double outerDiameter                = outerInfo->outerDiameter;
  G4Material* outerMaterial             = outerInfo->outerMaterial;
  BDSMagnetGeometryType geometryType    = outerInfo->geometryType;

  if (geometryType == BDSMagnetGeometryType::external)
    {
      outer = CreateExternal(name, outerInfo, outerLength, beampipe);
      return outer;
    }
  
  switch(magnetType.underlying())
    {
    case BDSMagnetType::decapole:
      outer = CreateDecapole(geometryType,name,outerLength,beampipe,
			     outerDiameter,chordLength,outerMaterial);
      break;
    case BDSMagnetType::vkicker:
      outer = CreateKicker(geometryType,name,outerLength,beampipe,
			   outerDiameter,chordLength,true,outerMaterial);
      break;
    case BDSMagnetType::hkicker:
      outer = CreateKicker(geometryType,name,outerLength,beampipe,
			   outerDiameter,chordLength,false,outerMaterial);
      break;
    case BDSMagnetType::muonspoiler:
      outer = CreateMuSpoiler(geometryType,name,outerLength,beampipe,
			      outerDiameter,chordLength,outerMaterial);
      break;
    case BDSMagnetType::octupole:
      outer = CreateOctupole(geometryType,name,outerLength,beampipe,
			     outerDiameter,chordLength,outerMaterial);
      break;
    case BDSMagnetType::quadrupole:
      outer = CreateQuadrupole(geometryType,name,outerLength,beampipe,
			       outerDiameter,chordLength,outerMaterial);
      break;
    case BDSMagnetType::rfcavity:
      outer = CreateRfCavity(geometryType,name,outerLength,beampipe,
			     outerDiameter,chordLength,outerMaterial);
      break;
    case BDSMagnetType::sectorbend:
      outer = CreateSectorBend(geometryType,name,outerLength,beampipe,
			       outerDiameter,chordLength,outerInfo->angleIn,
			       outerInfo->angleOut,outerMaterial);
      break;
    case BDSMagnetType::rectangularbend:
      outer = CreateRectangularBend(geometryType,name,outerLength,beampipe,
				    outerDiameter,chordLength,outerInfo->angleIn,
				    outerInfo->angleOut,outerMaterial);
      break;
    case BDSMagnetType::sextupole:
      outer = CreateSextupole(geometryType,name,outerLength,beampipe,
			      outerDiameter,chordLength,outerMaterial);
      break;
    case BDSMagnetType::solenoid:
      outer = CreateSolenoid(geometryType,name,outerLength,beampipe,
			     outerDiameter,chordLength,outerMaterial);
      break;
    case BDSMagnetType::multipole:
      outer = CreateMultipole(geometryType,name,outerLength,beampipe,
			      outerDiameter,chordLength,outerMaterial);
      break;
    default:
      G4cout << __METHOD_NAME__ << "unknown magnet type - no outer volume built" << G4endl;
      break;
    }
  return outer;
}

BDSMagnetOuter* BDSMagnetOuterFactory::CreateSectorBend(BDSMagnetGeometryType magnetType,
							G4String      name,
							G4double      length,
							BDSBeamPipe*  beamPipe,
							G4double      outerDiameter,
							G4double      containerLength,
							G4double      angleIn,
							G4double      angleOut,
							G4bool        yokeOnLeft,
							G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateSectorBend(name, length, beamPipe, outerDiameter, containerLength,
				   angleIn, angleOut, yokeOnLeft, outerMaterial);
}

BDSMagnetOuter* BDSMagnetOuterFactory::CreateRectangularBend(BDSMagnetGeometryType magnetType,
							     G4String      name,
							     G4double      length,
							     BDSBeamPipe*  beamPipe,
							     G4double      outerDiameter,
							     G4double      containerLength,
							     G4double      angleIn,
							     G4double      angleOut,
							     G4bool        yokeOnLeft,
							     G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateRectangularBend(name, length, beamPipe, outerDiameter,
					containerLength, angleIn, angleOut, yokeOnLeft, outerMaterial);
}
  

BDSMagnetOuter* BDSMagnetOuterFactory::CreateQuadrupole(BDSMagnetGeometryType magnetType,
							G4String      name,
							G4double      length,
							BDSBeamPipe*  beamPipe,
							G4double      outerDiameter,
							G4double      containerLength,
							G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateQuadrupole(name, length, beamPipe, outerDiameter, containerLength, outerMaterial);
}

BDSMagnetOuter* BDSMagnetOuterFactory::CreateSextupole(BDSMagnetGeometryType magnetType,
						       G4String      name,
						       G4double      length,
						       BDSBeamPipe*  beamPipe,
						       G4double      outerDiameter,
						       G4double      containerLength,
						       G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateSextupole(name, length, beamPipe, outerDiameter, containerLength, outerMaterial);
}

BDSMagnetOuter* BDSMagnetOuterFactory::CreateOctupole(BDSMagnetGeometryType magnetType,
						      G4String      name,
						      G4double      length,
						      BDSBeamPipe*  beamPipe,
						      G4double      outerDiameter,
						      G4double      containerLength,
						      G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateOctupole(name, length, beamPipe, outerDiameter, containerLength, outerMaterial);
}

BDSMagnetOuter* BDSMagnetOuterFactory::CreateDecapole(BDSMagnetGeometryType magnetType,
						      G4String      name,
						      G4double      length,
						      BDSBeamPipe*  beamPipe,
						      G4double      outerDiameter,
						      G4double      containerLength,
						      G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateDecapole(name, length, beamPipe, outerDiameter, containerLength, outerMaterial);
}

BDSMagnetOuter* BDSMagnetOuterFactory::CreateSolenoid(BDSMagnetGeometryType magnetType,
						      G4String      name,
						      G4double      length,
						      BDSBeamPipe*  beamPipe,
						      G4double      outerDiameter,
						      G4double      containerLength,
						      G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateSolenoid(name, length, beamPipe, outerDiameter, containerLength, outerMaterial);
}

BDSMagnetOuter* BDSMagnetOuterFactory::CreateMultipole(BDSMagnetGeometryType magnetType,
						       G4String      name,
						       G4double      length,
						       BDSBeamPipe*  beamPipe,
						       G4double      outerDiameter,
						       G4double      containerLength,
						       G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateMultipole(name, length, beamPipe, outerDiameter, containerLength, outerMaterial);
}

BDSMagnetOuter* BDSMagnetOuterFactory::CreateRfCavity(BDSMagnetGeometryType magnetType,
						      G4String      name,
						      G4double      length,
						      BDSBeamPipe*  beamPipe,
						      G4double      outerDiameter,
						      G4double      containerLength,
						      G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateRfCavity(name, length, beamPipe, outerDiameter, containerLength, outerMaterial);
}

BDSMagnetOuter* BDSMagnetOuterFactory::CreateMuSpoiler(BDSMagnetGeometryType magnetType,
						       G4String      name,
						       G4double      length,
						       BDSBeamPipe*  beamPipe,
						       G4double      outerDiameter,
						       G4double      containerLength,
						       G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateMuSpoiler(name, length, beamPipe, outerDiameter, containerLength, outerMaterial);
}

BDSMagnetOuter* BDSMagnetOuterFactory::CreateKicker(BDSMagnetGeometryType magnetType,
						    G4String      name,
						    G4double      length,
						    BDSBeamPipe*  beamPipe,
						    G4double      outerDiameter,
						    G4double      containerLength,
						    G4bool        vertical,
						    G4Material*   outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(magnetType);
  return factory->CreateKicker(name, length, beamPipe, outerDiameter, containerLength, vertical, outerMaterial);
}

BDSMagnetOuter* BDSMagnetOuterFactory::CreateExternal(G4String            name,
						      BDSMagnetOuterInfo* info,
						      G4double            length,
						      BDSBeamPipe*        beampipe)
{
  BDSGeometryExternal* geom = BDSGeometryFactory::Instance()->BuildGeometry(info->geometryTypeAndPath);

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
    
  BDSGeometryComponent* container = CreateContainer(name, length, geom);
  
  BDSMagnetOuter* outer = new BDSMagnetOuter(geom, container);
  return outer;
}

BDSGeometryComponent* BDSMagnetOuterFactory::CreateContainer(G4String             name,
							     G4double             length,
							     BDSGeometryExternal* external)
{
  G4double magnetContainerRadius = 3*CLHEP::m;
  G4VSolid* magnetContainerSolid = new G4Tubs(name + "_container_solid",   // name
				    0,                           // inner radius
				    magnetContainerRadius,       // outer radius
				    length * 0.5, // z half length
				    0,                           // starting angle
				    CLHEP::twopi);               // sweep angle

  G4Material* emptyMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->EmptyMaterial());
  G4LogicalVolume* magnetContainerLV = new G4LogicalVolume(magnetContainerSolid,
							   emptyMaterial,
							   name + "_container_lv");

  magnetContainerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetContainerVisAttr());

  BDSExtent magContExtent = BDSExtent(magnetContainerRadius, magnetContainerRadius, length*0.5);

  BDSGeometryComponent* magnetContainer = new BDSGeometryComponent(magnetContainerSolid,
								   magnetContainerLV,
								   magContExtent);
  
  return magnetContainer;
}
						      
