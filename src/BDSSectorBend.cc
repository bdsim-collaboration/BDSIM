#include "BDSGlobalConstants.hh" 
#include "BDSDebug.hh"

#include "BDSSectorBend.hh"

#include "BDSBeamPipeFactory.hh"
#include "BDSDipoleStepper.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetType.hh"
#include "BDSMaterials.hh"
#include "BDSSbendMagField.hh"
#include "BDSTunnelInfo.hh"
#include "BDSUtilities.hh"        // for calculateorientation

#include "G4CutTubs.hh"
#include "G4SubtractionSolid.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4UserLimits.hh"
#include "G4VPhysicalVolume.hh"

#include "globals.hh"             // geant4 types / globals

BDSSectorBend::BDSSectorBend(G4String           name,
			     G4double           length,
			     G4double           angle,
			     G4double           bField,
			     G4double           bGrad,
			     BDSBeamPipeInfo    beamPipeInfo,
			     BDSMagnetOuterInfo magnetOuterInfo,
			     BDSTunnelInfo      tunnelInfo):
  BDSMultipole(BDSMagnetType::sectorbend,name,length,beamPipeInfo,magnetOuterInfo,tunnelInfo),
  itsBField(bField),itsBGrad(bGrad)
{
  itsAngle = angle;
  // arc length = radius*angle
  //            = (chord length/(2.0*sin(angle/2))*angle
  if (itsAngle == 0.0)
     itsChordLength = itsLength;
  else
     itsChordLength = 2.0 * itsLength * sin(0.5*itsAngle) / itsAngle;

  // prepare normal vectors for input and output planes
  // calculate components of normal vectors (in the end mag(normal) = 1)
  orientation   = BDS::CalculateOrientation(itsAngle);
  G4double in_z = cos(0.5*fabs(itsAngle)); 
  G4double in_x = sin(0.5*fabs(itsAngle));
  inputface     = G4ThreeVector(-orientation*in_x, 0.0, -1.0*in_z);
  //-1 as pointing down in z for normal
  outputface    = G4ThreeVector(-orientation*in_x, 0.0, in_z);
}

void BDSSectorBend::Build()
{
  BDSMultipole::Build();
  
  if(BDSGlobalConstants::Instance()->GetIncludeIronMagFields())
    {
      G4double polePos[4];
      G4double Bfield[3];
      
      //coordinate in GetFieldValue
      polePos[0]=0.;
      polePos[1]=BDSGlobalConstants::Instance()->GetMagnetPoleRadius();
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
      
      BuildOuterFieldManager(2, BFldIron,CLHEP::halfpi);
    }
}

void BDSSectorBend::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper
  G4ThreeVector Bfield(0.,-itsBField,0.);
  // B-Field constructed with arc length for radius of curvature
  itsMagField = new BDSSbendMagField(Bfield,itsLength,itsAngle);
  itsEqRhs    = new G4Mag_UsualEqRhs(itsMagField);  
  BDSDipoleStepper* dipoleStepper = new BDSDipoleStepper(itsEqRhs);
  
  dipoleStepper->SetBField(-itsBField); // note the - sign...
  dipoleStepper->SetBGrad(itsBGrad);
  itsStepper = dipoleStepper;
}
void BDSSectorBend::BuildBeampipe()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "sector bend version " << G4endl;
#endif

  beampipe =
    BDSBeamPipeFactory::Instance()->CreateBeamPipeAngledInOut(beamPipeType,
							      itsName,
							      itsChordLength,
							      -itsAngle*0.5,
							      -itsAngle*0.5,
							      aper1,
							      aper2,
							      aper3,
							      aper4,
							      vacuumMaterial,
							      beamPipeThickness,
							      beamPipeMaterial);
  
  BeamPipeCommonTasks(); //from bdsmultipole;
}
