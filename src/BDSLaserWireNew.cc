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
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSLaser.hh"
#include "BDSLaserWireNew.hh"
#include "BDSLogicalVolumeLaser.hh"
#include "BDSMaterials.hh"
#include "BDSUtilities.hh"
#include "BDSWireScanner.hh"

#include "globals.hh"
#include "G4Hype.hh"
#include "G4Tubs.hh"
#include "G4IntersectionSolid.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"
#include "G4VisAttributes.hh"
#include "BDSException.hh"

BDSLaserWireNew::BDSLaserWireNew(G4String         nameIn,
				 G4double         lengthIn,
				 BDSBeamPipeInfo* beamPipeInfoIn,
				 BDSLaser*        laserIn,
				 G4double         wireDiameterIn,
				 G4double         wireLengthIn,
				 G4double         wireAngleIn,
				 G4double         wireLongitudinalAngleIn,
				 G4ThreeVector    wireOffsetIn,
				 G4Colour*        wireColourIn):
  BDSAcceleratorComponent(nameIn, lengthIn,0, "laserwire", beamPipeInfoIn),
  laser(laserIn),
  wireDiameter(wireDiameterIn),
  wireLength(wireLengthIn),
  wireAngle(wireAngleIn),
  wireLongitudinalAngle(wireLongitudinalAngleIn),
  wireOffset(wireOffsetIn),
  wireColour(wireColourIn)

{
  wireMaterial = BDSMaterials::Instance()->GetMaterial("LaserVac");

  if (wireDiameter <= 0)
    {
        throw BDSException(__METHOD_NAME__,"W0 or Sigma0 in element " + name +" must be greater than 0.");
    }
  
  if (wireLength <= 0)
    {
        throw BDSException(__METHOD_NAME__,"wireLength in element " + name +" must be greater than 0.");
    }

  if (wireLongitudinalAngle > CLHEP::halfpi && wireLongitudinalAngle < -1.0*CLHEP::halfpi)
  {
      throw BDSException(__METHOD_NAME__,"laserOffsetPhi in element " + name +" must be between -pi/2 and pi/2.");
  }

  if(wireAngle < CLHEP::pi && wireAngle < -1.0*CLHEP::pi)
  {
      throw BDSException(__METHOD_NAME__,"laserOffsetTheta in element " + name +" must be between 0 and two pi.");
  }
  // check whether the beam pipe will fit transversely (ignores presumably very small
  // wire diameter). work out end points off wire including length and offset in x,y.
  G4TwoVector offsetXY = G4TwoVector(wireOffset.x(), wireOffset.y());
  G4TwoVector tipTop = G4TwoVector(0, 0.5*wireLength);
  tipTop.rotate(wireAngle);
  G4TwoVector tipBot = G4TwoVector(tipTop);
  tipBot.rotate(CLHEP::pi);
  tipTop += offsetXY;
  tipBot += offsetXY;
  //G4double innerRadius = beamPipeInfo->IndicativeRadiusInner();
  /*if (tipTop.mag() > innerRadius || tipBot.mag() > innerRadius)
    {
    G4cerr << __METHOD_NAME__ << "Error: wire for \"" << name
    << "\" is too big to fit in beam pipe give offsets." << G4endl;
    exit(1);
    }
  */
  if (!wireColour)
    {wireColour = BDSColours::Instance()->GetColour("wirescanner");}
}

BDSLaserWireNew::~BDSLaserWireNew()
{
  delete laser;
}

void BDSLaserWireNew::BuildContainerLogicalVolume()
{
  BDSBeamPipeFactory *factory = BDSBeamPipeFactory::Instance();
  BDSBeamPipe *pipe = factory->CreateBeamPipe(name + "_beampipe",
					      chordLength,
					      beamPipeInfo);
  RegisterDaughter(pipe);
  
  // make the beam pipe container, this object's container
  containerLogicalVolume = pipe->GetContainerLogicalVolume();
  containerSolid = pipe->GetContainerSolid();
  
  // register vacuum volume (for biasing)
  SetAcceleratorVacuumLogicalVolume(pipe->GetVacuumLogicalVolume());
  
  // update extents
  InheritExtents(pipe);
  
  // update faces
  SetInputFaceNormal(pipe->InputFaceNormal());
  SetOutputFaceNormal(pipe->OutputFaceNormal());
}
/*
void BDSLaserWireNew::Build()
{
  BDSAcceleratorComponent::Build();

  G4VSolid* wire = BuildHyperbolicWireSolid();
  G4LogicalVolume* wireLV = BuildWireLV(wire);
  
  // visualisation attributes
  
  
  G4VisAttributes* wireVisAttr = new G4VisAttributes(*wireColour);
  wireLV->SetVisAttributes(wireVisAttr);
  RegisterVisAttributes(wireVisAttr);
  // placement

  G4RotationMatrix* placementWireRot = new G4RotationMatrix();
  placementWireRot->rotateX(wireLongitudinalAngle);
  // want to rotate about unit Z but this has now changed
  placementWireRot->rotateY(wireAngle);
  placementWireRot->rotateZ(0);

  G4PVPlacement* wirePV = new G4PVPlacement(placementWireRot,           // rotation
					    wireOffset,        // position
					    wireLV,            // its logical volume
					    name + "_wire_pv", // its name
					    GetAcceleratorVacuumLogicalVolume(),
					    false,                  // no boolean operation
					    0,                      // copy number
					    checkOverlaps);

  
  RegisterPhysicalVolume(wirePV);  
}
*/
G4VSolid* BDSLaserWireNew::BuildHyperbolicWireSolid()
{
  G4Hype* laserwire;
  laserwire = new G4Hype(name + "_laserwire_solid", //name
			 0,                  // inner radius
			 wireDiameter*0.5,   // outer radius
			 0,                  // inner stereo
			 laser->HyperbolicAngle(),     // outer stereo
			 wireLength*0.5);
  RegisterSolid(laserwire);

  G4ThreeVector unitLaserZ;
  unitLaserZ.set(0,0,1);


  // placement rotation
  G4RotationMatrix* wireRot = new G4RotationMatrix();
  // want to rotate about unit Z but this has now changed
  wireRot->rotateX(wireLongitudinalAngle);
  wireRot->rotateY(wireAngle);
  wireRot->rotateZ(0);
  wireRot->invert();
  RegisterRotationMatrix(wireRot);
  wireColour->SetAlpha(0.5);


  auto bpf =  BDSBeamPipeFactory::Instance();
  BDSBeamPipe* intersectionBP = bpf->CreateBeamPipeForVacuumIntersection(name + "_vacuum_intersection",
									 chordLength,
									 GetBeamPipeInfo());
  G4VSolid* vacuumSolid = intersectionBP->GetContainerSolid();
  // do intersection with vacuumSolid
  RegisterSolid(vacuumSolid);
  
  G4VSolid* lasersolid = new G4IntersectionSolid(name + "_laserwire_solid",laserwire,vacuumSolid,wireRot,wireOffset);
  
  RegisterSolid(lasersolid);
  
  return lasersolid;
}


G4LogicalVolume* BDSLaserWireNew::BuildWireLV(G4VSolid* solid)
{
  BDSLogicalVolumeLaser* wireLV = new BDSLogicalVolumeLaser(solid,
							    wireMaterial,
							    name + "_laserwire_lv",
							    laser);
  RegisterLogicalVolume(wireLV);
  return wireLV;
}

void BDSLaserWireNew::Build(){

    BDSAcceleratorComponent::Build();

    G4Tubs* laserMaster;
    laserMaster = new G4Tubs(name+"_laserwire_solid_Master", //name
                           0,                       //inner radius
                           wireDiameter*0.5,
                           wireLength*0.5,
                           0,            //closing angle
                           CLHEP::twopi);           //wire half length

    RegisterSolid(laserMaster);

    G4LogicalVolume* laserMasterLV = BuildWireLV(laserMaster);
    RegisterLogicalVolume(laserMasterLV);
    G4LogicalVolume* currentLV = laserMasterLV;
    wireColour->SetAlpha(0.05);

    G4VisAttributes* wireVisAttr = new G4VisAttributes(*wireColour);
    laserMasterLV->SetVisAttributes(wireVisAttr);
    RegisterVisAttributes(wireVisAttr);
    // placement


    G4int nVol = 50;
    G4double stepSize = wireDiameter/(nVol*2); //is twice step size
    G4double lengthReduction = wireLength/(nVol*4);

    G4ThreeVector place;
    place.set(0,0,0);

    for(G4int i = 1; i <= nVol-1; i++)
    {

        G4Tubs* laserLayer;
        laserLayer = new G4Tubs(name+"_laserwire_solid_"+std::to_string(i),
                                0,
                                (wireDiameter*0.5)-i*stepSize,
                                (wireLength*0.5)-i*lengthReduction,
                                0,
                                CLHEP::twopi);

        RegisterSolid(laserLayer);

        G4LogicalVolume* laserLayerLV = BuildWireLV(laserLayer);
        RegisterLogicalVolume(laserLayerLV);
        wireColour->SetAlpha(0.05);

        laserLayerLV->SetVisAttributes(wireVisAttr);
        RegisterVisAttributes(wireVisAttr);

        G4PVPlacement* laserLayerPV = new G4PVPlacement(0,
                                                        place,
                                                        laserLayerLV,
                                                        name + "_wire_pv_"+std::to_string(i),
                                                        currentLV,
                                                        false,
                                                        i,
                                                        checkOverlaps);
        RegisterPhysicalVolume(laserLayerPV);


        currentLV = laserLayerLV;
    }

    G4RotationMatrix* placementWireRot = new G4RotationMatrix();
    placementWireRot->rotateX(wireLongitudinalAngle);
    // want to rotate about unit Z but this has now changed
    placementWireRot->rotateY(wireAngle);
    placementWireRot->rotateZ(0);

    G4PVPlacement* laserMasterPV = new G4PVPlacement(placementWireRot,           // rotation
                                              wireOffset,        // position
                                              laserMasterLV,            // its logical volume
                                              name + "_wire_pv", // its name
                                              GetAcceleratorVacuumLogicalVolume(),
                                              false,                  // no boolean operation
                                              0,                      // copy number
                                              checkOverlaps);


    RegisterPhysicalVolume(laserMasterPV);


}

/*
{
  G4LogicalVolume* masterBox = new G4LogicalVolume();
  G4LogicalVolume* currentVolume = masterBox;

  for (G4int i =  0 : n)
{
    new G4Box();
    G4LogicalVolume* lv = new G4LogicalVolume(box);
    G4PVPlacement(name, lv, currentVolume);
    currentVolume = lv;

}

}G4PVPlacement* wirePV = new G4PVPlacement(placementWireRot,           // rotation
                                           wireOffset,        // position
                                           wireLV,            // its logical volume
                                           name + "_wire_pv", // its name
                                           GetAcceleratorVacuumLogicalVolume(),
                                           false,                  // no boolean operation
                                           0,                      // copy number
                                           checkOverlaps);
*/