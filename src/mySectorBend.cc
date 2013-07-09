#include "BDSGlobalConstants.hh" 

#include "mySectorBend.hh"
#include "G4Box.hh"
#include "G4Torus.hh"
#include "G4IntersectionSolid.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UserLimits.hh"
#include "G4TransportationManager.hh"
#include "G4PropagatorInField.hh"
#include "G4SubtractionSolid.hh"

#include <map>

//============================================================

typedef std::map<G4String,int> LogVolCountMap;
extern LogVolCountMap* LogVolCount;

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
extern LogVolMap* LogVol;

extern G4RotationMatrix* RotY90;
extern G4RotationMatrix* RotX90;

//============================================================

mySectorBend::mySectorBend(G4String aName,G4double aLength, 
			   G4double bpRad,G4double FeRad,
			   G4double bField, G4double angle, G4double outR,
			   G4double tilt,  G4double bGrad, 
			   G4String aMaterial):
  BDSMultipole(aName,aLength,bpRad,FeRad,SetVisAttributes(),"",aMaterial,0,0,angle),
  itsPhysiInnerSB(NULL),itsPhysiCompSB(NULL),itsPhysiCompSBOuter(NULL),
  itsStepper(NULL),itsMagField(NULL),itsEqRhs(NULL)
{

  if (outR==0) 
    SetOuterRadius(BDSGlobalConstants::Instance()->GetComponentBoxSize()/2);
  else
    SetOuterRadius(outR);
  itsTilt = tilt;
  itsBField = bField;
  itsBGrad = bGrad;

  itsType="sbend";

  if (!(*LogVolCount)[itsName] )
    {
      BuildSBMarkerLogicalVolume();

      if(BDSGlobalConstants::Instance()->GetBuildTunnel()){
        BuildTunnel();
      }

      BuildSBBeampipe();

      BuildBPFieldAndStepper();

      BuildBPFieldMgr(itsStepper,itsMagField);

      BuildSBOuterLogicalVolume();

      if(BDSGlobalConstants::Instance()->GetSensitiveBeamPipe()){
        SetMultipleSensitiveVolumes(itsBeampipeLogicalVolume);
      }
      if(BDSGlobalConstants::Instance()->GetSensitiveComponents()){
        SetMultipleSensitiveVolumes(itsOuterLogicalVolume);
      }

      if(BDSGlobalConstants::Instance()->GetIncludeIronMagFields())
	{
	  G4double polePos[4];
	  G4double Bfield[3];

	  polePos[0]=0.;
	  polePos[1]=BDSGlobalConstants::Instance()->GetMagnetPoleRadius();
	  polePos[2]=0.;
	  polePos[0]=-999.;//flag to use polePos rather than local track
	  //coordinate in GetFieldValue	    
	  
      	  itsMagField->GetFieldValue(polePos,Bfield);
	  G4double BFldIron=
	  sqrt(Bfield[0]*Bfield[0]+Bfield[1]*Bfield[1])*
	  BDSGlobalConstants::Instance()->GetMagnetPoleSize()/
	  (BDSGlobalConstants::Instance()->GetComponentBoxSize()/2-
	  BDSGlobalConstants::Instance()->GetMagnetPoleRadius());

	  // Magnetic flux from a pole is divided in two directions
	  BFldIron/=2.;
	  
	  //BuildOuterFieldManager(2, BFldIron,0);
	}

      
      // vis attr
      G4VisAttributes* VisAtt = 
	new G4VisAttributes(G4Colour(0., 0., 0));
      VisAtt->SetForceSolid(true);
      itsInnerBPLogicalVolume->SetVisAttributes(VisAtt);
      
      G4VisAttributes* VisAtt1 = 
	new G4VisAttributes(G4Colour(0.1, 0.1, 0.1));
      VisAtt1->SetForceSolid(true);
      itsBeampipeLogicalVolume->SetVisAttributes(VisAtt1);
      
      G4VisAttributes* VisAtt2 = 
	new G4VisAttributes(G4Colour(0., 0., 1.));
      VisAtt2->SetForceSolid(true);
      itsOuterLogicalVolume->SetVisAttributes(VisAtt2);
      

      (*LogVolCount)[itsName]=1;
      (*LogVol)[itsName]=itsMarkerLogicalVolume;
    }
  else
    {
      (*LogVolCount)[itsName]++;
      if(BDSGlobalConstants::Instance()->GetSynchRadOn()&& BDSGlobalConstants::Instance()->GetSynchRescale())
	{
	  // with synchrotron radiation, the rescaled magnetic field
	  // means elements with the same name must have different
	  //logical volumes, becuase they have different fields
	  itsName+=BDSGlobalConstants::Instance()->StringFromInt((*LogVolCount)[itsName]);

	  BuildSBMarkerLogicalVolume();

	  BuildSBBeampipe();
	
	  BuildBPFieldAndStepper();

	  BuildBPFieldMgr(itsStepper,itsMagField);

	  BuildSBOuterLogicalVolume();

	  SetMultipleSensitiveVolumes(itsBeampipeLogicalVolume);
	  SetMultipleSensitiveVolumes(itsOuterLogicalVolume);

	  // vis attr
	  G4VisAttributes* VisAtt = 
	    new G4VisAttributes(G4Colour(0., 0., 0));
	  VisAtt->SetForceSolid(true);
	  itsInnerBPLogicalVolume->SetVisAttributes(VisAtt);
	  
	  G4VisAttributes* VisAtt1 = 
	    new G4VisAttributes(G4Colour(0.1, 0.1, 0.1));
	  VisAtt1->SetForceSolid(true);
	  itsBeampipeLogicalVolume->SetVisAttributes(VisAtt1);
	  
	  G4VisAttributes* VisAtt2 = 
	    new G4VisAttributes(G4Colour(0., 0., 1.));
	  VisAtt2->SetForceSolid(true);
	  itsOuterLogicalVolume->SetVisAttributes(VisAtt2);
	  
	  
	  (*LogVol)[itsName]=itsMarkerLogicalVolume;
	}
      else
	{
	  itsMarkerLogicalVolume=(*LogVol)[itsName];
	}      
    }
}

void mySectorBend::SynchRescale(G4double factor)
{
  itsStepper->SetBGrad(itsBGrad*factor);
  itsStepper->SetBField(-itsBField*factor);
  // note that there are no methods to set the BDSSBendMagField as this
  // class does not do anything with the BFields.
#ifdef DEBUG
  G4cout << "Sbend " << itsName << " has been scaled" << G4endl;
#endif
}

G4VisAttributes* mySectorBend::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0,1,1));
  return itsVisAttributes;
}


void mySectorBend::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper
  G4ThreeVector Bfield(0.,-itsBField,0.);
  itsMagField=new BDSSbendMagField(Bfield,itsLength,itsAngle);
  
  itsEqRhs=new G4Mag_UsualEqRhs(itsMagField);  
  
  itsStepper = new myQuadStepper(itsEqRhs); // note the - sign...
  itsStepper->SetBField(-itsBField);
  itsStepper->SetBGrad(itsBGrad);

  BuildBPFieldMgr(itsStepper,itsMagField);

  itsBeampipeLogicalVolume->SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);
  itsInnerBPLogicalVolume->SetFieldManager(itsBPFieldMgr,false);

}


void mySectorBend::BuildSBMarkerLogicalVolume()
{
  // radius of curvature
  G4double rho = itsLength/fabs(itsAngle);

  itsMarkerLogicalVolume=    
    new G4LogicalVolume(new G4Torus(itsName+"_marker",
				    0,               // inner R
				    itsOuterR,       // outer R
				    rho,             // swept R
				    0,               // starting phi
				    fabs(itsAngle)), // delta phi
			BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			itsName+"_marker");
#ifndef NOUSERLIMITS
  itsMarkerUserLimits = new G4UserLimits(DBL_MAX,DBL_MAX,DBL_MAX);
  itsMarkerUserLimits->SetMaxAllowedStep(itsLength);
  itsMarkerLogicalVolume->SetUserLimits(itsMarkerUserLimits);
#endif
}


// construct a beampipe for sector bend
void mySectorBend::BuildSBBeampipe()
{
  //
  // 1) construct the solids
  //

  // radius of curvature
  G4double rho = itsLength/fabs(itsAngle);

  // *** toroidal beampipes - problems with G4Torus::DistanceToTorus
  // *** when calling G4PolynomialSolver.Solve()
  G4double bpThickness = BDSGlobalConstants::Instance()->GetBeampipeThickness();
  G4Torus *pipeTube = new G4Torus(itsName+"_pipe_outer",
				  itsBpRadius-bpThickness, // innerR
				  itsBpRadius,             // outer R
				  rho,                     // swept R
				  0,                       // starting phi
				  fabs(itsAngle) );        // delta phi
  
  
  G4Torus *pipeInner = new G4Torus(itsName+"_pipe_inner",
				   0,                       // inner R
				   itsBpRadius-bpThickness, // outer R
				   rho,                     // swept R
				   0,                       // starting phi
				   fabs(itsAngle) );        // delta phi


  //
  // 2) construct the logical volumes (solids+materials)
  //

  // use default beampipe material
  G4Material *material =  BDSMaterials::Instance()->GetMaterial( BDSGlobalConstants::Instance()->GetPipeMaterialName());
  
  itsBeampipeLogicalVolume=	
    new G4LogicalVolume(pipeTube,
			material,
			itsName+"_bmp_logical");
  
  itsInnerBPLogicalVolume=	
    new G4LogicalVolume(pipeInner,
			BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			itsName+"_bmp_Inner_log");


  //
  // 3) position the logical volumes inside the mother volume (the marker)
  //

  itsPhysiInnerSB = 
    new G4PVPlacement(
		      (G4RotationMatrix*)0,		       // no rotation
		      (G4ThreeVector)0,                       // no translation
		      itsInnerBPLogicalVolume, // its logical volume
		      itsName+"_InnerBmp",     // its name
		      itsMarkerLogicalVolume,  // its mother volume
		      false,		       // no boolean operation
		      0, true);		       // copy number

  itsPhysiCompSB = 
    new G4PVPlacement(
		      (G4RotationMatrix*)0,    // no rotation
		      (G4ThreeVector)0,        // no translation
		      itsBeampipeLogicalVolume,// its logical volume
		      itsName+"_bmp",	       // its name
		      itsMarkerLogicalVolume,  // its mother  volume
		      false,	               // no boolean operation
		      0, true);	               // copy number
#ifndef NOUSERLIMITS
  itsBeampipeUserLimits =
    new G4UserLimits("beampipe cuts",DBL_MAX,DBL_MAX,DBL_MAX,
  		     BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  
  itsInnerBeampipeUserLimits =
    new G4UserLimits("inner beampipe cuts",DBL_MAX,DBL_MAX,DBL_MAX,
  		     BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  /*  
      itsOuterUserLimits =
      new G4UserLimits("sbend cut",itsLength,DBL_MAX,DBL_MAX,
      BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  
      itsOuterLogicalVolume->SetUserLimits(itsOuterUserLimits);
  */
  itsBeampipeUserLimits->SetMaxAllowedStep(itsLength);
  itsBeampipeLogicalVolume->SetUserLimits(itsBeampipeUserLimits);
  
  itsInnerBeampipeUserLimits->SetMaxAllowedStep(itsLength);
  itsInnerBPLogicalVolume->SetUserLimits(itsInnerBeampipeUserLimits);
#endif
  // zero field in the marker volume
  itsMarkerLogicalVolume->
    SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);

}

void mySectorBend::BuildSBOuterLogicalVolume(G4bool OuterMaterialIsVacuum)
{
  // radius of curvature
  G4double rho = itsLength/fabs(itsAngle);

  // compute sagitta (???)
  // G4double sagitta=0.;
  // if(itsNSegments>0)
  //   {
  //     sagitta=(itsLength/itsAngle*(1.-cos(itsAngle/2.)))/itsNSegments;
  //   }

  G4Material* material;
  if(itsMaterial != "") material = BDSMaterials::Instance()->GetMaterial(itsMaterial);
  else material = BDSMaterials::Instance()->GetMaterial("Iron");

  if(OuterMaterialIsVacuum)
    {
    itsOuterLogicalVolume=
      new G4LogicalVolume(new G4Torus(itsName+"_solid",
				      itsInnerIronRadius, // inner R
				      itsOuterR,          // outer R
				      rho,                // swept R
				      0,                  // starting phi
				      fabs(itsAngle) ),   // delta phi
                          BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
                          itsName+"_outer");
    }

  if(!OuterMaterialIsVacuum)
    {
    itsOuterLogicalVolume=
      new G4LogicalVolume(new G4Torus(itsName+"_solid",
				      itsInnerIronRadius, // inner R
				      itsOuterR,          // outer R
				      rho,                // swept R
				      0,                  // starting phi
				      fabs(itsAngle) ),   // delta phi
			  material,
                          itsName+"_outer");
    }

  itsPhysiCompSBOuter =
    new G4PVPlacement(
                      (G4RotationMatrix*)0,                        // no rotation
                      (G4ThreeVector)0,                        // no translation
                      itsOuterLogicalVolume,    // its logical volume
                      itsName+"_solid",         // its name
                      itsMarkerLogicalVolume,   // its mother  volume
                      false,                    // no boolean operation
                      0, true);                 // copy number
#ifndef NOUSERLIMITS
  itsOuterUserLimits =
    new G4UserLimits("multipole cut",itsLength,DBL_MAX,DBL_MAX,
                     BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  //  itsOuterUserLimits->SetMaxAllowedStep(aLength);
  itsOuterLogicalVolume->SetUserLimits(itsOuterUserLimits);
#endif
}

mySectorBend::~mySectorBend()
{
  delete itsVisAttributes;
  delete itsMagField;
  delete itsEqRhs;
  delete itsStepper;
}
