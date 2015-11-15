#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSDebug.hh"

#include <cstdlib>
#include <cmath>
#include <string>

#include "G4Box.hh"
#include "G4CutTubs.hh"
#include "G4LogicalVolume.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4MagneticField.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include "G4VPhysicalVolume.hh"

#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSMagnetOuter.hh"
#include "BDSMagnetOuterFactory.hh"
#include "BDSMagnetType.hh"
#include "BDSMagnet.hh"
#include "BDSMultipoleOuterMagField.hh"

BDSMagnet::BDSMagnet(BDSMagnetType       type,
		     G4String            name,
		     G4double            length,
		     BDSBeamPipeInfo*    beamPipeInfoIn,
		     BDSMagnetOuterInfo* magnetOuterInfoIn):
  BDSAcceleratorComponent(name, length, 0, (*BDSMagnetType::dictionary)[type]),
  magnetType(type),
  beamPipeInfo(beamPipeInfoIn),
  magnetOuterInfo(magnetOuterInfoIn),
  magnetOuterOffset(G4ThreeVector(0,0,0))
{
  outerDiameter   = magnetOuterInfo->outerDiameter;
  containerRadius = 0.5*outerDiameter;
  inputface       = G4ThreeVector(0,0,0);
  outputface      = G4ThreeVector(0,0,0);
  
  itsStepper       = nullptr;
  itsMagField      = nullptr;
  itsEqRhs         = nullptr;
  itsBPFieldMgr    = nullptr;
  itsOuterFieldMgr = nullptr;
  itsChordFinder   = nullptr;
  itsOuterMagField = nullptr;
  
  beampipe = nullptr;
  outer    = nullptr;

  placeBeamPipe = false;
}

void BDSMagnet::Build()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif  
  BuildBeampipe();           // build beam pipe without placing it
  BuildBPFieldAndStepper();  // build magnetic field - provided by derived class
  BuildBPFieldMgr();         // build the field manager - done here
  AttachFieldToBeamPipe();   // attach the magnetic field to the vacuum
  BuildOuter();              // build outer and update container solid
  //BuildOuterFieldManager(..) // unused currently - uncomment when reimplemented!!!
  //AttachFieldToOuter();
  
  // calls BuildContainerLogicalVolume() (implemented in this class) which
  // builds the container if needed
  BDSAcceleratorComponent::Build(); 

  PlaceComponents();    // place things (if needed) in container
}

void BDSMagnet::BuildBeampipe()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  
  beampipe = BDSBeamPipeFactory::Instance()->CreateBeamPipe(name,
							    chordLength - lengthSafety,
							    beamPipeInfo);

  RegisterDaughter(beampipe);

  SetAcceleratorVacuumLogicalVolume(beampipe->GetVacuumLogicalVolume());
}

void BDSMagnet::BuildBPFieldMgr()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  if (itsMagField && itsEqRhs && itsStepper)
    {
      itsChordFinder = new G4ChordFinder(itsMagField,
					 BDSGlobalConstants::Instance()->GetChordStepMinimum(),
					 itsStepper);
      
      itsChordFinder->SetDeltaChord(BDSGlobalConstants::Instance()->GetDeltaChord());
      itsBPFieldMgr = new G4FieldManager();
      itsBPFieldMgr->SetDetectorField(itsMagField);
      itsBPFieldMgr->SetChordFinder(itsChordFinder);
      
      // these options are always non-zero so always set them
      itsBPFieldMgr->SetDeltaIntersection(BDSGlobalConstants::Instance()->GetDeltaIntersection());
      itsBPFieldMgr->SetMinimumEpsilonStep(BDSGlobalConstants::Instance()->GetMinimumEpsilonStep());
      itsBPFieldMgr->SetMaximumEpsilonStep(BDSGlobalConstants::Instance()->GetMaximumEpsilonStep());
      itsBPFieldMgr->SetDeltaOneStep(BDSGlobalConstants::Instance()->GetDeltaOneStep());
    }
}

void BDSMagnet::AttachFieldToBeamPipe()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // SET FIELD
  if(itsBPFieldMgr)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "field exists and attaching" << G4endl;
#endif
      beampipe->GetVacuumLogicalVolume()->SetFieldManager(itsBPFieldMgr,false);
    }
#ifdef BDSDEBUG
  else
    {G4cout << __METHOD_NAME__ << "no field for beam pipe!" << G4endl;}
#endif
}

void BDSMagnet::BuildOuter()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  G4Material* outerMaterial          = magnetOuterInfo->outerMaterial;
  BDSMagnetGeometryType geometryType = magnetOuterInfo->geometryType;
  G4double outerDiameter             = magnetOuterInfo->outerDiameter - lengthSafetyLarge;
  G4double outerLength               = chordLength - 2*lengthSafety;
  
  // chordLength is provided to the outer factory to make a new container for the whole
  // magnet object based on the shape of the magnet outer geometry.
  
  //build the right thing depending on the magnet type
  //saves basically the same function in each derived class
  // RBEND does its own thing by override this method so isn't here
  BDSMagnetOuterFactory* theFactory  = BDSMagnetOuterFactory::Instance();
  switch(magnetType.underlying())
    {
    case BDSMagnetType::decapole:
      outer = theFactory->CreateDecapole(geometryType,name,outerLength,beampipe,
					 outerDiameter,chordLength,outerMaterial);
      break;
    case BDSMagnetType::vkicker:
      outer = theFactory->CreateKicker(geometryType,name,outerLength,beampipe,
				       outerDiameter,chordLength,true,outerMaterial);
      break;
    case BDSMagnetType::hkicker:
      outer = theFactory->CreateKicker(geometryType,name,outerLength,beampipe,
				       outerDiameter,chordLength,false,outerMaterial);
      break;
    case BDSMagnetType::muspoiler:
      outer = theFactory->CreateMuSpoiler(geometryType,name,outerLength,beampipe,
					  outerDiameter,chordLength,outerMaterial);
      break;
    case BDSMagnetType::octupole:
      outer = theFactory->CreateOctupole(geometryType,name,outerLength,beampipe,
					 outerDiameter,chordLength,outerMaterial);
      break;
    case BDSMagnetType::quadrupole:
      outer = theFactory->CreateQuadrupole(geometryType,name,outerLength,beampipe,
					   outerDiameter,chordLength,outerMaterial);
      break;
    case BDSMagnetType::rfcavity:
      outer = theFactory->CreateRfCavity(geometryType,name,outerLength,beampipe,
					 outerDiameter,chordLength,outerMaterial);
      break;
    case BDSMagnetType::sectorbend:
      outer = theFactory->CreateSectorBend(geometryType,name,outerLength,beampipe,
					   outerDiameter,chordLength,angle,outerMaterial);
      break;
    case BDSMagnetType::sextupole:
      outer = theFactory->CreateSextupole(geometryType,name,outerLength,beampipe,
					  outerDiameter,chordLength,outerMaterial);
      break;
    case BDSMagnetType::solenoid:
      outer = theFactory->CreateSolenoid(geometryType,name,outerLength,beampipe,
					 outerDiameter,chordLength,outerMaterial);
      break;
    case BDSMagnetType::multipole:
      outer = theFactory->CreateMultipole(geometryType,name,outerLength,beampipe,
					  outerDiameter,chordLength,outerMaterial);
      break;
    default:
      G4cout << __METHOD_NAME__ << "unknown magnet type - no outer volume built" << G4endl;
      outer = nullptr;
      break;
    }

  if(outer)
    {
      // copy necessary bits out of BDSGeometryComponent that holds
      // container information for whole magnet object provided by
      // magnet outer factory.
      BDSGeometryComponent* container = outer->GetMagnetContainer();
      containerSolid    = container->GetContainerSolid()->Clone();
      G4ThreeVector contOffset = container->GetPlacementOffset();
      // set the main offset of the whole magnet which is placed w.r.t. the
      // zero coordinate of the container solid
      SetPlacementOffset(contOffset);

      RegisterDaughter(outer);
      InheritExtents(container); // update extents
      outer->ClearMagnetContainer();
    }
}

void BDSMagnet::BuildOuterFieldManager(G4int    nPoles,
				       G4double poleField,
				       G4double phiOffset)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  if(nPoles<=0 || nPoles>10 || nPoles%2 !=0)
    G4Exception("BDSMagnet: Invalid number of poles", "-1", FatalException, "");
  if (poleField==0) return;

  itsOuterMagField = new BDSMultipoleOuterMagField(nPoles,poleField,phiOffset);
  itsOuterFieldMgr = new G4FieldManager(itsOuterMagField);

  // these options are always non-zero so always set them
  itsOuterFieldMgr->SetDeltaIntersection(BDSGlobalConstants::Instance()->GetDeltaIntersection());
  itsOuterFieldMgr->SetMinimumEpsilonStep(BDSGlobalConstants::Instance()->GetMinimumEpsilonStep());
  itsOuterFieldMgr->SetMaximumEpsilonStep(BDSGlobalConstants::Instance()->GetMaximumEpsilonStep());
  itsOuterFieldMgr->SetDeltaOneStep(BDSGlobalConstants::Instance()->GetDeltaOneStep());
}

void BDSMagnet::AttachFieldToOuter()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  
  if (outer)
    {
      if (itsOuterFieldMgr)
	{outer->GetContainerLogicalVolume()->SetFieldManager(itsOuterFieldMgr,false);}
      else
	{outer->GetContainerLogicalVolume()->SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);}
    }
}

void BDSMagnet::BuildContainerLogicalVolume()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // note beam pipe is not optional!
  if (outer)
    {//build around that
      // container solid will have been updated in BuildOuter if the outer exists
      containerLogicalVolume = new G4LogicalVolume(containerSolid,
						   emptyMaterial,
						   name + "_container_lv");
      placeBeamPipe = true;
    }
  else
    {
      // use beam pipe container volume as ours and no need to place beam pipe
      containerSolid         = beampipe->GetContainerSolid();
      containerLogicalVolume = beampipe->GetContainerLogicalVolume();
      InheritExtents(beampipe);
      placeBeamPipe = false;
    }

  // now protect the fields inside the container volume by giving the
  // it a null magnetic field (otherwise G4VPlacement can
  // over-ride the already-created fields, by calling 
  // G4LogicalVolume::AddDaughter, which calls 
  // pDaughterLogical->SetFieldManager(fFieldManager, true) - the
  // latter 'true' over-writes all the other fields
  // This shouldn't override the field attached to daughters (vacuum for example) which will
  // retain their field manager if one is already specified.
  containerLogicalVolume->SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false); 
}

void BDSMagnet::PlaceComponents()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  if (placeBeamPipe)
    {
      G4ThreeVector beamPipeOffset = -1*GetPlacementOffset();
      // place beampipe
      G4PVPlacement* beamPipePV = new G4PVPlacement(nullptr,                       // rotation
						    beamPipeOffset,          // position in container
						    beampipe->GetContainerLogicalVolume(),  // its logical volume
						    name + "_beampipe_pv",   // its name
						    containerLogicalVolume,  // its mother  volume
						    false,                   // no boolean operation
						    0,                       // copy number
						    BDSGlobalConstants::Instance()->GetCheckOverlaps());
      
      RegisterPhysicalVolume(beamPipePV);
    }

  if (outer)
    {
      //G4ThreeVector placementOffset = magnetOuterOffset + outer->GetPlacementOffset();
      G4ThreeVector outerOffset = outer->GetPlacementOffset();
      
      // place outer volume
      G4PVPlacement* magnetOuterPV = new G4PVPlacement(nullptr,                      // rotation
						       outerOffset,            // at normally (0,0,0)
						       outer->GetContainerLogicalVolume(), // its logical volume
						       name+"_outer_pv",       // its name
						       containerLogicalVolume, // its mother  volume
						       false,                  // no boolean operation
						       0,                      // copy number
						       BDSGlobalConstants::Instance()->GetCheckOverlaps());

      RegisterPhysicalVolume(magnetOuterPV);
    }
}

BDSMagnet::~BDSMagnet()
{
  delete magnetOuterInfo;
  delete itsBPFieldMgr;
  delete itsChordFinder;
  delete itsMagField;
  delete itsEqRhs;
  delete itsStepper;
}
