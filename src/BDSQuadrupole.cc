//  
//   BDSIM, (C) 2001-2006 
//   
//   version 0.3
//  
//
//
//   Quadrupole class
//
//   History
//
//     21 Nov 2006 by Agapov,  v.0.3
//     22 Mar 2005 by Agapov, Carter,  v.0.2
//     x  x   2002 by Blair
//
//

#include "BDSGlobalConstants.hh" 
#include "BDSDebug.hh"

#include "BDSQuadrupole.hh"

#include "BDSBeamPipeInfo.hh"
#include "BDSMaterials.hh"
#include "BDSQuadMagField.hh"
#include "BDSQuadStepper.hh"
#include "BDSEnergyCounterSD.hh"
#include "BDSSDManager.hh"

#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4Polyhedra.hh"
#include "G4PVPlacement.hh"               
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

#include "BDSMagnetOuterFactory.hh"
#include "BDSMagnetType.hh"

BDSQuadrupole::BDSQuadrupole(G4String aName, G4double aLength, 
			     G4double bpRad, G4double FeRad,
			     G4double bGrad, G4double tilt, G4double outR,
                             std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
			     G4String aTunnelMaterial, G4String aMaterial, G4String spec):
  BDSMultipole(aName, aLength, bpRad, FeRad, blmLocZ, blmLocTheta, aTunnelMaterial, aMaterial),
  itsBGrad(bGrad)
{
#ifdef BDSDEBUG 
  G4cout<< __METHOD_NAME__ << "spec=" << spec << G4endl;
#endif
  // get specific quadrupole type
  G4String qtype = getParameterValueString(spec, "type");
#ifdef BDSDEBUG 
  G4cout<< __METHOD_NAME__ << "qtype="<<qtype<<G4endl;
#endif

  SetOuterRadius(outR);
  itsTilt=tilt;
}

BDSQuadrupole::BDSQuadrupole(G4String      name,
			     G4double      length,
			     G4double      bGrad,
			     beamPipeInfo  beamPipeInfoIn,
			     G4double      boxSize,
			     G4String      outerMaterial,
			     G4String      tunnelMaterial,
			     G4double      tunnelRadius,
			     G4double      tunnelOffsetX):
  BDSMultipole(name,length,beamPipeInfoIn,boxSize,outerMaterial,tunnelMaterial,tunnelRadius,tunnelOffsetX),
  itsBGrad(bGrad)
{
  G4String qtype = "cylinder";
}

void BDSQuadrupole::Build() 
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMultipole::Build();
  
  if(BDSGlobalConstants::Instance()->GetIncludeIronMagFields())
    {
      G4double polePos[4];
      G4double Bfield[3];
      
      //coordinate in GetFieldValue
      polePos[0]=-BDSGlobalConstants::Instance()->GetMagnetPoleRadius()*sin(CLHEP::pi/4);
      polePos[1]=BDSGlobalConstants::Instance()->GetMagnetPoleRadius()*cos(CLHEP::pi/4);
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
      
      BuildOuterFieldManager(4, BFldIron,CLHEP::pi/4);
    }
}

void BDSQuadrupole::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper
  itsMagField=new BDSQuadMagField(1*itsBGrad); //L Deacon checking sign of field 4/7/12
  itsEqRhs=new G4Mag_UsualEqRhs(itsMagField);

  BDSQuadStepper* quadStepper=new BDSQuadStepper(itsEqRhs);
  quadStepper->SetBGrad(itsBGrad);
  itsStepper = quadStepper;
}

void BDSQuadrupole::BuildOuterLogicalVolume(G4bool /*OuterMaterialIsVacuum*/)
{

  BDSGeometryComponent* outerLV = BDSMagnetOuterFactory::Instance()->CreateQuadrupole(BDSMagnetType::cylindrical,
										      itsName,
										      itsLength,
										      beampipe,
										      boxSize,
										      BDSMaterials::Instance()->GetMaterial("Iron"));

  G4cout << "NAME " << outerLV->GetContainerSolid()->GetName() << G4endl;
										      
  // build magnet (geometry + magnetic field)
  // according to quad type
  G4String geometry = BDSGlobalConstants::Instance()->GetMagnetGeometry();
  
  if(geometry =="standard") 
    BuildStandardOuterLogicalVolume(); // standard - quad with poles and pockets
  else if(geometry =="cylinder")
    BDSMultipole::BuildOuterLogicalVolume(false);
  else //default - cylinder - standard
    BDSMultipole::BuildOuterLogicalVolume(false);

  //remember if it's vacuum, it won't be built - have to check it's there
  if (itsOuterLogicalVolume)
    {itsOuterLogicalVolume->SetSensitiveDetector(BDSSDManager::Instance()->GetEnergyCounterOnAxisSD());}
}

void BDSQuadrupole::SetVisAttributes()
{
  itsVisAttributes = new G4VisAttributes(true);
  itsVisAttributes->SetColor(1,0,0);
  itsVisAttributes->SetForceSolid(true);
  itsVisAttributes->SetVisibility(true);
}


void BDSQuadrupole::BuildStandardOuterLogicalVolume()
{
  G4double outerRadius = itsOuterR;
  if(itsOuterR==0) outerRadius = BDSGlobalConstants::Instance()->GetComponentBoxSize()/2;

  outerRadius = outerRadius/sqrt(2.0);

  G4int n_poles = 4; // number of poles
  double mag_inradius = outerRadius/2.0 * sqrt(2.0); // inner radius

  double zplanepos [2] = {0,itsLength};
  double pole_extra_length = 0.05*CLHEP::m;

  double rinner [2] = {mag_inradius, mag_inradius};
  //G4double rinner [2] = {itsInnerIronRadius,itsInnerIronRadius};
  G4double router [2] = {outerRadius * sqrt(2.0),outerRadius * sqrt(2.0)};

  double pole_inradius = itsInnerIronRadius;
  double pole_extradius = mag_inradius+pole_extra_length;
  //double itstilt = 0;

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
    new G4VisAttributes(G4Colour(1., 0., 0.));
  VisAtt->SetForceSolid(true);
  PoleSLV->SetVisAttributes(VisAtt);
    
  G4RotationMatrix* rm_outer = new G4RotationMatrix();
  rm_outer->rotateZ(360.0/n_poles/4.0*CLHEP::deg-itsTilt*180.0/CLHEP::pi*CLHEP::degree);
  G4ThreeVector uz = G4ThreeVector(0.,0.,-itsLength/2.0); 
  // insert the outer volume into the marker volume
  itsPhysiComp = 
    new G4PVPlacement(
		      rm_outer,                      // no rotation
		      //(G4ThreeVector)0,                      // its position
		      uz,
		      itsOuterLogicalVolume,  // its logical volume
		      itsName+"_outer_phys",  // its name
		      itsMarkerLogicalVolume, // its mother  volume
		      false,                  // no boolean operation
		      0, BDSGlobalConstants::Instance()->GetCheckOverlaps());                     // copy number
  
#ifndef NOUSERLIMITS
  itsOuterUserLimits =
    new G4UserLimits("quadrupole cut",itsLength,DBL_MAX,BDSGlobalConstants::Instance()->GetMaxTime(),
		     BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  itsOuterLogicalVolume->SetUserLimits(itsOuterUserLimits);
#endif
}
