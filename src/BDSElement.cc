#include "BDSAcceleratorComponent.hh"
#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSElement.hh"
#include "BDSGeometryFactory.hh"
#include "BDSGlobalConstants.hh"
#include "BDSFieldFactory.hh"

#include "globals.hh" // geant4 globals / types
#include "G4Box.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

#include <vector>

BDSElement::BDSElement(G4String      name,
		       G4double      length,
		       G4double      outerDiameterIn,
		       G4String      geometry,
		       G4String      bmap,
		       G4ThreeVector bMapOffsetIn):
  BDSAcceleratorComponent(name, length, 0, "element"),
  outerDiameter(outerDiameterIn),
  geometryFileName(geometry),
  bMapFileName(bmap),
  bMapOffset(bMapOffsetIn)
{
  itsFieldVolName="";

  // WARNING: ALign in and out will only apply to the first instance of the
  //          element. Subsequent copies will have no alignment set.
  align_in_volume  = nullptr;
  align_out_volume = nullptr;
}

void BDSElement::Build()
{
  // base class build - builds container volume
  BDSAcceleratorComponent::Build();

  // construct the input geometry and b fields and place in the container
  PlaceComponents();
}

void BDSElement::BuildContainerLogicalVolume()
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
  
  containerSolid = new G4Box(name + "_container_solid",
			     outerDiameter*0.5,
			     outerDiameter*0.5,   
			     chordLength*0.5);
  
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");
  
  // zero field in the marker volume
  containerLogicalVolume->SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);

  SetExtentX(-outerDiameter*0.5, outerDiameter*0.5);
  SetExtentY(-outerDiameter*0.5, outerDiameter*0.5);
  SetExtentZ(-chordLength*0.5, chordLength*0.5);
}

void BDSElement::PlaceComponents()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  
  BDSGeometryFactory* gFact = new BDSGeometryFactory();
  
  geometry = gFact->BuildGeometry(geometryFileName);
  geometry->Construct(containerLogicalVolume);

  RegisterSensitiveVolume(geometry->GetSensitiveComponents());
  
  for(G4int id=0; id<(G4int)geometry->GetGFlashComponents().size(); id++){
    SetGFlashVolumes(geometry->GetGFlashComponents()[id]);
  }
  
  align_in_volume  = geometry->GetAlignInVolume();
  align_out_volume = geometry->GetAlignOutVolume();
  
#ifdef USE_LCDD
  /*
  G4cout << __METHOD_NAME__ << " - setting lcdd vis attributes." << G4endl;
  if(geom->format()->spec()==(G4String)"lcdd") {
    //Make marker visible (temp debug)
    G4VisAttributes* VisAttLCDD = new G4VisAttributes(*BDSColours::Instance()->GetColour("warning"));
    VisAttLCDD->SetForceSolid(true);  
    VisAttLCDD->SetVisibility(false);
    itsMarkerLogicalVolume->SetVisAttributes(VisAttLCDD);
  }
  */
#endif
}

void BDSElement::AlignComponent(G4ThreeVector& TargetPos, 
				G4RotationMatrix *TargetRot, 
				G4RotationMatrix& globalRotation,
				G4ThreeVector& rtot,
				G4ThreeVector& rlast,
				G4ThreeVector& localX,
				G4ThreeVector& localY,
				G4ThreeVector& localZ)
{
  if(!align_in_volume)
    {
      if(!align_out_volume)
	{
	  // advance co-ords in usual way if no alignment volumes found
	  rtot = rlast + localZ*(chordLength/2);
	  rlast = rtot + localZ*(chordLength/2);
	  return;
	}
      else 
	{
#ifdef BDSDEBUG
	  G4cout << "BDSElement : Aligning outgoing to SQL element " 
		 << align_out_volume->GetName() << G4endl;
#endif
	  G4RotationMatrix Trot = *TargetRot;
	  G4RotationMatrix trackedRot;
	  G4RotationMatrix outRot = *(align_out_volume->GetFrameRotation());
	  trackedRot.transform(outRot.inverse());
	  trackedRot.transform(Trot.inverse());
	  globalRotation = trackedRot;

	  G4ThreeVector outPos = align_out_volume->GetFrameTranslation();
	  G4ThreeVector diff = outPos;

	  G4ThreeVector zHalfAngle = G4ThreeVector(0.,0.,1.);

	  zHalfAngle.transform(globalRotation);

	  //moving positioning to outgoing alignment volume
	  rlast = TargetPos - ((outPos.unit()).transform(Trot.inverse()) )*diff.mag();
	  localX.transform(outRot.inverse());
	  localY.transform(outRot.inverse());
	  localZ.transform(outRot.inverse());

	  localX.transform(Trot.inverse());
	  localY.transform(Trot.inverse());
	  localZ.transform(Trot.inverse());

	  //moving position in Z be at least itsLength/2 away
	  rlast +=zHalfAngle*(chordLength/2 + diff.z());
	  return;
	}
    }

  if(align_in_volume)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "Aligning incoming to SQL element " 
      	     << align_in_volume->GetName() << G4endl;
#endif
      
      const G4RotationMatrix* inRot = align_in_volume->GetFrameRotation();
      TargetRot->transform((*inRot).inverse());
      
      G4ThreeVector inPos = align_in_volume->GetFrameTranslation();
      inPos.transform((*TargetRot).inverse());
      TargetPos+=G4ThreeVector(inPos.x(), inPos.y(), 0.0);
      
      if(!align_out_volume)
	{
	  // align outgoing (i.e. next component) to Marker Volume
	  G4RotationMatrix Trot = *TargetRot;
	  globalRotation.transform(Trot.inverse());
	  
	  G4ThreeVector zHalfAngle = G4ThreeVector(0.,0.,1.);
	  zHalfAngle.transform(Trot.inverse());
	  
	  rlast = TargetPos + zHalfAngle*(chordLength/2);
	  localX.transform(Trot.inverse());
	  localY.transform(Trot.inverse());
	  localZ.transform(Trot.inverse());
	  return;
	}
      else
	{
#ifdef BDSDEBUG
	  G4cout << "BDSElement : Aligning outgoing to SQL element " 
		 << align_out_volume->GetName() << G4endl;
#endif
	  G4RotationMatrix Trot = *TargetRot;
	  G4RotationMatrix trackedRot;
	  G4RotationMatrix outRot = *(align_out_volume->GetFrameRotation());
	  trackedRot.transform(outRot.inverse());
	  trackedRot.transform(Trot.inverse());
	  globalRotation = trackedRot;

	  G4ThreeVector outPos = align_out_volume->GetFrameTranslation();
	  G4ThreeVector diff = outPos;

	  G4ThreeVector zHalfAngle = G4ThreeVector(0.,0.,1.);

	  zHalfAngle.transform(globalRotation);

	  //moving positioning to outgoing alignment volume
	  rlast = TargetPos - ((outPos.unit()).transform(Trot.inverse()) )*diff.mag();
	  localX.transform(outRot.inverse());
	  localY.transform(outRot.inverse());
	  localZ.transform(outRot.inverse());

	  localX.transform(Trot.inverse());
	  localY.transform(Trot.inverse());
	  localZ.transform(Trot.inverse());

	  //moving position in Z be at least itsLength/2 away
	  rlast +=zHalfAngle*(chordLength/2 + diff.z());
	  return;
	}
    }
  
}

BDSElement::~BDSElement()
{;}
