#include "BDSAcceleratorComponent.hh"
#include "BDSDebug.hh"
#include "BDSExtent.hh"
#include "BDSExecOptions.hh"
#include "BDSElement.hh"
#include "BDSGeometryExternal.hh"
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
  bMapOffset(bMapOffsetIn),
  itsFieldVolName(""),
  align_in_volume(nullptr),
  align_out_volume(nullptr)
{
  // WARNING: ALign in and out will only apply to the first instance of the
  //          element. Subsequent copies will have no alignment set.
}

void BDSElement::BuildContainerLogicalVolume()
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ <<G4endl;
#endif

  BDSGeometryExternal* geom = BDSGeometryFactory::Instance()->BuildGeometry(geometryFileName);
  
  if (!geom)
    {G4cerr << __METHOD_NAME__ << "Error loading geometry" << G4endl; exit(1);}
  
  // We don't registe the geometry as a daughter as the geometry factory retains
  // ownership of the geometry and will clean it up at the end.
  
  // make the beam pipe container, this object's container
  containerLogicalVolume = geom->GetContainerLogicalVolume();
  containerSolid         = geom->GetContainerSolid();
  
  // update extents
  InheritExtents(geom);

  const BDSExtent geomExtent = geom->GetExtent();
  BDSExtent nominalExt = BDSExtent(outerDiameter*0.5, outerDiameter*0.5, chordLength*0.5);
  if (nominalExt.TransverselyGreaterThan(geomExtent))
    {SetExtent(nominalExt);}
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
