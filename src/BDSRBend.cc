#include "BDSGlobalConstants.hh" 

#include "BDSRBend.hh"
#include "G4Tubs.hh"
#include "G4Trd.hh"
#include "G4IntersectionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UserLimits.hh"
#include "G4TransportationManager.hh"

#include <map>

//============================================================

typedef std::map<G4String,int> LogVolCountMap;
extern LogVolCountMap* LogVolCount;

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
extern LogVolMap* LogVol;


//============================================================

BDSRBend::BDSRBend(G4String aName, G4double aLength, 
                   G4double bpRad, G4double FeRad,
                   G4double bField, G4double angle, G4double outR,
                   std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
                   G4double tilt, G4double bGrad, 
                   G4String aTunnelMaterial, G4String aMaterial):
  BDSMultipole(aName, aLength, bpRad, FeRad, SetVisAttributes(), blmLocZ, blmLocTheta, aTunnelMaterial, aMaterial,
	       0, 0, angle),
  markerSolidVolume(NULL),rbendRectangleSolidVolume(NULL),rbendRectangleLogicalVolume(NULL),
  middleBeampipeLogicalVolume(NULL),middleInnerBPLogicalVolume(NULL),endsBeampipeLogicalVolume(NULL),
  endsInnerBPLogicalVolume(NULL),endsBeampipeUserLimits(NULL),endsInnerBeampipeUserLimits(NULL),
  innerBeampipeVisAtt(NULL),beampipeVisAtt(NULL),itsStepper(NULL),itsMagField(NULL),itsEqRhs(NULL)
{
  SetOuterRadius(outR);
  itsTilt=tilt;
  itsBField=bField;
  itsBGrad=bGrad;
  //  itsBpRadius*=2; //bending magnet so double beam pipe radius
  
  //magnetic field length for rbend
  itsMagFieldLength = std::min (
				((itsLength/itsAngle)*sin(itsAngle/2)
				 - fabs(cos(itsAngle/2))*outR*tan(itsAngle/2)/2), 
				((itsLength/itsAngle)*sin(itsAngle/2)
				 + fabs(cos(itsAngle/2))*outR*tan(itsAngle/2)/2)
				);
#ifdef BDSDEBUG
  G4cout << "BDSRBend>> rbend itsMagFieldLength = " << itsMagFieldLength << G4endl;
#endif

  if (!(*LogVolCount)[itsName])
    {
      //
      // build external volume
      // 
      BuildRBMarkerLogicalVolume();

      //
      //build tunnel 
      if(BDSGlobalConstants::Instance()->GetBuildTunnel()){
	BuildTunnel(); //Geometry problem, do not build tunnel
      }
      
      //
      // build beampipe (geometry + magnetic field)
      //
      BuildBPFieldAndStepper();
      BuildBPFieldMgr(itsStepper,itsMagField);
      BuildRBBeampipe();

      //
      // build magnet (geometry + magnetic field)
      //
      BuildRBOuterLogicalVolume();
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
	  
	  BuildOuterFieldManager(2, BFldIron,CLHEP::pi/2);
	}
      BuildBLMs();
      //
      // define sensitive volumes for hit generation
      //
      if(BDSGlobalConstants::Instance()->GetSensitiveBeamPipe()){
        SetMultipleSensitiveVolumes(itsBeampipeLogicalVolume);
      }
      if(BDSGlobalConstants::Instance()->GetSensitiveComponents()){
        SetMultipleSensitiveVolumes(itsOuterLogicalVolume);
      }

      //
      // set visualization attributes
      //
      itsOuterLogicalVolume->SetVisAttributes(itsVisAttributes);

      //
      // append marker logical volume to volume map
      //
      (*LogVolCount)[itsName]=1;
      (*LogVol)[itsName]=itsMarkerLogicalVolume;
    }
  else
    {
      (*LogVolCount)[itsName]++;
	  //
	  // use already defined marker volume
	  //
	  itsMarkerLogicalVolume=(*LogVol)[itsName];
    }
}

G4VisAttributes* BDSRBend::SetVisAttributes()
{
  itsVisAttributes = new G4VisAttributes(G4Colour(0,0,1)); //blue
  itsVisAttributes->SetForceSolid(true);
  return itsVisAttributes;
}

void BDSRBend::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper
  G4ThreeVector Bfield(0.,-itsBField,0.);
  itsMagField=new BDSSbendMagField(Bfield,itsMagFieldLength,itsAngle);

  itsEqRhs=new G4Mag_UsualEqRhs(itsMagField);  
  
  itsStepper = new BDSDipoleStepper(itsEqRhs);
  itsStepper->SetBField(-itsBField); // note the - sign...
  itsStepper->SetBGrad(itsBGrad);
}

void BDSRBend::BuildRBMarkerLogicalVolume()
{
  if (markerSolidVolume==0) {

    G4double boxSize=BDSGlobalConstants::Instance()->GetComponentBoxSize()+BDSGlobalConstants::Instance()->GetTunnelRadius();

    G4double xHalfLengthMinus = (itsLength/itsAngle)*sin(itsAngle/2)
      - fabs(cos(itsAngle/2))*boxSize*tan(itsAngle/2)/2;
    
    G4double xHalfLengthPlus = (itsLength/itsAngle)*sin(itsAngle/2)
      + fabs(cos(itsAngle/2))*boxSize*tan(itsAngle/2)/2;

    markerSolidVolume = new G4Trd(itsName+"_marker",
				  xHalfLengthPlus,     // x hlf lgth at +z
				  xHalfLengthMinus,    // x hlf lgth at -z
				  boxSize/2,           // y hlf lgth at +z
				  boxSize/2,           // y hlf lgth at -z
				  fabs(cos(itsAngle/2))*boxSize/2);// z hlf lgth
    

    rbendRectangleSolidVolume = new G4Trd(itsName+"_rbend_rectangle",
					  itsMagFieldLength,     
					  itsMagFieldLength,    
					  boxSize/2,          
					  boxSize/2,           
					  fabs(cos(itsAngle/2))*boxSize/2);
    
  }

  G4String LocalLogicalName=itsName;
  
  itsMarkerLogicalVolume=    
    new G4LogicalVolume(markerSolidVolume,
			BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			LocalLogicalName+"_marker");

  rbendRectangleLogicalVolume=    
    new G4LogicalVolume(rbendRectangleSolidVolume,
			BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			LocalLogicalName+"_rbend_rectangle");
#ifndef NOUSERLIMITS
  itsMarkerUserLimits = new G4UserLimits(DBL_MAX,DBL_MAX,DBL_MAX);
  itsMarkerUserLimits->SetMaxAllowedStep(itsMagFieldLength*5);
  itsMarkerLogicalVolume->SetUserLimits(itsMarkerUserLimits);
  rbendRectangleLogicalVolume->SetUserLimits(itsMarkerUserLimits);
#endif
  //
  // zero field in the marker volume
  //
  itsMarkerLogicalVolume->
    SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);
}


// construct a beampipe for r bend
void BDSRBend::BuildRBBeampipe()
{
  //
  // use default beampipe material
  //
  G4Material *material =  BDSMaterials::Instance()->GetMaterial( BDSGlobalConstants::Instance()->GetPipeMaterialName());
  
  //
  // compute some geometrical parameters
  //
  G4double bpThickness = BDSGlobalConstants::Instance()->GetBeampipeThickness();
  G4double boxSize = BDSGlobalConstants::Instance()->GetComponentBoxSize();

  G4double xHalfLengthMinus =
    (itsLength/itsAngle)*sin(itsAngle/2)
    - fabs(cos(itsAngle/2)) * boxSize * tan(itsAngle/2)/2;


  G4double xHalfLengthPlus =
    (itsLength/itsAngle)*sin(itsAngle/2)
    + fabs(cos(itsAngle/2)) * boxSize * tan(itsAngle/2)/2;


  G4double tubLen = std::max(xHalfLengthPlus,xHalfLengthMinus);

  //
  // build beampipe
  //
  G4Tubs *pipeTubsEnv = new G4Tubs(itsName+"_pipe_outer_env",
				   itsBpRadius, // inner R
				   itsBpRadius+bpThickness+BDSGlobalConstants::Instance()->GetLengthSafety()/2.0,             // outer R
				   tubLen,                  // length
				   0,                       // starting phi
				   CLHEP::twopi * CLHEP::rad );           // delta phi
  
  G4Tubs *pipeInnerEnv = new G4Tubs(itsName+"_pipe_inner_env",
				    0,                       // inner R
				    itsBpRadius, // outer R
				    tubLen,                  // length
				    0,                       // starting phi
				    CLHEP::twopi * CLHEP::rad );           // delta phi

  G4IntersectionSolid *pipeTubs =
    new G4IntersectionSolid(itsName+"_pipe_outer",
			    pipeTubsEnv,
 			    markerSolidVolume,
			    BDSGlobalConstants::Instance()->RotYM90(),
			    (G4ThreeVector)0);
  
  G4IntersectionSolid *pipeInner =
    new G4IntersectionSolid(itsName+"_pipe_inner",
			    pipeInnerEnv, 
 			    markerSolidVolume,
			    BDSGlobalConstants::Instance()->RotYM90(),
			    (G4ThreeVector)0);

  G4IntersectionSolid *pipeMiddleTubs =
    new G4IntersectionSolid(itsName+"_pipe_middle_outer",
			    pipeTubsEnv,
 			    rbendRectangleSolidVolume,
			    BDSGlobalConstants::Instance()->RotYM90(),
			    (G4ThreeVector)0);
  
  G4IntersectionSolid *pipeMiddleInner =
    new G4IntersectionSolid(itsName+"_pipe_middle_inner",
			    pipeInnerEnv, 
 			    rbendRectangleSolidVolume,
			    BDSGlobalConstants::Instance()->RotYM90(),
			    (G4ThreeVector)0);

  G4SubtractionSolid *pipeEndsTubsTmp =
    new G4SubtractionSolid(itsName+"_pipe_ends_outer_tmp",
			    pipeTubsEnv,
 			    rbendRectangleSolidVolume,
			    BDSGlobalConstants::Instance()->RotYM90(),
			    (G4ThreeVector)0);
  
  G4SubtractionSolid *pipeEndsInnerTmp =
    new G4SubtractionSolid(itsName+"_pipe_ends_inner_tmp",
			    pipeInnerEnv, 
 			    rbendRectangleSolidVolume,
			    BDSGlobalConstants::Instance()->RotYM90(),
			    (G4ThreeVector)0);

  G4IntersectionSolid *pipeEndsTubs =
    new G4IntersectionSolid(itsName+"_pipe_ends_outer",
			    pipeEndsTubsTmp,
 			    markerSolidVolume,
			    BDSGlobalConstants::Instance()->RotYM90(),
			    (G4ThreeVector)0);
  
  G4IntersectionSolid *pipeEndsInner =
    new G4IntersectionSolid(itsName+"_pipe_ends_inner",
			    pipeEndsInnerTmp, 
 			    markerSolidVolume,
			    BDSGlobalConstants::Instance()->RotYM90(),
			    (G4ThreeVector)0);

  itsBeampipeLogicalVolume=	
    new G4LogicalVolume(pipeTubs,
			material,
			itsName+"_bmp_logical");
  
  itsInnerBPLogicalVolume=	
    new G4LogicalVolume(pipeInner,
			BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			itsName+"_bmp_Inner_log");

  middleBeampipeLogicalVolume=	
    new G4LogicalVolume(pipeMiddleTubs,
			material,
			itsName+"_bmp_logical_middle");
  
 middleInnerBPLogicalVolume=	
    new G4LogicalVolume(pipeMiddleInner,
			BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			itsName+"_bmp_Inner_log_middle");

 endsBeampipeLogicalVolume=	
    new G4LogicalVolume(pipeEndsTubs,
			material,
			itsName+"_bmp_logical_ends");
  
 endsInnerBPLogicalVolume=	
    new G4LogicalVolume(pipeEndsInner,
			BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			itsName+"_bmp_Inner_log_ends");



   //
  // set magnetic field inside beampipe
  //
 middleBeampipeLogicalVolume->SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);
 middleInnerBPLogicalVolume->SetFieldManager(itsBPFieldMgr,false);


  G4VPhysicalVolume* PhysiInner;
  PhysiInner = 
    new G4PVPlacement(
		      BDSGlobalConstants::Instance()->RotY90(),		       // rotation
		      (G4ThreeVector)0,	               // at (0,0,0)
		      middleInnerBPLogicalVolume, // its logical volume
		      itsName+"_InnerBmp",     // its name
		      itsMarkerLogicalVolume,  // its mother volume
		      false,		       // no booleanm operation
		      0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		       // copy number


  G4VPhysicalVolume* PhysiComp;
  PhysiComp =
    new G4PVPlacement(
		      BDSGlobalConstants::Instance()->RotY90(),		        // rotation
		      (G4ThreeVector)0,	                // at (0,0,0)
		      middleBeampipeLogicalVolume, // its logical volume
		      itsName+"_bmp",	        // its name
		      itsMarkerLogicalVolume,   // its mother volume
		      false,		        // no boolean operation
		      0, BDSGlobalConstants::Instance()->GetCheckOverlaps()
);		        // copy number

  G4VPhysicalVolume* PhysiInnerEnds;
  PhysiInnerEnds = 
    new G4PVPlacement(
		      BDSGlobalConstants::Instance()->RotY90(),		       // rotation
		      (G4ThreeVector)0,               // at (0,0,0)
		      endsInnerBPLogicalVolume, // its logical volume
		      itsName+"_InnerBmp",     // its name
		      itsMarkerLogicalVolume,  // its mother volume
		      false,		       // no booleanm operation
		      0, BDSGlobalConstants::Instance()->GetCheckOverlaps() );		       // copy number
  
  G4VPhysicalVolume* PhysiCompEnds;
  PhysiCompEnds =
    new G4PVPlacement(
		      BDSGlobalConstants::Instance()->RotY90(),		        // rotation
		      (G4ThreeVector)0,	                // at (0,0,0)
		      endsBeampipeLogicalVolume, // its logical volume
		      itsName+"_bmp",	        // its name
		      itsMarkerLogicalVolume,   // its mother volume
		      false,		        // no boolean operation
		      0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		        // copy number
  
  SetMultiplePhysicalVolumes(PhysiInner);
  SetMultiplePhysicalVolumes(PhysiComp);
  SetMultiplePhysicalVolumes(PhysiInnerEnds);
  SetMultiplePhysicalVolumes(PhysiCompEnds);
  
#ifndef NOUSERLIMITS
  //
  // set user limits for stepping, tracking and propagation in B field
  //
  itsBeampipeUserLimits =
    new G4UserLimits("beampipe cuts",DBL_MAX,DBL_MAX,DBL_MAX,
  		     BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  itsBeampipeUserLimits->SetMaxAllowedStep(itsMagFieldLength);
  itsBeampipeUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  middleBeampipeLogicalVolume->SetUserLimits(itsBeampipeUserLimits);

  G4double endsMaxAllowedStep = itsMagFieldLength;

  endsBeampipeUserLimits =
    new G4UserLimits("beampipe ends cuts",DBL_MAX,DBL_MAX,DBL_MAX,
  		     BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  endsBeampipeUserLimits->SetMaxAllowedStep(endsMaxAllowedStep);
  endsBeampipeUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  endsBeampipeLogicalVolume->SetUserLimits(endsBeampipeUserLimits);
  
  itsInnerBeampipeUserLimits =
    new G4UserLimits("inner beampipe cuts",DBL_MAX,DBL_MAX,DBL_MAX,
  		     BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  itsInnerBeampipeUserLimits->SetMaxAllowedStep(itsMagFieldLength);
  itsInnerBeampipeUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  middleInnerBPLogicalVolume->SetUserLimits(itsInnerBeampipeUserLimits);

  endsInnerBeampipeUserLimits =
    new G4UserLimits("inner beampipe ends cuts",DBL_MAX,DBL_MAX,DBL_MAX,
  		     BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  endsInnerBeampipeUserLimits->SetMaxAllowedStep(endsMaxAllowedStep);
  endsInnerBeampipeUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  endsInnerBPLogicalVolume->SetUserLimits(endsInnerBeampipeUserLimits);
#endif

  //
  // set visualization attributes
  //
  innerBeampipeVisAtt =  new G4VisAttributes(G4Colour(0., 0., 0));
  innerBeampipeVisAtt->SetForceSolid(true);
  middleInnerBPLogicalVolume->SetVisAttributes(innerBeampipeVisAtt);
  endsInnerBPLogicalVolume->SetVisAttributes(innerBeampipeVisAtt);

  beampipeVisAtt = new G4VisAttributes(G4Colour(0.4, 0.4, 0.4));
  beampipeVisAtt->SetForceSolid(true);
  middleBeampipeLogicalVolume->SetVisAttributes(beampipeVisAtt);
  endsBeampipeLogicalVolume->SetVisAttributes(beampipeVisAtt);
}

void BDSRBend::BuildRBOuterLogicalVolume(G4bool OuterMaterialIsVacuum){

  G4Material* material;
  if(itsMaterial != "")
    material = BDSMaterials::Instance()->GetMaterial(itsMaterial);
  else
    material = BDSMaterials::Instance()->GetMaterial("Iron");

  G4double boxSize = BDSGlobalConstants::Instance()->GetComponentBoxSize();
 
  G4double xHalfLengthMinus = (itsLength/itsAngle)*sin(itsAngle/2)
    - fabs(cos(itsAngle/2)) * boxSize * tan(itsAngle/2)/2;

  G4double xHalfLengthPlus = (itsLength/itsAngle)*sin(itsAngle/2)
    + fabs(cos(itsAngle/2)) * boxSize * tan(itsAngle/2)/2;

  G4double tubLen = std::max(xHalfLengthPlus,xHalfLengthMinus);
  
  G4Tubs *magTubsEnv =
    new G4Tubs(itsName+"_solid_env",
	       itsInnerIronRadius, // inner R + overlap safety
	       itsOuterR+BDSGlobalConstants::Instance()->GetLengthSafety()/2.0,          // outer R
	       tubLen,             // length
	       0,                  // starting phi
	       CLHEP::twopi * CLHEP::rad );      // delta phi
  
  G4IntersectionSolid *magTubs =
    new G4IntersectionSolid(itsName+"_solid",
			    magTubsEnv,
			    rbendRectangleSolidVolume,
			    BDSGlobalConstants::Instance()->RotYM90(),
			    (G4ThreeVector)0); 

  if(OuterMaterialIsVacuum)
    {
      itsOuterLogicalVolume = 
	new G4LogicalVolume(magTubs,
			    BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			    itsName+"_outer");
    }
  else
    {
      itsOuterLogicalVolume=
	new G4LogicalVolume(magTubs,
			    material,
			    itsName+"_outer");
    }

  itsPhysiComp =
    new G4PVPlacement(
                      BDSGlobalConstants::Instance()->RotY90(),                 // rotation
                      (G4ThreeVector)0,                      // at (0,0,0)
                      itsOuterLogicalVolume,  // its logical volume
                      itsName+"_solid",       // its name
                      itsMarkerLogicalVolume, // its mother  volume
                      false,                  // no boolean operation
                      0, BDSGlobalConstants::Instance()->GetCheckOverlaps());                     // copy number

  SetMultiplePhysicalVolumes(itsPhysiComp);

#ifndef NOUSERLIMITS
  itsOuterUserLimits =
    new G4UserLimits("multipole cut",DBL_MAX,DBL_MAX,BDSGlobalConstants::Instance()->GetMaxTime(),
                     BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  itsOuterUserLimits->SetMaxAllowedStep(itsMagFieldLength);
  itsOuterLogicalVolume->SetUserLimits(itsOuterUserLimits);
#endif
}

BDSRBend::~BDSRBend()
{
  delete innerBeampipeVisAtt;
  delete beampipeVisAtt;
//   delete itsBeampipeUserLimits;
//   delete endsBeampipeUserLimits;
//   delete endsInnerBeampipeUserLimits;
  delete itsMagField;
  delete itsEqRhs;
  delete itsStepper;
}
