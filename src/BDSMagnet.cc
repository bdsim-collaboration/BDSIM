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
  itsType(type),
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
}

void BDSMagnet::Build()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BuildBPFieldAndStepper();
  BuildBPFieldMgr(itsStepper, itsMagField);

  BDSAcceleratorComponent::Build(); // builds the container
  BuildBeampipe();
  BuildOuterVolume();
}

void BDSMagnet::BuildBeampipe()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  
  beampipe = BDSBeamPipeFactory::Instance()->CreateBeamPipe(name,
							    chordLength - lengthSafety,
							    beamPipeInfo);
  BeamPipeCommonTasks();
}

void BDSMagnet::BeamPipeCommonTasks()
{
  //actual beam pipe ->SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);
  // SET FIELD
  if(itsBPFieldMgr)
    {beampipe->GetVacuumLogicalVolume()->SetFieldManager(itsBPFieldMgr,false);}

  // now protect the fields inside the marker volume by giving the
  // marker a null magnetic field (otherwise G4VPlacement can
  // over-ride the already-created fields, by calling 
  // G4LogicalVolume::AddDaughter, which calls 
  // pDaughterLogical->SetFieldManager(fFieldManager, true) - the
  // latter 'true' over-writes all the other fields
  
  containerLogicalVolume->SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);
  
  // register logical & physical volumes  + rotation matrices using geometry component base class
  InheritObjects(beampipe);

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

void BDSMagnet::BuildBPFieldMgr(G4MagIntegratorStepper* aStepper,
				G4MagneticField*        aField)
{
  itsChordFinder = new G4ChordFinder(aField,
				     BDSGlobalConstants::Instance()->GetChordStepMinimum(),
				     aStepper);

  itsChordFinder->SetDeltaChord(BDSGlobalConstants::Instance()->GetDeltaChord());
  itsBPFieldMgr= new G4FieldManager();
  itsBPFieldMgr->SetDetectorField(aField);
  itsBPFieldMgr->SetChordFinder(itsChordFinder);
  if(BDSGlobalConstants::Instance()->GetDeltaIntersection()>0)
    {itsBPFieldMgr->SetDeltaIntersection(BDSGlobalConstants::Instance()->GetDeltaIntersection());}
  if(BDSGlobalConstants::Instance()->GetMinimumEpsilonStep()>0)
    itsBPFieldMgr->SetMinimumEpsilonStep(BDSGlobalConstants::Instance()->GetMinimumEpsilonStep());
  if(BDSGlobalConstants::Instance()->GetMaximumEpsilonStep()>0)
    itsBPFieldMgr->SetMaximumEpsilonStep(BDSGlobalConstants::Instance()->GetMaximumEpsilonStep());
  if(BDSGlobalConstants::Instance()->GetDeltaOneStep()>0)
    itsBPFieldMgr->SetDeltaOneStep(BDSGlobalConstants::Instance()->GetDeltaOneStep());
}

void BDSMagnet::BuildOuterVolume()
{
  G4Material* outerMaterial          = magnetOuterInfo->outerMaterial;
  BDSMagnetGeometryType geometryType = magnetOuterInfo->geometryType;
  G4double outerDiameter             = magnetOuterInfo->outerDiameter - lengthSafetyLarge;
  G4double outerLength               = chordLength - 2*lengthSafety;
  
  //build the right thing depending on the magnet type
  //saves basically the same funciton in each derived class
  BDSMagnetOuterFactory* theFactory = BDSMagnetOuterFactory::Instance();
  switch(itsType.underlying()){
  case BDSMagnetType::decapole:
    outer = theFactory->CreateDecapole(geometryType,name,outerLength,beampipe,
				       outerDiameter,outerMaterial);
    break;
  case BDSMagnetType::vkicker:
    outer = theFactory->CreateKicker(geometryType,name,outerLength,beampipe,
				     outerDiameter,true,outerMaterial);
    break;
  case BDSMagnetType::hkicker:
    outer = theFactory->CreateKicker(geometryType,name,outerLength,beampipe,
				     outerDiameter,false,outerMaterial);
    break;
  case BDSMagnetType::muspoiler:
    outer = theFactory->CreateMuSpoiler(geometryType,name,outerLength,beampipe,
					outerDiameter,outerMaterial);
    break;
  case BDSMagnetType::octupole:
    outer = theFactory->CreateOctupole(geometryType,name,outerLength,beampipe,
				       outerDiameter,outerMaterial);
    break;
  case BDSMagnetType::quadrupole:
    outer = theFactory->CreateQuadrupole(geometryType,name,outerLength,beampipe,
					 outerDiameter,outerMaterial);
    break;
  case BDSMagnetType::rectangularbend:
    outer = theFactory->CreateRectangularBend(geometryType,name,outerLength,beampipe,
					      outerDiameter,angle,outerMaterial);
    break;
  case BDSMagnetType::rfcavity:
    outer = theFactory->CreateRfCavity(geometryType,name,outerLength,beampipe,
				       outerDiameter,outerMaterial);
    break;
  case BDSMagnetType::sectorbend:
    outer = theFactory->CreateSectorBend(geometryType,name,outerLength,beampipe,
					 outerDiameter,angle,outerMaterial);
    break;
  case BDSMagnetType::sextupole:
    outer = theFactory->CreateSextupole(geometryType,name,outerLength,beampipe,
					outerDiameter,outerMaterial);
    break;
  case BDSMagnetType::solenoid:
    outer = theFactory->CreateSolenoid(geometryType,name,outerLength,beampipe,
				       outerDiameter,outerMaterial);
    break;
  case BDSMagnetType::multipole:
    outer = theFactory->CreateMultipole(geometryType,name,outerLength,beampipe,
					outerDiameter,outerMaterial);
    break;
  default:
    G4cout << __METHOD_NAME__ << "unknown magnet type - no outer volume built" << G4endl;
    outer = nullptr;
    break;
  }

  if(outer)
    {
      // register logical volumes using geometry component base class
      InheritObjects(outer);

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
      
      //update extents
      SetExtentX(outer->GetExtentX());
      SetExtentY(outer->GetExtentY());
      SetExtentZ(outer->GetExtentZ());
    }
}

void BDSMagnet::BuildContainerLogicalVolume()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  if (BDS::IsFinite(angle))
    {
      containerSolid = new G4CutTubs(name+"_container_solid", // name
				     0.0,                     // minimum radius - solid here
				     containerRadius,         // radius - determined above
				     chordLength*0.5,         // length about centre point
				     0.0,                     // starting angle
				     2.0*CLHEP::pi,           // sweep angle
				     inputface,               // input face normal vector
				     outputface);             // output face normal vector
    }
  else
    {
      containerSolid = new G4Box(name + "_container_solid",
				 containerRadius,
				 containerRadius,
				 chordLength*0.5);
    }
  
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");

  // taken from FinaliseBeamPipe method - supposed to protect against fields being overridden
  containerLogicalVolume->
    SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);

  SetExtentX(-containerRadius, containerRadius);
  SetExtentY(-containerRadius, containerRadius);
  SetExtentZ(-chordLength*0.5, chordLength*0.5);
}

void BDSMagnet::BuildOuterFieldManager(G4int nPoles, G4double poleField,
					  G4double phiOffset)
{
  if(nPoles<=0 || nPoles>10 || nPoles%2 !=0)
    G4Exception("BDSMagnet: Invalid number of poles", "-1", FatalException, "");
  itsOuterFieldMgr=nullptr;
  if (poleField==0) return;

  itsOuterMagField=new BDSMultipoleOuterMagField(nPoles,poleField,phiOffset);

  itsOuterFieldMgr=new G4FieldManager(itsOuterMagField);
  if(BDSGlobalConstants::Instance()->GetDeltaIntersection()>0){
    itsOuterFieldMgr->SetDeltaIntersection(BDSGlobalConstants::Instance()->GetDeltaIntersection());
  }
  if(BDSGlobalConstants::Instance()->GetMinimumEpsilonStep()>0)
    itsOuterFieldMgr->SetMinimumEpsilonStep(BDSGlobalConstants::Instance()->GetMinimumEpsilonStep());
  if(BDSGlobalConstants::Instance()->GetMaximumEpsilonStep()>0)
    itsOuterFieldMgr->SetMaximumEpsilonStep(BDSGlobalConstants::Instance()->GetMaximumEpsilonStep());
  if(BDSGlobalConstants::Instance()->GetDeltaOneStep()>0)
    itsOuterFieldMgr->SetDeltaOneStep(BDSGlobalConstants::Instance()->GetDeltaOneStep());
  outer->GetContainerLogicalVolume()->SetFieldManager(itsOuterFieldMgr,false);
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
