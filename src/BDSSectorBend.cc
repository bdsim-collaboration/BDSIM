#include "BDSGlobalConstants.hh" 

#include "BDSSectorBend.hh"
#include "G4Tubs.hh"
#include "G4Trd.hh"
#include "G4IntersectionSolid.hh"
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

BDSSectorBend::BDSSectorBend(G4String aName, G4double aLength, 
			     G4double bpRad, G4double FeRad,
			     G4double bField, G4double angle, G4double outR,
                             std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
			     G4double tilt, G4double bGrad, 
			     G4String aTunnelMaterial, G4String aMaterial, G4double aXAper, G4double aYAper):
  BDSMultipole(aName, aLength, bpRad, FeRad, SetVisAttributes(), blmLocZ, blmLocTheta, aTunnelMaterial, aMaterial,
	       aXAper, aYAper, angle),
  itsStepper(NULL),itsMagField(NULL),itsEqRhs(NULL)
{
  SetOuterRadius(outR);
  itsTilt=tilt;
  itsBField=bField;
  itsBGrad=bGrad;
  itsType="sbend";

  if (!(*LogVolCount)[itsName])
    {
      //
      // build external volume
      // 
      BuildSBMarkerLogicalVolume();
      G4VisAttributes* VisAtt1 = new G4VisAttributes(G4Colour(0.4, 0.4, 0.4));
      VisAtt1->SetVisibility(true);
      VisAtt1->SetForceSolid(true);
      itsMarkerLogicalVolume->SetVisAttributes(VisAtt1);
      //
      // build tunnel
      //
//       G4double oldTunnelRad=      BDSGlobalConstants::Instance()->GetTunnelRadius();
         
      if(BDSGlobalConstants::Instance()->GetBuildTunnel()){
	BuildTunnel();  //Do not build tunnel as there is a geometry problem
      }
      
      //      BDSGlobalConstants::Instance()->SetTunnelRadius(oldTunnelRad);

      //Build IP8Gate
      //      BuildGate();
     
      //
      // build beampipe (geometry + magnetic field)
      //
      
	      BuildBPFieldAndStepper();
	      BuildBPFieldMgr(itsStepper,itsMagField);
	      BuildSBBeampipe();

      //
      // build magnet (geometry + magnetic field)
      //
      

      BuildSBOuterLogicalVolume();
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

  BuildBLMs();
}

G4VisAttributes* BDSSectorBend::SetVisAttributes()
{
  itsVisAttributes = new G4VisAttributes(G4Colour(0,0,1)); //blue
  itsVisAttributes->SetForceSolid(true);
  itsVisAttributes->SetVisibility(true);
  return itsVisAttributes;
}

void BDSSectorBend::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper
  G4ThreeVector Bfield(0.,-itsBField,0.);
  itsMagField=new BDSSbendMagField(Bfield,itsLength,itsAngle);

  itsEqRhs=new G4Mag_UsualEqRhs(itsMagField);  
  
  itsStepper = new myQuadStepper(itsEqRhs);
  itsStepper->SetBField(-itsBField); // note the - sign...
  itsStepper->SetBGrad(itsBGrad);
}

void BDSSectorBend::BuildSBMarkerLogicalVolume()
{
  G4double xLength, yLength;
  xLength = yLength = std::max(itsOuterR,BDSGlobalConstants::Instance()->GetComponentBoxSize()/2);
  xLength = std::max(xLength, this->GetTunnelRadius()+2*std::abs(this->GetTunnelOffsetX()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness() + 4*BDSGlobalConstants::Instance()->GetLengthSafety() );   
  yLength = std::max(yLength, this->GetTunnelRadius()+2*std::abs(BDSGlobalConstants::Instance()->GetTunnelOffsetY()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness()+4*BDSGlobalConstants::Instance()->GetLengthSafety() );

  G4double transverseSize=2*std::max(xLength, yLength);

#ifdef DEBUG 
  G4cout<<"marker volume : x/y="<<transverseSize/CLHEP::m<<
    " m, l= "<<  (itsLength)/2/CLHEP::m <<" m"<<G4endl;
#endif

  G4double xHalfLengthPlus, xHalfLengthMinus;
  if(fabs(itsAngle) > 1e-20){
    xHalfLengthMinus = (itsLength/itsAngle)*sin(itsAngle/2)
      - fabs(cos(itsAngle/2))*transverseSize*tan(itsAngle/2)/2;
    
    xHalfLengthPlus = (itsLength/itsAngle)*sin(itsAngle/2)
      + fabs(cos(itsAngle/2))*transverseSize*tan(itsAngle/2)/2;
  } else {
    xHalfLengthPlus=(itsLength)/2.0;
    xHalfLengthMinus=(itsLength)/2.0;
  }
  
  if((xHalfLengthPlus<0) || (xHalfLengthMinus<0)){
    G4cerr << "Bend radius in " << itsName << " too small for this tunnel/component geometry. Exiting." << G4endl;
    G4cerr << "xHalfLengthPlus:  " << xHalfLengthPlus << G4endl;
    G4cerr << "xHalfLengthMinus: " << xHalfLengthMinus << G4endl;
    exit(1);
  }

  itsMarkerSolidVolume = new G4Trd(itsName+"_marker",
				   xHalfLengthPlus,     // x hlf lgth at +z
				   xHalfLengthMinus,    // x hlf lgth at -z
				   transverseSize/2,    // y hlf lgth at +z
				   transverseSize/2,    // y hlf lgth at -z
				   fabs(cos(itsAngle/2))*transverseSize/2);// z hlf lgth

  G4String LocalLogicalName=itsName;
  
  itsMarkerLogicalVolume=    
    new G4LogicalVolume(itsMarkerSolidVolume,
			BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			LocalLogicalName+"_marker");

  itsMarkerUserLimits = new G4UserLimits(DBL_MAX,DBL_MAX,DBL_MAX, BDSGlobalConstants::Instance()->GetThresholdCutCharged());
G4double  maxStepFactor=0.5;
  itsMarkerUserLimits->SetMaxAllowedStep(itsLength*maxStepFactor);
  itsMarkerLogicalVolume->SetUserLimits(itsMarkerUserLimits);
  

  //
  // zero field in the marker volume
  //
  itsMarkerLogicalVolume->
    SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);
}


// construct a beampipe for sector bend
void BDSSectorBend::BuildSBBeampipe()
{
  //
  // use default beampipe material
  //
    G4Material *material =  BDSMaterials::Instance()->GetMaterial( BDSGlobalConstants::Instance()->GetPipeMaterialName());
    //  G4Material *material =  BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()); //LD 8/11/10 
  
  //
  // compute some geometrical parameters
  //
  G4double boxSize = BDSGlobalConstants::Instance()->GetComponentBoxSize();

  G4double xHalfLengthPlus, xHalfLengthMinus, tubLen;
  if(itsAngle != 0){
    xHalfLengthMinus =
      (itsLength/itsAngle)*sin(itsAngle/2)
      - fabs(cos(itsAngle/2)) * boxSize * tan(itsAngle/2)/2;
    
    xHalfLengthPlus =
      (itsLength/itsAngle)*sin(itsAngle/2)
      + fabs(cos(itsAngle/2)) * boxSize * tan(itsAngle/2)/2;
    tubLen = std::max(xHalfLengthPlus,xHalfLengthMinus);
  } else {
    tubLen=(itsLength)/2.0;
  }

  

  //
  // build beampipe
  //
  G4VSolid *pipeTubsEnv = new G4SubtractionSolid("_pipe_outer_env",
                                               new G4EllipticalTube(itsName+"_pipe_outer_tmp_1",
                                                                    this->GetAperX()+BDSGlobalConstants::Instance()->GetBeampipeThickness(), 
                                                                    this->GetAperY()+BDSGlobalConstants::Instance()->GetBeampipeThickness(),          
                                                                    tubLen),
                                               new G4EllipticalTube(itsName+"_pipe_outer_tmp_2",
                                                                    this->GetAperX()+BDSGlobalConstants::Instance()->GetLengthSafety()/2.0, 
                                                                    this->GetAperY()+BDSGlobalConstants::Instance()->GetLengthSafety()/2.0,          
                                                                    tubLen*2)
                                               );



  
  G4VSolid *pipeInnerEnv = new G4EllipticalTube(itsName+"_pipe_outer_tmp_2",
                                                this->GetAperX(), 
                                                this->GetAperY(),          
                                                tubLen);

  G4IntersectionSolid *pipeTubs =
    new G4IntersectionSolid(itsName+"_pipe_outer",
			    pipeTubsEnv,
 			    itsMarkerSolidVolume,
			    BDSGlobalConstants::Instance()->RotYM90(),
			    (G4ThreeVector)0);
  
  G4IntersectionSolid *pipeInner =
    new G4IntersectionSolid(itsName+"_pipe_inner",
			    pipeInnerEnv, 
 			    itsMarkerSolidVolume,
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

  G4VPhysicalVolume* PhysiInner;
  PhysiInner = 
    new G4PVPlacement(
		      BDSGlobalConstants::Instance()->RotY90(),		       // rotation
		      (G4ThreeVector)0,	               // at (0,0,0)
		      itsInnerBPLogicalVolume, // its logical volume
		      itsName+"_InnerBmp",     // its name
		      itsMarkerLogicalVolume,  // its mother volume
		      false,		       // no booleanm operation
		      0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		       // copy number
  
  SetMultiplePhysicalVolumes(PhysiInner);

  G4VPhysicalVolume* PhysiComp;
  PhysiComp =
    new G4PVPlacement(
		      BDSGlobalConstants::Instance()->RotY90(),		        // rotation
		      (G4ThreeVector)0,	                // at (0,0,0)
		      itsBeampipeLogicalVolume, // its logical volume
		      itsName+"_bmp",	        // its name
		      itsMarkerLogicalVolume,   // its mother volume
		      false,		        // no boolean operation
		      0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		        // copy number
  
  SetMultiplePhysicalVolumes(PhysiComp);

  //
  // set user limits for stepping, tracking and propagation in B field
  //
#ifndef NOUSERLIMITS
  itsBeampipeUserLimits =
    new G4UserLimits("beampipe cuts",DBL_MAX,DBL_MAX,DBL_MAX,
  		     BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  G4double maxStepFactor=0.5;
  itsBeampipeUserLimits->SetMaxAllowedStep(itsLength*maxStepFactor);
  itsBeampipeUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  itsBeampipeLogicalVolume->SetUserLimits(itsBeampipeUserLimits);
  
  G4double maxStepFactorIn=0.5;
  itsInnerBeampipeUserLimits =
    new G4UserLimits("inner beampipe cuts",DBL_MAX,DBL_MAX,DBL_MAX,
  		     BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  itsInnerBeampipeUserLimits->SetMaxAllowedStep(itsLength*maxStepFactorIn);
  itsInnerBeampipeUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  itsInnerBPLogicalVolume->SetUserLimits(itsInnerBeampipeUserLimits);
#endif

  //
  // set magnetic field inside beampipe
  //
  itsBeampipeLogicalVolume->SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);
  itsInnerBPLogicalVolume->SetFieldManager(itsBPFieldMgr,false);

  //
  // set visualization attributes
  //
  G4VisAttributes* VisAtt =  new G4VisAttributes(G4Colour(0., 0., 0));
  VisAtt->SetForceSolid(true);
  itsInnerBPLogicalVolume->SetVisAttributes(VisAtt);

  G4VisAttributes* VisAtt1 = new G4VisAttributes(G4Colour(0.4, 0.4, 0.4));
  VisAtt1->SetVisibility(true);
  VisAtt1->SetForceSolid(true);
  itsBeampipeLogicalVolume->SetVisAttributes(VisAtt1);
}


G4double BDSSectorBend::GetArcLength()
{
  // arc length = radius*angle
  //            = (geometrical length/(2.0*sin(angle/2))*angle
  if (itsAngle == 0.0)
    return itsLength;
  else
    return (itsLength * (0.5*itsAngle) / sin(0.5*itsAngle));
}

void BDSSectorBend::BuildSBOuterLogicalVolume(G4bool OuterMaterialIsVacuum){

  G4Material* material;
  
  if(itsMaterial != "")
    material = BDSMaterials::Instance()->GetMaterial(itsMaterial);
  else
    material = BDSMaterials::Instance()->GetMaterial("Iron");
  
  G4double xHalfLengthPlus, xHalfLengthMinus, tubLen;
  if(itsAngle != 0){
    xHalfLengthMinus = (itsLength/itsAngle)*sin(itsAngle/2)
      - fabs(cos(itsAngle/2)) * BDSGlobalConstants::Instance()->GetComponentBoxSize() * tan(itsAngle/2)/2
      - BDSGlobalConstants::Instance()->GetLengthSafety()/2;

    xHalfLengthPlus = (itsLength/itsAngle)*sin(itsAngle/2)
    + fabs(cos(itsAngle/2)) * BDSGlobalConstants::Instance()->GetComponentBoxSize() * tan(itsAngle/2)/2
      - BDSGlobalConstants::Instance()->GetLengthSafety()/2;
    
    tubLen = std::max(xHalfLengthPlus,xHalfLengthMinus);
  } else {
    tubLen = (itsLength-BDSGlobalConstants::Instance()->GetLengthSafety())/2.0;
  }
  
  
  G4VSolid *magTubsEnv = new G4SubtractionSolid(itsName+"_solid_env",
                                                new G4Tubs(itsName+"_solid_tmp_1",
                                                           itsInnerIronRadius + BDSGlobalConstants::Instance()->GetLengthSafety()/2.0, // inner R + overlap safety
                                                           itsOuterR,          // outer R
                                                           tubLen,             // length
                                                           0,                  // starting phi
                                                           CLHEP::twopi * CLHEP::rad ),      // delta phi
                                                new G4EllipticalTube(itsName+"_pipe_outer_tmp_2",
                                                                     this->GetAperX()+BDSGlobalConstants::Instance()->GetBeampipeThickness()+BDSGlobalConstants::Instance()->GetLengthSafety()/2.0, 
                                                                     this->GetAperY()+BDSGlobalConstants::Instance()->GetBeampipeThickness()+BDSGlobalConstants::Instance()->GetLengthSafety()/2.0,          
                                                                     tubLen*2)
                                                );
  
  G4IntersectionSolid *magTubs =
    new G4IntersectionSolid(itsName+"_solid",
			    magTubsEnv,
			    itsMarkerSolidVolume,
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
  G4double  maxStepFactor=0.5;

#ifndef NOUSERLIMITS
  itsOuterUserLimits =
    new G4UserLimits("multipole cut",DBL_MAX,DBL_MAX,DBL_MAX,
                     BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  itsOuterUserLimits->SetMaxAllowedStep(itsLength*maxStepFactor);
  itsOuterUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  itsOuterLogicalVolume->SetUserLimits(itsOuterUserLimits);
#endif
}

BDSSectorBend::~BDSSectorBend()
{
  delete itsMagField;
  delete itsEqRhs;
  delete itsStepper;
}
