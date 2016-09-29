#include "BDSAcceleratorModel.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSBeamlinePlacementBuilder.hh"
#include "BDSExtent.hh"
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactory.hh"
#include "BDSParser.hh"
#include "BDSSimpleComponent.hh"
#include "BDSUtilities.hh"

#include "parser/placement.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/EulerAngles.h"

#include "globals.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

#include <vector>


void BDS::BuildPlacementGeometry()
{
  auto placements = BDSParser::Instance()->GetPlacements();

  if (placements.empty())
    {return;} // don't do anything - no placements
  
  BDSBeamline* placementBL = new BDSBeamline();

  for (const auto& placement : placements)
    {
      auto geom = BDSGeometryFactory::Instance()->BuildGeometry(placement.geometryFile);

      G4double length = geom->GetExtent().DZ();
      BDSSimpleComponent* comp = new BDSSimpleComponent(geom->GetName(),
							geom,
							length);

      G4ThreeVector midPos = G4ThreeVector(placement.x*CLHEP::m,
					   placement.y*CLHEP::m,
					   placement.z*CLHEP::m);

      G4RotationMatrix* rm = nullptr;
      if (placement.axisAngle)
	{
	  G4ThreeVector axis = G4ThreeVector(placement.axisX,
					     placement.axisY,
					     placement.axisZ);
	  rm = new G4RotationMatrix(axis, placement.angle*CLHEP::rad);
	}
      else
	{
	  if (BDS::IsFinite(placement.phi)   ||
	      BDS::IsFinite(placement.theta) ||
	      BDS::IsFinite(placement.psi))
	    {// only build if finite
	      CLHEP::HepEulerAngles ang = CLHEP::HepEulerAngles(placement.phi*CLHEP::rad,
								placement.theta*CLHEP::rad,
								placement.psi*CLHEP::rad);
	      rm = new G4RotationMatrix(ang);
	    }
	}
      
      /// Here we're assuming the length is along z which may not be true, but
      /// close enough for this purpose as we rely only on the centre position.
      G4ThreeVector halfLengthBeg = G4ThreeVector(0,0,-length*0.5);
      G4ThreeVector halfLengthEnd = G4ThreeVector(0,0, length*0.5);
      G4ThreeVector startPos = midPos + halfLengthBeg.transform(*rm);
      G4ThreeVector endPos   = midPos + halfLengthEnd.transform(*rm);
      
      BDSBeamlineElement* el = new BDSBeamlineElement(comp,
						      startPos,
						      midPos,
						      endPos,
						      rm,
						      new G4RotationMatrix(*rm),
						      new G4RotationMatrix(*rm),
						      startPos,
						      midPos,
						      endPos,
						      new G4RotationMatrix(*rm),
						      new G4RotationMatrix(*rm),
						      new G4RotationMatrix(*rm),
						      -1,-1,-1);

      placementBL->AddBeamlineElement(el);
    }

  BDSAcceleratorModel::Instance()->RegisterPlacementBeamline(placementBL);
}
