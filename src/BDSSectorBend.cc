#include "BDSGlobalConstants.hh" 
#include "BDSDebug.hh"

#include "BDSSectorBend.hh"

#include "BDSBeamPipeFactory.hh"
#include "BDSDipoleStepper.hh"
#include "BDSMaterials.hh"
#include "BDSSbendMagField.hh"
#include "BDSSDManager.hh"
#include "BDSUtilities.hh"        // for calculateorientation

#include "G4Polyhedra.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4CutTubs.hh"
#include "G4EllipticalTube.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4PVPlacement.hh"               
#include "G4UserLimits.hh"
#include "G4VPhysicalVolume.hh"

#include "globals.hh"             // geant4 types / globals

BDSSectorBend::BDSSectorBend(G4String        name,
			     G4double        length,
			     G4double        angle,
			     G4double        bField,
			     G4double        bGrad,
			     BDSBeamPipeInfo beamPipeInfoIn,
			     G4double        boxSize,
			     G4String        outerMaterial,
			     G4String        tunnelMaterial,
			     G4double        tunnelRadius,
			     G4double        tunnelOffsetX):
  BDSMultipole(name,length,beamPipeInfoIn,boxSize,outerMaterial,tunnelMaterial,tunnelRadius,tunnelOffsetX),
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

void BDSSectorBend::SetVisAttributes()
{
  itsVisAttributes = new G4VisAttributes(G4Colour(0,0,1)); //blue
  itsVisAttributes->SetForceSolid(true);
  itsVisAttributes->SetVisibility(true);
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

void BDSSectorBend::BuildOuterLogicalVolume(G4bool /*OuterMaterialIsVacuum*/)
{
  // build magnet outer geometry + magnetic field    
  G4String geometry = "cylinder";
 
  if(geometry =="standard") 
    BuildStandardOuterLogicalVolume(false); // standard - sbend with poles and pockets
  else if(geometry =="cylinder")  
    BuildCylindricalOuterLogicalVolume(false); // cylinder outer volume
  else //default - cylinder - standard
    BuildCylindricalOuterLogicalVolume(false); // cylinder outer volume
  
  //remember if it's vacuum, it won't be built - have to check it's there
  if (itsOuterLogicalVolume)
    {itsOuterLogicalVolume->SetSensitiveDetector(BDSSDManager::Instance()->GetEnergyCounterOnAxisSD());}
}

void BDSSectorBend::BuildMarkerLogicalVolume()
{
  G4double xLength, yLength;
  xLength = yLength = std::max(itsOuterR,BDSGlobalConstants::Instance()->GetComponentBoxSize()/2);
  xLength = std::max(xLength, this->GetTunnelRadius()+2*std::abs(this->GetTunnelOffsetX()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness() + 4*BDSGlobalConstants::Instance()->GetLengthSafety() );
  yLength = std::max(yLength, this->GetTunnelRadius()+2*std::abs(BDSGlobalConstants::Instance()->GetTunnelOffsetY()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness()+4*BDSGlobalConstants::Instance()->GetLengthSafety() );

  G4double transverseSize = 2.0 * std::max(xLength, yLength); //factor of 2 fairly arbitrary i think

#ifdef BDSDEBUG 
  G4cout<<"marker volume : x/y="<<transverseSize/CLHEP::m<<
    " m, l= "<<  (itsChordLength)/CLHEP::m <<" m"<<G4endl;
#endif

  //make marker volume from G4cuttubs - a cylinder along the chord line with angled faces defined by normal vectors
  //this is done first so other solids (beam pipe) can be trimmed with the angled face
  
  itsMarkerSolidVolume = new G4CutTubs( itsName+"_marker",  // name
					0.0,                // minimum radius = 0 for solid cylinder
					transverseSize/2.0, // radius - determined above
					itsChordLength/2.0, // length about centre point
					0.0,                // starting angle
					2.0*CLHEP::pi,      // finishing angle - full
					inputface,          // input face normal vector
					outputface );       // output face normal vector

  G4String LocalLogicalName = itsName;
  itsMarkerLogicalVolume=    
    new G4LogicalVolume(itsMarkerSolidVolume,
			BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetEmptyMaterial()),
			LocalLogicalName+"_marker");

#ifndef NOUSERLIMITS
  itsMarkerUserLimits = new G4UserLimits(*(BDSGlobalConstants::Instance()->GetDefaultUserLimits()));
  itsMarkerUserLimits->SetMaxAllowedStep(itsLength*0.5);
  itsMarkerLogicalVolume->SetUserLimits(itsMarkerUserLimits);
#endif
  
  // zero field in the marker volume
  itsMarkerLogicalVolume->
    SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);

  SetExtentX(-transverseSize*0.5,transverseSize*0.5);
  SetExtentY(-transverseSize*0.5,transverseSize*0.5);
  SetExtentZ(-itsChordLength*0.5,itsChordLength*0.5);
}

void BDSSectorBend::BuildBeampipe()
{
  #ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " sector bend version " << G4endl;
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

void BDSSectorBend::BuildCylindricalOuterLogicalVolume(G4bool outerMaterialIsVacuum)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // check if outer volume is required
  if (outerMaterialIsVacuum)
    {return;} // no need to create another volume

  // test beampipe instance exists / has been built already
  if (!beampipe){
    G4cerr << __METHOD_NAME__ << " no beampipe has been built - can't wrap around it" << G4endl;
    exit(1);
  }

  // build the logical volume
  G4Material* material;
  if(itsMaterial != "")
    { material = BDSMaterials::Instance()->GetMaterial(itsMaterial);}
  else
    { material = BDSMaterials::Instance()->GetMaterial("Iron");}

  G4double lengthSafety = BDSGlobalConstants::Instance()->GetLengthSafety();
  G4double outerRadius  = boxSize*0.5;
  if (beampipe->ContainerIsCircular())
    {
      // simple circular beampipe - no need for a subtraction solid
      G4double innerRadius = beampipe->GetContainerRadius()+lengthSafety;
      
      // check outerRadius is bigger
      if (outerRadius < innerRadius)
	{
	  G4cout << __METHOD_NAME__ << " - warning - beampipe is bigger than the boxSize" << G4endl
		 << "setting boxSize to be just big enough to contain beampipe " << G4endl;
	  outerRadius = innerRadius+1*CLHEP::cm;
	}
      itsOuterLogicalVolume = new G4LogicalVolume(new G4CutTubs( itsName+"_outer_solid",            // name
								 innerRadius,
								 outerRadius,
								 itsChordLength*0.5-2*lengthSafety, // length about centre point
								 0.0,                               // starting angle
								 2.0*CLHEP::pi,                     // finishing angle - full
								 inputface,                         // input face normal vector
								 outputface ),
						  material,
						  itsName+"_outer_lv");
     
    }
  else
    {
      // not a simple circular beampipe - have to use subtraction solid
      // check outer radius is really outside the beampipe
      G4double maxX = std::max(beampipe->GetExtentX().first,beampipe->GetExtentX().second);
      G4double maxY = std::max(beampipe->GetExtentY().first,beampipe->GetExtentY().second);
      if (outerRadius < maxX)
	{outerRadius = maxX + 1*CLHEP::cm;} //minimum extra size
      if (outerRadius < maxY)
	{outerRadius = maxY + 1*CLHEP::cm;}
      G4double hypotenuse = sqrt(maxX*maxX + maxY*maxY);
      if (outerRadius < hypotenuse)
	{outerRadius = hypotenuse + 1*CLHEP::cm;}
      
      itsOuterLogicalVolume =
	new G4LogicalVolume( new G4SubtractionSolid (itsName+"_outer_solid",
						     new G4CutTubs( itsName+"_outer_solid_cylinder",   // name
								    0.0,     // solid cylinder for unambiguous subtraction
								    outerRadius,
								    itsChordLength*0.5-2*lengthSafety, // length about centre point
								    0.0,                               // starting angle
								    2.0*CLHEP::pi,                     // finishing angle - full
								    inputface,                         // input face normal vector
								    outputface ),
						     beampipe->GetContainerSubtractionSolid() ),
						     material,
						     itsName+"_outer_lv");
    }
  
  RegisterLogicalVolume(itsOuterLogicalVolume);

  // place the outer volume
  itsPhysiComp = new G4PVPlacement(
				   (G4RotationMatrix*)0,   // no rotation
				   (G4ThreeVector)0,       // its position
				   itsOuterLogicalVolume,  // its logical volume
				   itsName+"_outer_phys",  // its name
				   itsMarkerLogicalVolume, // its mother  volume
				   false,		   // no boolean operation
				   0,                      // copy number
				   BDSGlobalConstants::Instance()->GetCheckOverlaps());		      
  
  // set visualization attributes
  itsOuterLogicalVolume->SetVisAttributes(itsVisAttributes);

#ifndef NOUSERLIMITS
  G4double maxStepFactor=0.5;
  itsOuterUserLimits = new G4UserLimits(*(BDSGlobalConstants::Instance()->GetDefaultUserLimits()));
  itsOuterUserLimits->SetMaxAllowedStep(itsLength*maxStepFactor);
  itsOuterLogicalVolume->SetUserLimits(itsOuterUserLimits);
#endif
}
 

////////////////////////////////////////////////////////////////////////////////
//                             Detailed geometry                              //
////////////////////////////////////////////////////////////////////////////////

void BDSSectorBend::BuildStandardOuterLogicalVolume(G4bool OuterMaterialIsVacuum){

  G4Material* material;
  
  if(itsMaterial != "")
    material = BDSMaterials::Instance()->GetMaterial(itsMaterial);
  else
    material = BDSMaterials::Instance()->GetMaterial("Iron");

  G4double beamPipeThickness = BDSGlobalConstants::Instance()->GetBeamPipeThickness();
  G4double lengthSafety      = BDSGlobalConstants::Instance()->GetLengthSafety();
  
  G4VSolid *magTubsEnv = 
    new G4SubtractionSolid(itsName+"_solid_env",
			   new G4Tubs(itsName+"_solid_tmp_1",
				      itsInnerIronRadius + lengthSafety/2.0, // inner R + overlap safety
				      itsOuterR,                    // outer R
				      itsChordLength,               // length
				      0,                            // starting phi
				      CLHEP::twopi * CLHEP::rad ),  // delta phi
			   new G4EllipticalTube(itsName+"_pipe_outer_tmp_2",
						this->GetAperX()+beamPipeThickness+lengthSafety,
						this->GetAperY()+beamPipeThickness+lengthSafety,
						itsChordLength*2.2) // full length + 20% for unambiguous subtraction
			   );

  G4Material* air  = G4Material::GetMaterial("G4_AIR");
  G4double worldLength = 2*CLHEP::m;

  G4cout << "worldLength " << worldLength;

  // Definitions of Solids, Logical Volumes, Physical Volumes
  // World

  G4Box* worldS
    = new G4Box("world",                                    //its name
                worldLength/2,worldLength/2,worldLength/2); //its size
  G4LogicalVolume* worldLV
    = new G4LogicalVolume(
                 worldS,   //its solid
                 air,      //its material
                 "World"); //its name
  
  //  Must place the World Physical volume unrotated at (0,0,0)

  // magnet parameters
  double mag_inradius = 192*CLHEP::mm; // inner radius
  double mag_extradius = 400*CLHEP::mm; // external radius

  // Defining external shape using polyhedra
  double zplanepos [2] = {0,itsChordLength};
  double pipelength [2] = {-2.0*CLHEP::cm,itsChordLength+2.0*CLHEP::cm};
  double rinner [2] = {mag_inradius, mag_inradius};
  double router [2] = {mag_extradius, mag_extradius};

  G4Polyhedra* polyShape = new G4Polyhedra("polyShape", 0.*CLHEP::deg, 360.*CLHEP::deg, 4, 2, zplanepos, rinner, router);

  // Coils
  double coil_size_x = 8.0*CLHEP::cm;
  double coil_size_y = 4.0*CLHEP::cm;

  G4VSolid* Coil = new G4Box("Coil",coil_size_x,coil_size_y,itsChordLength/2.0);

  G4LogicalVolume* CoilLV = 
    new G4LogicalVolume(Coil,      //its solid
                        material,  //its material
                        "CoilLV"); //its name
    
  // Beampipe
  double beampipe_rinner [2] = {0.0, 0.0};
  double beampipe_router [2] = {this->GetAperY()+beamPipeThickness, this->GetAperY()+beamPipeThickness};

  G4Polyhedra* Beampipe = new G4Polyhedra("Beampipe", 0.*CLHEP::deg, 360.*CLHEP::deg, 4, 2, pipelength, beampipe_rinner, beampipe_router);
  G4LogicalVolume* BeampipeLV = 
    new G4LogicalVolume(Beampipe,   //its solid
                        material,   //its material
                        "BeampipeLV"); //its name

  // Iron yoke subtracting beampipe
  G4LogicalVolume* shieldLV = 
    new G4LogicalVolume(  new G4SubtractionSolid(
						 "Shield",
						 polyShape,
						 Beampipe,
						 0, 
						 G4ThreeVector((mag_extradius+mag_inradius)/2.0*cos(CLHEP::pi/4.0),
							       (mag_extradius+mag_inradius)/2.0*sin(CLHEP::pi/4.0),0.)),             //its solid
			  material,    //its material
			  "shieldLV"); //its name

  G4ThreeVector positionQuad = G4ThreeVector(0,0,0);
  
  // Place the external shape in the mother volume

  G4RotationMatrix* rm = new G4RotationMatrix();
  rm->rotateZ(360.0/2.0/4.0*CLHEP::deg);
  G4ThreeVector uz = G4ThreeVector(-35.*CLHEP::cm,0.,0.); 

  G4PVPlacement* shieldPhysVol = new G4PVPlacement(rm,             //rotation,
						   uz,             //position
						   shieldLV,       //its logical volume
						   "poleShield",   //its name
						   worldLV,        //its mother  volume
						   false,          //no boolean operation
						   0,              //copy number
						   BDSGlobalConstants::Instance()->GetCheckOverlaps()); // checking ove

  SetMultiplePhysicalVolumes(shieldPhysVol);

  double magnet_shift = -35.*CLHEP::cm; // displace magnet to center ir with respect to the beampipe
  double coil_pos_x = mag_inradius-coil_size_x;
  double coil_pos_y = mag_inradius-coil_size_y;
  G4ThreeVector positionCoil1 = G4ThreeVector(coil_pos_x + magnet_shift,coil_pos_y,itsChordLength/2.0);
  
  G4PVPlacement* coil1PhysVol = new G4PVPlacement(0,             //rotation,
						  positionCoil1, //position
						  CoilLV,        //its logical volume
						  "Coil",        //its name
						  worldLV,       //its mother  volume
						  false,         //no boolean operation
						  0,             //copy number
						  BDSGlobalConstants::Instance()->GetCheckOverlaps()); // checking overlaps

  G4ThreeVector positionCoil2 = G4ThreeVector(coil_pos_x + magnet_shift,-coil_pos_y,itsChordLength/2.0);
  
  G4PVPlacement* coil2PhysVol = new G4PVPlacement(0,             //rotation,
						  positionCoil2, //position
						  CoilLV,        //its logical volume
						  "Coil",        //its name
						  worldLV,       //its mother  volume
						  false,         //no boolean operation
						  0,             //copy number
						  BDSGlobalConstants::Instance()->GetCheckOverlaps()); // checking overlaps


  double coil_pos_x2 = mag_extradius+coil_size_x;
  double coil_pos_y2 = mag_inradius-coil_size_y;
  G4ThreeVector positionCoil3 = G4ThreeVector(coil_pos_x2 + magnet_shift,coil_pos_y2,itsChordLength/2.0);
  
  G4PVPlacement* coil3PhysVol = new G4PVPlacement(0,             //rotation,
						  positionCoil3, //position
						  CoilLV,        //its logical volume
						  "Coil",        //its name
						  worldLV,       //its mother  volume
						  false,         //no boolean operation
						  0,             //copy number
						  BDSGlobalConstants::Instance()->GetCheckOverlaps()); // checking overlaps

 G4ThreeVector positionCoil4 = G4ThreeVector(coil_pos_x2 + magnet_shift,-coil_pos_y2,itsChordLength/2.0);
  
  G4PVPlacement* coil4PhysVol = new G4PVPlacement(0,             //rotation,
						  positionCoil4, //position
						  CoilLV,        //its logical volume
						  "Coil",        //its name
						  worldLV,       //its mother  volume
						  false,         //no boolean operation
						  0,             //copy number
						  BDSGlobalConstants::Instance()->GetCheckOverlaps()); // checking overlaps

  SetMultiplePhysicalVolumes(coil1PhysVol);
  SetMultiplePhysicalVolumes(coil2PhysVol);
  SetMultiplePhysicalVolumes(coil3PhysVol);
  SetMultiplePhysicalVolumes(coil4PhysVol);

  
  G4VisAttributes* magnetVisAtt = new G4VisAttributes(G4Colour(0.4, 0.4, 0.4));

  BeampipeLV ->SetVisAttributes(magnetVisAtt);
  
  // color-coding for the pole
  G4VisAttributes* VisAtt = new G4VisAttributes(G4Colour(0.,0.,1.));
  VisAtt->SetForceSolid(true);
  CoilLV->SetVisAttributes(VisAtt);
  shieldLV->SetVisAttributes(VisAtt);

  G4RotationMatrix* rm2 = new G4RotationMatrix();
  rm2->rotateZ(45.0*CLHEP::deg);

  G4IntersectionSolid *magTubs = new G4IntersectionSolid(itsName+"_solid",
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

  G4ThreeVector worldposition = G4ThreeVector(0.,0.,0.);


  itsPhysiComp =  new G4PVPlacement(0,                      // rotation
				    worldposition,          // position at (0,0,0)
				    worldLV,                // logical volume
				    itsName+"_solid",       // its name
				    itsMarkerLogicalVolume, // its mother  volume
				    false,                  // no boolean operation
				    0,                      // copy number
				    BDSGlobalConstants::Instance()->GetCheckOverlaps());
  
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

G4double BDSSectorBend::GetChordLength()
{
  return itsChordLength;
}

BDSSectorBend::~BDSSectorBend()
{
}

void BDSSectorBend::BuildOuterVolume()
{
  return;
}
