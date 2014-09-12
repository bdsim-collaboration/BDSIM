/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 

   Modified 22.03.05 by J.C.Carter, Royal Holloway, Univ. of London.
   Changed StringFromInt to be the BDSGlobal version
*/

#include "BDSGlobalConstants.hh" 
#include "BDSDebug.hh"

#include "BDSSextupole.hh"

#include "BDSMaterials.hh"
#include "BDSSextMagField.hh"
#include "BDSSextStepper.hh"

#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4Polyhedra.hh"
#include "G4PVPlacement.hh"               
#include "G4UserLimits.hh"
#include "G4TransportationManager.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

//============================================================

BDSSextupole::BDSSextupole(G4String aName, G4double aLength, 
			   G4double bpRad, G4double FeRad,
			   G4double BDblPrime, G4double tilt, 
			   G4double outR, 
                           std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
                           G4String aTunnelMaterial, G4String aMaterial):
  BDSMultipole(aName, aLength, bpRad, FeRad, blmLocZ, blmLocTheta, aTunnelMaterial, aMaterial),
  itsBDblPrime(BDblPrime)
{
  SetOuterRadius(outR);
  itsTilt=tilt;
}

void BDSSextupole::Build()
{
  BDSMultipole::Build();
  if(BDSGlobalConstants::Instance()->GetIncludeIronMagFields())
    {
      G4double polePos[4];
      G4double Bfield[3];
      
      //coordinate in GetFieldValue
      polePos[0]=-BDSGlobalConstants::Instance()->GetMagnetPoleRadius()*sin(CLHEP::pi/6);
      polePos[1]=BDSGlobalConstants::Instance()->GetMagnetPoleRadius()*cos(CLHEP::pi/6);
      polePos[2]=0.;
      polePos[3]=-999.;//flag to use polePos rather than local track
      
      itsMagField->GetFieldValue(polePos,Bfield);
      G4double BFldIron=
	sqrt(Bfield[0]*Bfield[0]+Bfield[1]*Bfield[1])*
	BDSGlobalConstants::Instance()->GetMagnetPoleSize()/
	(BDSGlobalConstants::Instance()->GetComponentBoxSize()/2-
	 BDSGlobalConstants::Instance()->GetMagnetPoleRadius());
      
      // Magnetic flux from a pole is divided in two directions
      BFldIron/=2.;
      
      BuildOuterFieldManager(6, BFldIron,CLHEP::pi/6);
    }
}

void BDSSextupole::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(1,1,0));
  itsVisAttributes->SetForceSolid(true);
}

void BDSSextupole::BuildOuterLogicalVolume(G4bool /*OuterMaterialIsVacuum*/)
{
  // build magnet (geometry + magnetic field)
  // according to quad type
  
  G4String geometry = BDSGlobalConstants::Instance()->GetMagnetGeometry();
  
  if(geometry =="standard") 
    BuildStandardOuterLogicalVolume(); // standard - quad with poles and pockets
  else if(geometry =="cylinder")  
    BuildCylindricalOuterLogicalVolume(); // cylinder outer volume
  else //default - cylinder - standard
    BuildCylindricalOuterLogicalVolume(); // cylinder outer volume

  //
  // define sensitive volumes for hit generation
  //
  if(BDSGlobalConstants::Instance()->GetSensitiveComponents()){
    AddSensitiveVolume(itsOuterLogicalVolume);
  }
}

//////////////////////////////////////////////////////////////////////////////////////////
//				Cylindrical geometry					//
//////////////////////////////////////////////////////////////////////////////////////////

void BDSSextupole::BuildCylindricalOuterLogicalVolume()
{
 
  G4double outerRadius = itsOuterR;
  if(itsOuterR==0) outerRadius = BDSGlobalConstants::Instance()->GetComponentBoxSize()/2;

#ifdef DEBUG 
  G4cout << __METHOD_NAME__ << "Outer volume inner radius :"
         << " r= " << (itsInnerIronRadius)/CLHEP::m << " m"
         << " l= " << aLength/2./CLHEP::m << " m"
         << G4endl;
#endif

#ifdef DEBUG 
  G4cout << __METHOD_NAME__ << "Outer radius :"
         << " r= " << outerRadius/CLHEP::m << " m"
         << " l= " << aLength/2./CLHEP::m << " m"
         << G4endl;
#endif
  
   itsOuterLogicalVolume=
     new G4LogicalVolume(
			new G4Tubs(itsName+"_outer_solid",
				   itsInnerIronRadius,
				   outerRadius,
				   itsLength/2,
				   0,CLHEP::twopi*CLHEP::radian),

			
			//BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			BDSMaterials::Instance()->GetMaterial("Iron"),
			itsName+"_outer");


    // color-coding for the pole
  G4VisAttributes* VisAtt = 
    new G4VisAttributes(G4Colour(1.,1.,0.));
  VisAtt->SetForceSolid(true);


  
  itsPhysiComp = new G4PVPlacement(
				   (G4RotationMatrix*)0,		      // no rotation
				   (G4ThreeVector)0,                      // its position
		      itsOuterLogicalVolume,  // its logical volume
		      itsName+"_outer_phys",  // its name
		      itsMarkerLogicalVolume, // its mother  volume
		      false,		      // no boolean operation
				   0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		      // copy number
  
  //Add the physical volumes to a vector which can be used for e.g. geometrical biasing
  SetMultiplePhysicalVolumes(itsPhysiComp);

  //
  // set visualization attributes
  //
  itsOuterLogicalVolume->SetVisAttributes(itsVisAttributes);

#ifndef NOUSERLIMITS
  G4double maxStepFactor=0.5;
  itsOuterUserLimits =  new G4UserLimits("multipole cut");
  itsOuterUserLimits->SetUserMinEkine( BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  itsOuterUserLimits->SetMaxAllowedStep(itsLength*maxStepFactor);
  itsOuterLogicalVolume->SetUserLimits(itsOuterUserLimits);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////
//				Detailed geometry					//
//////////////////////////////////////////////////////////////////////////////////////////

void BDSSextupole::BuildStandardOuterLogicalVolume()
{
 
  G4double outerRadius = itsOuterR;
  if(itsOuterR==0) outerRadius = BDSGlobalConstants::Instance()->GetComponentBoxSize()/2;

#ifdef DEBUG 
  G4cout << __METHOD_NAME__ << "Outer volume inner radius :"
         << " r= " << (itsInnerIronRadius)/CLHEP::m << " m"
         << " l= " << aLength/2./CLHEP::m << " m"
         << G4endl;
#endif

#ifdef DEBUG 
  G4cout << __METHOD_NAME__ << "Outer radius :"
         << " r= " << outerRadius/CLHEP::m << " m"
         << " l= " << aLength/2./CLHEP::m << " m"
         << G4endl;
#endif

 
  
  G4int n_poles = 6; // number of poles
  double mag_inradius = 250*CLHEP::mm; // inner radius

  double zplanepos [2] = {0,itsLength};  

  double rinner [2] = {mag_inradius, mag_inradius};
  
  G4double router [2] = {outerRadius ,outerRadius };

  double pole_inradius = itsInnerIronRadius;
  double pole_extradius = mag_inradius+0.05*CLHEP::m;

itsOuterLogicalVolume=
   new G4LogicalVolume(

			new G4Polyhedra(itsName+"_outer_solid", 
					0.*CLHEP::degree, 
					360.*CLHEP::degree, 
					2*n_poles, 
					2, 
					zplanepos, 
					rinner, 
					router),
			
			//BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			BDSMaterials::Instance()->GetMaterial("Iron"),
			itsName+"_outer");


  // Defining poles
  G4ThreeVector positionQuad = G4ThreeVector(0,0,0);
  G4Tubs* poleS
    = new G4Tubs("pole",
		 pole_inradius,
		 pole_extradius,
		 itsLength/2.0,
		 0.,
		 180.0/n_poles*CLHEP::deg);
  
  G4LogicalVolume* PoleSLV = 
    new G4LogicalVolume(poleS,             //its solid
                        BDSMaterials::Instance()->GetMaterial("Iron"),   //its material
                        "PoleSLV");        //its name
  
  
  for (G4int n = 0; n < n_poles; n++) {

    // Calculate position with respect to the reference frame 
    // of the mother volume
    G4RotationMatrix* rm = new G4RotationMatrix();
    rm->rotateZ((n+0.5)*360.0/n_poles*CLHEP::degree-itsTilt*180.0/CLHEP::pi*CLHEP::degree);
    G4ThreeVector uz = G4ThreeVector(0.,0.,itsLength/2.0);     
    G4ThreeVector position = uz;
    //G4Transform3D transform = G4Transform3D(rm,position);

    // Place the poles with the appropriate transformation
   
    new G4PVPlacement(rm,             //rotation,
		      uz,             //position
                      PoleSLV,            //its logical volume
                      "poleS",               //its name
                      itsOuterLogicalVolume,             //its mother  volume
                      false,                 //no boolean operation
                      n,                     //copy number
                      BDSGlobalConstants::Instance()->GetCheckOverlaps());       // checking overlaps 
	
  }

  // color-coding for the pole
  G4VisAttributes* VisAtt = 
    new G4VisAttributes(G4Colour(1.,1.,0.));
  VisAtt->SetForceSolid(true);
  PoleSLV->SetVisAttributes(VisAtt);

  G4RotationMatrix* rm2 = new G4RotationMatrix();
  rm2->rotateZ(360.0/n_poles/4.0*CLHEP::degree-itsTilt*180.0/CLHEP::pi*CLHEP::degree);
  
  itsPhysiComp = new G4PVPlacement(
				   //(G4RotationMatrix*)0,		      // no rotation
				   rm2,
				   (G4ThreeVector)0,                      // its position
		      itsOuterLogicalVolume,  // its logical volume
		      itsName+"_outer_phys",  // its name
		      itsMarkerLogicalVolume, // its mother  volume
		      false,		      // no boolean operation
				   0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		      // copy number
  
  //Add the physical volumes to a vector which can be used for e.g. geometrical biasing
  SetMultiplePhysicalVolumes(itsPhysiComp);

#ifndef NOUSERLIMITS
  G4double maxStepFactor=0.5;
  itsOuterUserLimits =  new G4UserLimits("multipole cut");
  itsOuterUserLimits->SetUserMinEkine( BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  itsOuterUserLimits->SetMaxAllowedStep(itsLength*maxStepFactor);
  itsOuterLogicalVolume->SetUserLimits(itsOuterUserLimits);
#endif
 

}

//////////////////////////////////////////////////////////////////////////////////////////


void BDSSextupole::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper
  itsMagField=new BDSSextMagField(1*itsBDblPrime); //L Deacon testing field sign 4/7/12
  itsEqRhs=new G4Mag_UsualEqRhs(itsMagField);

  itsStepper=new BDSSextStepper(itsEqRhs);
  BDSSextStepper* sextStepper = dynamic_cast<BDSSextStepper*>(itsStepper);
  sextStepper->SetBDblPrime(itsBDblPrime);
}

BDSSextupole::~BDSSextupole()
{
}
