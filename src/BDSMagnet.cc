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
#include "BDSUtilities.hh"

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
  itsK1 = 0.0; itsK2 = 0.0; itsK3 = 0.0;
  
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
  BuildBeampipe();            // build beam pipe without placing it
  BuildBPFieldAndStepper();   // build magnetic field
  InheritObjects(beampipe);   // inherit all beam pipe stuff
  AttachFieldToBeamPipe();    // attach the magnetic field to the vacuum
  BuildOuter();               // build outer and update container solid
  if (outer)
    {
      InheritObjects(outer);  // inherit all outer stuff
      // it would make sense to build the field here, but this is actually
      // called in each derived class.
      // each class works out the flux at the poles ignore the size of this magnet
      // and based on global constants (1 size for all) and then calls buildouterfieldmanager
      // this is all done by overriding this method - Build() which is perhaps unnecessary
      // and should be changed
      //BuildOuterFieldManager(..) // unused currently - uncomment when reimplemented!!!
      //AttachFieldToOuter();
    }  
  
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
}

void BDSMagnet::BuildBPFieldMgr(G4MagIntegratorStepper* aStepper,
				G4MagneticField*        aField)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  itsChordFinder = new G4ChordFinder(aField,
				     BDSGlobalConstants::Instance()->GetChordStepMinimum(),
				     aStepper);

  itsChordFinder->SetDeltaChord(BDSGlobalConstants::Instance()->GetDeltaChord());
  itsBPFieldMgr = new G4FieldManager();
  itsBPFieldMgr->SetDetectorField(aField);
  itsBPFieldMgr->SetChordFinder(itsChordFinder);

  // these options are always non-zero so always set them
  itsBPFieldMgr->SetDeltaIntersection(BDSGlobalConstants::Instance()->GetDeltaIntersection());
  itsBPFieldMgr->SetMinimumEpsilonStep(BDSGlobalConstants::Instance()->GetMinimumEpsilonStep());
  itsBPFieldMgr->SetMaximumEpsilonStep(BDSGlobalConstants::Instance()->GetMaximumEpsilonStep());
  itsBPFieldMgr->SetDeltaOneStep(BDSGlobalConstants::Instance()->GetDeltaOneStep());
}

void BDSMagnet::AttachFieldToBeamPipe()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // SET FIELD
  if(itsBPFieldMgr)
    {beampipe->GetVacuumLogicalVolume()->SetFieldManager(itsBPFieldMgr,false);}
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
  //saves basically the same funciton in each derived class
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
    case BDSMagnetType::rectangularbend:
      outer = theFactory->CreateRectangularBend(geometryType,name,outerLength,beampipe,
						outerDiameter,chordLength,angle,outerMaterial);
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
      containerSolid = outer->GetMagnetContainer()->GetContainerSolid()->Clone();
      InheritExtents(outer); //update extents
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
  outer->GetContainerLogicalVolume()->SetFieldManager(itsOuterFieldMgr,false);
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

      // taken from FinaliseBeamPipe method - supposed to protect against fields being overridden
      containerLogicalVolume->SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);
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
      // place beampipe
      G4PVPlacement* beamPipePV = new G4PVPlacement(0,                         // rotation
						    (G4ThreeVector)0,          // at (0,0,0)
						    beampipe->GetContainerLogicalVolume(),  // its logical volume
						    name + "_beampipe_pv",     // its name
						    containerLogicalVolume,    // its mother  volume
						    false,                     // no boolean operation
						    0,                         // copy number
						    BDSGlobalConstants::Instance()->GetCheckOverlaps());
      
      RegisterPhysicalVolume(beamPipePV);
    }

  if (outer)
    {
      G4ThreeVector placementOffset = magnetOuterOffset + outer->GetPlacementOffset();
      
      // place outer volume
      G4PVPlacement* magnetOuterPV = new G4PVPlacement(0,                           // rotation
						       placementOffset,             // at normally (0,0,0)
						       outer->GetContainerLogicalVolume(), // its logical volume
						       name+"_outer_pv",            // its name
						       containerLogicalVolume,      // its mother  volume
						       false,                       // no boolean operation
						       0,                           // copy number
						       BDSGlobalConstants::Instance()->GetCheckOverlaps());

      RegisterPhysicalVolume(magnetOuterPV);
    }
}

BDSMagnet::~BDSMagnet()
{
  // safe to delete even if haven't been built as initialised to nullptr
  delete magnetOuterInfo; magnetOuterInfo = nullptr;
  delete itsBPFieldMgr;   itsBPFieldMgr   = nullptr;
  delete itsChordFinder;  itsChordFinder  = nullptr;
  delete itsMagField;     itsMagField     = nullptr;
  delete itsEqRhs;        itsEqRhs        = nullptr;
  delete itsStepper;      itsStepper      = nullptr;
}
