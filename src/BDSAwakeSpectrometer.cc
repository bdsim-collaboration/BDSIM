/* BDSIM code.    
AWAKE spectrometer.
*/

#include "BDSGlobalConstants.hh" 
#include "BDSAwakeSpectrometer.hh"
#include "BDSMaterials.hh"
#include "BDSSampler.hh"
#include "BDSSamplerSD.hh"
#include "BDSCCDCamera.hh"
#include "G4Box.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4UserLimits.hh"
#include "G4SubtractionSolid.hh"
#include "BDSDebug.hh"

#include "BDSAwakeMultilayerScreen.hh"
//#include "UltraFresnelLens.hh"
//#include "UltraFresnelLensParameterisation.hh"

#include "G4Trap.hh"
//#include "BDSOutputBase.hh"

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
extern LogVolMap* LogVol;

//============================================================
BDSAwakeSpectrometer::BDSAwakeSpectrometer (G4String aName, G4double length=2.7*CLHEP::m, G4String bmapFile = "", G4double bmapZOffset =0, G4double poleStartZ=62.733*CLHEP::cm, G4String material="lanex", G4double thickness = 0.3 * CLHEP::mm, G4double windowScreenGap=0, G4double angle = -45*CLHEP::pi/180.0, G4double windowThickness=0, G4String windowMaterial="G4_Al", G4double screenEndZ = (258-62.733)*CLHEP::cm):
  BDSAcceleratorComponent(aName, length, 0, 0, 0,"","",0,0,0,0,0,0,"",bmapFile, bmapZOffset), _mlScreen(NULL), _camera(NULL), _poleStartZ(poleStartZ), _screenEndZ(screenEndZ), _material(material), _thickness(thickness), _screenAngle(angle), _windowScreenGap(windowScreenGap), _windowThickness(windowThickness), _windowMaterial(windowMaterial)
{
  _vacChambType=2;
  //Set as part of precision region (for energy loss monitoring)
  itsPrecisionRegion=1;

  //Set the rotation of the screen
  _screenRotationMatrix = new G4RotationMatrix();
    _screenRotationMatrix->rotateY(_screenAngle);

  _vacRotationMatrix = new G4RotationMatrix();
  MagnetDefaults();
}

void BDSAwakeSpectrometer::MagnetDefaults(){
  //Initialise geometry pointers.
  itsCoilLog=NULL;
  //Initialise dimensions.
  itsYokeSize.set(
		  902*CLHEP::mm,
		  1168*CLHEP::mm,		
		  1*CLHEP::m
		  );

  itsCoilSize.set(
		  (320+320+262)*CLHEP::mm,
		  180*CLHEP::mm,
		  (1000+320+285)*CLHEP::mm
		  );
  itsUpstreamCoilLength=285*CLHEP::mm; //The part of the could that sticks out of the upstream/downstream end;
  itsDownstreamCoilLength=320*CLHEP::mm;
  //The geometric centre (i.e. centre of the magnet) relative to the yoke.
  itsMagnetCentre.set(
		      243*CLHEP::mm,
		      0,
		      0
		      );
  //The coil position relative to the yoke centre.
  itsCoilPos.set(
		 (itsYokeSize.x() + itsCoilSize.x())/2.0 - (320+262)*CLHEP::mm,
		 0,
		 (itsDownstreamCoilLength-itsUpstreamCoilLength)/2.0
		 );
  //The part of the aperture that is within the yoke.
  itsApertureSize.set(
		      (320+262+30)*CLHEP::mm,
		      180*CLHEP::mm,
		      itsYokeSize.z()
		      );
  //Aperture position relative to the centre of the yoke.
  itsAperturePos.set(
		     (itsYokeSize.x()-itsApertureSize.x())/2.0, //C-type magnet, aperture is to the side.
		     itsMagnetCentre.y(), //Aperture is centred vertically around the magnet centre.
		     0
		     );
  //Upper coil position relative to magnet centre.
  itsUpperCoilPos.set(
		      itsCoilPos.x(),
		      (itsCoilSize.y()+itsApertureSize.y())/2.0,
		      itsCoilPos.z()
		      );
  itsLowerCoilPos.set(
		      itsUpperCoilPos.x(),
		      -1*itsUpperCoilPos.y(),
		      itsUpperCoilPos.z()
		      );
  itsPoleSize.set(
		  320*CLHEP::mm,
		  50*CLHEP::mm,
		  itsYokeSize.z()
		  );
  itsUpperPolePos.set(
		      itsMagnetCentre.x(),
		      (itsPoleSize.y()+itsApertureSize.y())/2.0,
		      itsMagnetCentre.z()
		      );
  itsLowerPolePos.set(
		      itsUpperPolePos.x(),
		      -1 * itsUpperPolePos.y(),
		      itsUpperPolePos.z()
		      );
  
}

void BDSAwakeSpectrometer::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0.3,0.4,0.2));
  itsVisAttributes->SetForceWireframe(true);

  _visAttFront=new G4VisAttributes(G4Colour(1.0,0.0,0.0,0.5));
  _visAttScint=new G4VisAttributes(G4Colour(0.0,1.0,0.0,0.5));
  _visAttBase =new G4VisAttributes(G4Colour(0.3,0.3,0.3,0.5));
  _visAttSampler=new G4VisAttributes(G4Colour(0.2,0.2,0.0,0.5));
  

  _visAttFront->SetForceSolid(true);
  _visAttScint->SetForceSolid(true);
  _visAttBase->SetForceSolid(true);
  _visAttSampler->SetForceSolid(true);
  _visAttSampler->SetVisibility(true);
}

void BDSAwakeSpectrometer::BuildMagnet(){
  BuildYoke();
  BuildPoles();
  BuildCoils();
  BuildField();
  BuildVacuumChamber();
}

void BDSAwakeSpectrometer::BuildCoils(){
  G4ThreeVector size = itsCoilSize;
  G4VSolid*  coilSolid = new G4Box("coilSolid",size.x(),size.y(),size.z());
  itsCoilLog = new G4LogicalVolume(coilSolid, BDSMaterials::Instance()->GetMaterial("G4_Cu"),"itsCoilLog",0,0,0);
}

void BDSAwakeSpectrometer::BuildPoles(){
  G4ThreeVector size = itsPoleSize;
  G4VSolid* poleSolid = new G4Box("poleSolid",size.x(),size.y(),size.z());
  itsPoleLog = new G4LogicalVolume(poleSolid, BDSMaterials::Instance()->GetMaterial("G4_Cu"),"itsPoleLog",0,0,0);
}

void BDSAwakeSpectrometer::BuildYoke(){
  G4ThreeVector size = itsYokeSize;
  G4ThreeVector pos = itsYokePos;
  if(itsCoilLog==NULL){
    BuildCoils();
  }
  //Build the aperture solid...
  G4VSolid* ApertureSolid = new G4Box("AperSolid1",size.x(),size.y(),size.z());
  //  Build a block for the yoke..
  G4VSolid* YokeSolid1 = new G4Box("YokeSolid1",size.x(),size.y(),size.z());
  // Subtract the aperture..
  G4VSolid* YokeSolid2 = new G4SubtractionSolid("YokeSolid2", YokeSolid1,ApertureSolid, nullRotationMatrix, itsAperturePos);
  //Subtract gaps for the coils..					       
  G4VSolid* YokeSolid3 = new G4SubtractionSolid("YokeSolid3", YokeSolid2,itsCoilLog->GetSolid(), nullRotationMatrix, itsUpperCoilPos);
  G4VSolid* YokeSolid = new G4SubtractionSolid("itsYokeSolid", YokeSolid3,itsCoilLog->GetSolid(), nullRotationMatrix, itsLowerCoilPos);
  itsYokeLog = new G4LogicalVolume(YokeSolid, BDSMaterials::Instance()->GetMaterial("G4_Fe"),"itsYokeLog",0,0,0);
  G4VisAttributes* YokeVisAtt = new G4VisAttributes(G4Color(1,0,0));
  YokeVisAtt->SetForceWireframe(true);
  YokeVisAtt->SetVisibility(true);
  itsYokeLog->SetVisAttributes(YokeVisAtt);
}

void BDSAwakeSpectrometer::PlaceYoke(){
  if(itsYokeLog == NULL){
    BuildYoke();
  }
  new G4PVPlacement(nullRotationMatrix,itsYokePos,itsYokeLog,"Yoke",
		    itsMarkerLogicalVolume,false,0,BDSGlobalConstants::Instance()->GetCheckOverlaps());
}

  void BDSAwakeSpectrometer::BuildField(){}
  void BDSAwakeSpectrometer::BuildVacuumChamber(){}

void BDSAwakeSpectrometer::BuildCameraScoringPlane(){
  G4String tmp = "_cameraScoringPlane";
  _scoringPlaneName=itsName+tmp;
  int nThisSampler= BDSSampler::GetNSamplers() + 1;
  G4String ident="_camera";
  _samplerName = ("Sampler_"+BDSGlobalConstants::Instance()->StringFromInt(nThisSampler)+"_"+_scoringPlaneName);
  _samplerName2 = ("Sampler_"+BDSGlobalConstants::Instance()->StringFromInt(nThisSampler)+"_"+_scoringPlaneName+"_2");

  
  //Build and place the volume...
  itsCameraScoringPlaneSolid = new G4Box("CameraScoringPlaneSolid",100*CLHEP::mm/2.0,500*CLHEP::mm/2.0,_scoringPlaneThickness/2.0);

  itsCameraScoringPlaneLog = new G4LogicalVolume(itsCameraScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial("vacuum"),"CameraScoringPlaneLog",0,0,0);
  itsCameraScoringPlaneLog->SetVisAttributes(_visAttSampler);

  G4double dispX=_cameraScreenDist-_scoringPlaneThickness/2.0;
  G4double dispY=0;
  G4double dispZ=-_cameraScreenDist/2.0;;


  new G4PVPlacement(BDSGlobalConstants::Instance()->RotY90(),G4ThreeVector(dispX,dispY,dispZ),itsCameraScoringPlaneLog,_samplerName,
		    itsMarkerLogicalVolume,false,0,BDSGlobalConstants::Instance()->GetCheckOverlaps());
  
  itsCameraScoringPlaneLog2 = new G4LogicalVolume(itsCameraScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial("vacuum"),"CameraScoringPlaneLog2",0,0,0);
  itsCameraScoringPlaneLog2->SetVisAttributes(_visAttSampler);

  G4double dispX2=-sin(_screenAngle)*_cameraScreenDist;
  G4double dispY2=0;
  G4double dispZ2=cos(_screenAngle)*_cameraScreenDist-_cameraScreenDist/2.0;


  new G4PVPlacement(_screenRotationMatrix,G4ThreeVector(dispX2,dispY2,dispZ2),itsCameraScoringPlaneLog2,_samplerName2,
		    itsMarkerLogicalVolume,false,0,BDSGlobalConstants::Instance()->GetCheckOverlaps());
  
  
  (*LogVol)[_samplerName]=itsCameraScoringPlaneLog;
  (*LogVol)[_samplerName2]=itsCameraScoringPlaneLog2;

  itsCameraScoringPlaneLog->SetSensitiveDetector(BDSSampler::GetSensitiveDetector());
  itsCameraScoringPlaneLog2->SetSensitiveDetector(BDSSampler::GetSensitiveDetector());
  //SPM bdsOutput->nSamplers++;
  BDSSampler::AddExternalSampler(_samplerName+"_1");
  BDSSampler::AddExternalSampler(_samplerName2+"_1");

  _samplerName3 = ("Sampler_"+BDSGlobalConstants::Instance()->StringFromInt(nThisSampler)+"_"+_scoringPlaneName+"_3");
  _samplerName4 = ("Sampler_"+BDSGlobalConstants::Instance()->StringFromInt(nThisSampler)+"_"+_scoringPlaneName+"_4");

  
  //Build and place the volume...
  itsCameraScoringPlaneLog3 = new G4LogicalVolume(itsCameraScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial("vacuum"),"CameraScoringPlaneLog3",0,0,0);
  itsCameraScoringPlaneLog3->SetVisAttributes(_visAttSampler);

  G4double dispX3=_cameraScreenDist/2.0-_scoringPlaneThickness/2.0;
  G4double dispY3=0;
  G4double dispZ3=-_cameraScreenDist/2.0;;

  new G4PVPlacement(BDSGlobalConstants::Instance()->RotY90(),G4ThreeVector(dispX3,dispY3,dispZ3),itsCameraScoringPlaneLog3,_samplerName3,
		    itsMarkerLogicalVolume,false,0,BDSGlobalConstants::Instance()->GetCheckOverlaps());
  
  itsCameraScoringPlaneLog4 = new G4LogicalVolume(itsCameraScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial("vacuum"),"CameraScoringPlaneLog4",0,0,0);
  itsCameraScoringPlaneLog4->SetVisAttributes(_visAttSampler);

  G4double dispX4=-sin(_screenAngle)*_cameraScreenDist/2.0;
  G4double dispY4=0;
  G4double dispZ4=cos(_screenAngle)*_cameraScreenDist/2.0-_cameraScreenDist/2.0;


  new G4PVPlacement(_screenRotationMatrix,G4ThreeVector(dispX4,dispY4,dispZ4),itsCameraScoringPlaneLog4,_samplerName4,
		    itsMarkerLogicalVolume,false,0,BDSGlobalConstants::Instance()->GetCheckOverlaps());
  
  
  (*LogVol)[_samplerName3]=itsCameraScoringPlaneLog3;
  (*LogVol)[_samplerName4]=itsCameraScoringPlaneLog4;
  itsCameraScoringPlaneLog3->SetSensitiveDetector(BDSSampler::GetSensitiveDetector());
  itsCameraScoringPlaneLog4->SetSensitiveDetector(BDSSampler::GetSensitiveDetector());
  BDSSampler::AddExternalSampler(_samplerName3+"_1");
  BDSSampler::AddExternalSampler(_samplerName4+"_1");

  _samplerName5 = ("Sampler_"+BDSGlobalConstants::Instance()->StringFromInt(nThisSampler)+"_"+_scoringPlaneName+"_5");
  _samplerName6 = ("Sampler_"+BDSGlobalConstants::Instance()->StringFromInt(nThisSampler)+"_"+_scoringPlaneName+"_6");

  
  //Build and place the volume...
  itsCameraScoringPlaneLog5 = new G4LogicalVolume(itsCameraScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial("vacuum"),"CameraScoringPlaneLog5",0,0,0);
  itsCameraScoringPlaneLog5->SetVisAttributes(_visAttSampler);

  G4double dispX5=_cameraScreenDist/4.0-_scoringPlaneThickness/2.0;
  G4double dispY5=0;
  G4double dispZ5=-_cameraScreenDist/2.0;;

  new G4PVPlacement(BDSGlobalConstants::Instance()->RotY90(),G4ThreeVector(dispX5,dispY5,dispZ5),itsCameraScoringPlaneLog5,_samplerName5,
		    itsMarkerLogicalVolume,false,0,BDSGlobalConstants::Instance()->GetCheckOverlaps());
  
  itsCameraScoringPlaneLog6 = new G4LogicalVolume(itsCameraScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial("vacuum"),"CameraScoringPlaneLog6",0,0,0);
  itsCameraScoringPlaneLog6->SetVisAttributes(_visAttSampler);

  G4double dispX6=-sin(_screenAngle)*_cameraScreenDist/4.0;
  G4double dispY6=0;
  G4double dispZ6=cos(_screenAngle)*_cameraScreenDist/4.0-_cameraScreenDist/2.0;


  new G4PVPlacement(_screenRotationMatrix,G4ThreeVector(dispX6,dispY6,dispZ6),itsCameraScoringPlaneLog6,_samplerName6,
		    itsMarkerLogicalVolume,false,0,BDSGlobalConstants::Instance()->GetCheckOverlaps());
  
  
  (*LogVol)[_samplerName5]=itsCameraScoringPlaneLog5;
  (*LogVol)[_samplerName6]=itsCameraScoringPlaneLog6;
  itsCameraScoringPlaneLog5->SetSensitiveDetector(BDSSampler::GetSensitiveDetector());
  itsCameraScoringPlaneLog6->SetSensitiveDetector(BDSSampler::GetSensitiveDetector());
  BDSSampler::AddExternalSampler(_samplerName5+"_1");
  BDSSampler::AddExternalSampler(_samplerName6+"_1");

#ifndef NOUSERLIMITS
  G4double maxStepFactor=0.5;
  G4UserLimits* itsScoringPlaneUserLimits =  new G4UserLimits();
  itsScoringPlaneUserLimits->SetMaxAllowedStep(_scoringPlaneThickness*maxStepFactor);
  itsCameraScoringPlaneLog->SetUserLimits(itsScoringPlaneUserLimits);
#endif
}

//void BDSAwakeSpectrometer::BuildFresnelLens(){
  ////////////////////////////////////////////////////////////////////////////////////////////////////////                                                    
  /*
  G4cout << "#                                                    #" << G4endl ;
  G4cout << "#           Building the Fresnel lens ...            #" << G4endl ;
  G4cout << "#                                                    #" << G4endl ;

  G4double      LensDiameter        = 457*CLHEP::mm ; // Size of the optical active area of the lens.                                                                
  G4int      LensNumOfGrooves    = 13 ;
  //G4int      LensNumOfGrooves    = 129 ;                                                                                                                    
  //G4int      LensNumOfGrooves    = 1287 ;                                                                                                                   

  G4double      LensBorderThickness = 2.8*CLHEP::mm ;     // Thickness of the border area.                                                                           
  G4double      LensFocalLength     = 441.973*CLHEP::mm ; // This parameter depends on the lens geometry, etc !!                                                     
  G4Material   *LensMaterial        = G4Material::GetMaterial(name = "Acrylic") ;
  G4ThreeVector LensPosition        = UVscopePosition+G4ThreeVector(0.0*CLHEP::mm,0.0*CLHEP::mm,UVscopeHeight/2.0-UVscopeBaffle) ;

  UltraFresnelLens *FresnelLens = new UltraFresnelLens(LensDiameter,LensNumOfGrooves,LensMaterial,_log) ;
  */
//}


void BDSAwakeSpectrometer::BuildScreenScoringPlane(){
  G4String tmp = "_screenScoringPlane";
  _screenScoringPlaneName=itsName+tmp;
  int nThisSampler= BDSSampler::GetNSamplers() + 1;
  G4String ident="_screen";
  _screenSamplerName = ("Sampler_"+BDSGlobalConstants::Instance()->StringFromInt(nThisSampler)+"_"+_screenScoringPlaneName);
  _screenSamplerName2 = ("Sampler_"+BDSGlobalConstants::Instance()->StringFromInt(nThisSampler)+"_"+_screenScoringPlaneName+"_2");
  
  //Build and place the volume...
  itsScreenScoringPlaneSolid = new G4Box("ScreenScoringPlaneSolid",_screenWidth/2.0,_screenHeight/2.0,_scoringPlaneThickness/2.0);
  itsScreenScoringPlaneLog = new G4LogicalVolume(itsScreenScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial("vacuum"),"ScreenScoringPlaneLog",0,0,0);
  itsScreenScoringPlaneLog->SetVisAttributes(_visAttSampler);
  itsScreenScoringPlaneLog2 = new G4LogicalVolume(itsScreenScoringPlaneSolid,BDSMaterials::Instance()->GetMaterial("vacuum"),"ScreenScoringPlaneLog2",0,0,0);
  itsScreenScoringPlaneLog2->SetVisAttributes(_visAttSampler);
  G4double dispX=0;
  G4double dispY=0;
  G4double dispZ=2*std::cos(std::abs(_screenAngle))*(_screenThickness/2.0+_scoringPlaneThickness/2.0)-_cameraScreenDist/2.0;
  G4double dispZ2=-2*std::cos(std::abs(_screenAngle))*(_screenThickness/2.0+_scoringPlaneThickness/2.0)-_cameraScreenDist/2.0;
  new G4PVPlacement(_screenRotationMatrix,G4ThreeVector(dispX,dispY,dispZ),itsScreenScoringPlaneLog,_screenSamplerName,
		    itsMarkerLogicalVolume,false,0,BDSGlobalConstants::Instance()->GetCheckOverlaps());

  new G4PVPlacement(_screenRotationMatrix,G4ThreeVector(dispX,dispY,dispZ2),itsScreenScoringPlaneLog2,_screenSamplerName2,
		    itsMarkerLogicalVolume,false,0,BDSGlobalConstants::Instance()->GetCheckOverlaps());
  
  //--
  (*LogVol)[_screenSamplerName]=itsScreenScoringPlaneLog;
  
  (*LogVol)[_screenSamplerName2]=itsScreenScoringPlaneLog2;
  
  //--
  itsScreenScoringPlaneLog->SetSensitiveDetector(BDSSampler::GetSensitiveDetector());
  //-----------
  itsScreenScoringPlaneLog2->SetSensitiveDetector(BDSSampler::GetSensitiveDetector());
  //SPM bdsOutput->nSamplers++;
  //--
  BDSSampler::AddExternalSampler(_screenSamplerName+"_1");
  //----------
  BDSSampler::AddExternalSampler(_screenSamplerName2+"_1");
#ifndef NOUSERLIMITS
  G4double maxStepFactor=0.5;
  G4UserLimits* itsScoringPlaneUserLimits =  new G4UserLimits();
  itsScoringPlaneUserLimits->SetMaxAllowedStep(_scoringPlaneThickness*maxStepFactor);
  itsScreenScoringPlaneLog->SetUserLimits(itsScoringPlaneUserLimits);
#endif
}

void BDSAwakeSpectrometer::Build(){
      SetVisAttributes(); 
      BuildScreen();
      BuildCamera();	
      ComputeDimensions();
      //      BuildMagnet();
      BuildMarkerLogicalVolume();
      /*
      if(_vacChambType==2){
	BuildVacuumChamber2();
      } else {
      BuildVacuumChamber1();
      }
      */
      //      BuildScreenScoringPlane();
      //      BuildCameraScoringPlane();
      //      PlaceMagnet();
      PlaceScreen();
      //      PlaceCamera();
      //      if(BDSGlobalConstants::Instance()->GetBuildTunnel()){
      //	BuildTunnel();
      //      }
      AddSensitiveVolume(itsMarkerLogicalVolume);
      BuildFieldAndStepper();
}

void BDSAwakeSpectrometer::PlaceMagnet(){
}

void BDSAwakeSpectrometer::BuildCamera(){
  _camera=new BDSCCDCamera();
}
void BDSAwakeSpectrometer::PlaceCamera(){
  _camera->phys(new G4PVPlacement(_screenRotationMatrix,
				  G4ThreeVector(-1*_cameraScreenDist*sin(_screenAngle),0,1*_cameraScreenDist*cos(_screenAngle)),
				  _camera->log(),
				  _camera->name()+"_phys",
				  itsMarkerLogicalVolume,
				  false,
				  0,
				  true)
		);
}

void BDSAwakeSpectrometer::BuildScreen()
{
  G4cout << "Building BDSAwakeMultilayerScreen...." << G4endl;
  G4double grainSize = 10*1e-6*CLHEP::m;
  _mlScreen = new BDSAwakeMultilayerScreen(_material,_thickness, _windowScreenGap ,grainSize, _windowThickness, _windowMaterial);
  
  G4cout << "finished." << G4endl;
  //  if(BDSGlobalConstants::Instance()->GetSensitiveComponents()){
  //    for(int i=0; i<_mlScreen->nLayers(); i++){
  //      AddSensitiveVolume(_mlScreen[i].log());
  //    }
  //  } 
  G4cout << "BDSAwakeSpectrometer: finished building screen" << G4endl;
}

void BDSAwakeSpectrometer::PlaceScreen(){
  _mlScreen->place(_screenRotationMatrix,
		   G4ThreeVector(_screenCentreX,0,_screenCentreZ),
		   itsMarkerLogicalVolume
		   );
}

void BDSAwakeSpectrometer::ComputeDimensions(){
  std::cout << __METHOD_NAME__ << std::endl;
  //-------
  //Screen dimensions.
  _screenWidth=_mlScreen->size().x();
  _screenHeight=_mlScreen->size().y();
  std::cout << "... got screen dimensions... " << std::endl;
  //The scoring plane...
  _scoringPlaneThickness=1*CLHEP::um;
  _screenThickness = _mlScreen->size().z();
  _totalThickness = _screenThickness + 2*_scoringPlaneThickness;
  G4double z_wid = _screenWidth * std::sin(std::abs(_screenAngle));//Length due to the screen width and angle
  G4double z_thi = _totalThickness * std::cos(std::abs(_screenAngle));//Length due to the screen thickness
  _screen_z_dim = z_wid+z_thi;
  G4double x_wid = _screenWidth * std::cos(std::abs(_screenAngle));//Length due to the screen width and angle
  G4double x_thi = _totalThickness * std::sin(std::abs(_screenAngle));//Length due to the screen thickness
  _screen_x_dim = x_wid+x_thi;
  //Vacuum chamber dimensions.
  _vacThickness=2*CLHEP::mm;
  _vacInnerWidth=7*CLHEP::cm;
  _vacInnerHeight=7*CLHEP::cm;
  _vacHeight=_vacInnerHeight+2*_vacThickness;
  _startZPos = -itsLength/2.0;
  //Pole position
  _poleStartZ += _startZPos;
  //Screen position
  _screenEndZ += _poleStartZ;
  _screenCentreZ = _screenEndZ - _screen_z_dim/2.0;
  _screenCentreX = _screen_x_dim/2.0 + _vacInnerWidth/2.0;
  



  //  itsXLength = itsYLength = BDSGlobalConstants::Instance()->GetComponentBoxSize()/2;
  //  itsXLength = std::max(itsXLength, this->GetTunnelRadius()+2*std::abs(this->GetTunnelOffsetX()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness() + 4*BDSGlobalConstants::Instance()->GetLengthSafety() );   
  //  itsYLength = std::max(itsYLength, this->GetTunnelRadius()+2*std::abs(BDSGlobalConstants::Instance()->GetTunnelOffsetY()) + BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness()+4*BDSGlobalConstants::Instance()->GetLengthSafety() );

  _cameraScreenDist=(4.0)*CLHEP::m;
  //  _cameraScreenDist=4*213*CLHEP::mm;


  
  //  G4double thi=_totalThickness+2*_cameraScreenDist+2*_camera->size().z()+2*_scoringPlaneThickness;

  


  itsXLength = (_screen_x_dim + 2*_vacWidth1)+2*_cameraScreenDist;
  itsYLength = std::max(_screenHeight,_camera->size().y());
  itsYLength = std::max(itsYLength,50*CLHEP::cm);
  std::cout << __METHOD_NAME__ << " " << itsLength << " " << itsXLength << " " << itsYLength << std::endl;

  _vacDispZ2=(-itsLength)/2.0+(_vacWidth2)/2.0;


  _vacLength=itsLength;

  


  std::cout << __METHOD_END__ << std::endl;
}

void BDSAwakeSpectrometer::BuildMarkerLogicalVolume(){
  itsMarkerSolidVolume=new G4Box( itsName+"_marker_solid",
				  itsXLength/2.0,
				  itsYLength/2.0,
				  itsLength/2.0); //z half length 

  itsMarkerLogicalVolume=new G4LogicalVolume
    (itsMarkerSolidVolume, 
     BDSMaterials::Instance()->GetMaterial("vacuum"),
     itsName+"_marker_log");
  G4VisAttributes* visAtt = new G4VisAttributes(G4Color(0,1,0));
  visAtt->SetForceWireframe(true);
  visAtt->SetVisibility(true);
  itsMarkerLogicalVolume->SetVisAttributes(visAtt);
#ifndef NOUSERLIMITS
  G4double maxStepFactor=0.5;
  itsMarkerUserLimits =  new G4UserLimits();
  itsMarkerUserLimits->SetMaxAllowedStep(itsLength*maxStepFactor);
  itsMarkerUserLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  itsMarkerLogicalVolume->SetUserLimits(itsMarkerUserLimits);
#endif
}

void BDSAwakeSpectrometer::BuildVacuumChamber1(){


  G4VSolid* vacuumOuterSolid = new G4Box("vacuumSolid",_vacWidth1/2.0,
					 _vacHeight/2.0, 
					 _vacLength/2.0);


  G4VSolid* vacuumSolid = new G4Box("vacuumSolid",_vacInnerWidth/2.0, _vacInnerHeight/2.0, _vacLength/2.0);

  G4LogicalVolume* vacuumOuterLog = new G4LogicalVolume(vacuumOuterSolid, BDSMaterials::Instance()->GetMaterial("iron"), "vacuumOuterLog",0,0,0);

  new G4PVPlacement(_vacRotationMatrix, 
		    G4ThreeVector(_vacDispX1,0,0), 
		    vacuumOuterLog, 
		    "awakeScreenOuterVacuumPV", 
		    itsMarkerLogicalVolume, 
		    false, 
		    0,
		    BDSGlobalConstants::Instance()->GetCheckOverlaps()
		    );

  G4LogicalVolume* vacuumLog = new G4LogicalVolume(vacuumSolid, BDSMaterials::Instance()->GetMaterial("vacuum"), "vacuumLog",0,0,0);

 
  new G4PVPlacement(new G4RotationMatrix(), 
		    G4ThreeVector(_vacThickness/2.0-(_vacMylarThickness+_vacKevlarThickness)/2.0,0,0), 
		    vacuumLog, 
		    "awakeScreenVacuumPV", 
		    vacuumOuterLog, 
		    false, 
		    0,
		    BDSGlobalConstants::Instance()->GetCheckOverlaps()
		    );
 

  G4VSolid* vacuumWindowSolid = new G4Box("vacuumWindowSolid",(_vacMylarThickness+_vacKevlarThickness)/2.0, _vacInnerHeight/2.0, _vacLength/2.0);
  G4LogicalVolume* vacuumWindowLog = new G4LogicalVolume(vacuumWindowSolid, BDSMaterials::Instance()->GetMaterial("vacuum"), "vacuumWindowLog",0,0,0);

  G4VSolid* kevlarWindowSolid = new G4Box("kevlarWindowSolid",_vacKevlarThickness/2.0, _vacInnerHeight/2.0, _vacLength/2.0);
  G4LogicalVolume* kevlarWindowLog = new G4LogicalVolume(kevlarWindowSolid, BDSMaterials::Instance()->GetMaterial("G4_KEVLAR"), "kevlarWindowLog",0,0,0);

  G4VSolid* mylarWindowSolid = new G4Box("mylarWindowSolid",_vacMylarThickness/2.0, _vacInnerHeight/2.0, _vacLength/2.0);
  G4LogicalVolume* mylarWindowLog = new G4LogicalVolume(mylarWindowSolid, BDSMaterials::Instance()->GetMaterial("G4_MYLAR"), "mylarWindowLog",0,0,0);

  new G4PVPlacement(new G4RotationMatrix(), 
		    G4ThreeVector(_vacWidth1/2.0-(_vacMylarThickness+_vacKevlarThickness)/2.0,0,0), 
		    vacuumWindowLog, 
		    "awakeScreenVacuumWindowPV", 
		    vacuumOuterLog, 
		    false, 
		    0,
		    BDSGlobalConstants::Instance()->GetCheckOverlaps()
		    );
  
  new G4PVPlacement(new G4RotationMatrix(), 
		    G4ThreeVector((_vacKevlarThickness+_vacMylarThickness)/2.0 - _vacKevlarThickness/2.0,0,0), 
		    kevlarWindowLog, 
		    "awakeScreenKevlarVacuumWindowPV", 
		    vacuumWindowLog, 
		    false, 
		    0,
		    BDSGlobalConstants::Instance()->GetCheckOverlaps()
		    );

  new G4PVPlacement(new G4RotationMatrix(), 
		    G4ThreeVector(-(_vacKevlarThickness+_vacMylarThickness)/2.0 + _vacMylarThickness/2.0,0,0), 
		    mylarWindowLog, 
		    "awakeScreenMylarVacuumWindowPV", 
		    vacuumWindowLog, 
		    false, 
		    0,
		    BDSGlobalConstants::Instance()->GetCheckOverlaps()
		    );

  G4VisAttributes* vacVisAttributes=new G4VisAttributes(G4Colour(0.3,0.0,0.4,0.5));
  vacVisAttributes->SetForceWireframe(true);
  vacVisAttributes->SetVisibility(true);
  vacuumOuterLog->SetVisAttributes(vacVisAttributes);

  G4VisAttributes* winVisAttributes=new G4VisAttributes(G4Colour(1.0,0.0,0.0,0.5));
  winVisAttributes->SetForceSolid(true);
  winVisAttributes->SetVisibility(true);
  vacuumWindowLog->SetVisAttributes(winVisAttributes);



}

void BDSAwakeSpectrometer::BuildVacuumChamber2(){



  G4VSolid* vacuumOuterSolid = new G4Trap("vacuumOuterSolid",
					  _vacWidth2/2.0,
					  1.0*(-acos(0.0)+atan(2.0)),
					  0,
					  _vacThickness/2.0,
					  1e-30,1e-30,
					  0,
					  _vacThickness/2.0,
					  _vacWidth2/2.0, _vacWidth2/2.0,
					  0);

  G4VSolid* vacuumInnerSolid = new G4Trap("vacuumInnerSolid",
					  _vacWidth2/2.0,
					  1.0*(-acos(0.0)+atan(2.0)),
					  0,
					  _vacInnerHeight/2.0,
					  1e-30,1e-30,
					  0,
					  _vacInnerHeight/2.0,
					  _vacWidth2/2.0, _vacWidth2/2.0,
					  0);


  G4LogicalVolume* vacuumOuterLog = new G4LogicalVolume(vacuumOuterSolid, BDSMaterials::Instance()->GetMaterial("iron"), "vacuumOuterLog",0,0,0);

  G4LogicalVolume* vacuumInnerLog = new G4LogicalVolume(vacuumInnerSolid, BDSMaterials::Instance()->GetMaterial("vacuum"), "vacuumInnerLog",0,0,0);

  _vacRotationMatrix->rotateY(CLHEP::pi);

  new G4PVPlacement(_vacRotationMatrix, 
		    G4ThreeVector(_vacDispX2,(_vacHeight+_vacThickness)/2.0,_vacDispZ2), 
		    vacuumOuterLog, 
		    "awakeScreenUpperVacuumPV", 
		    itsMarkerLogicalVolume, 
		    false, 
		    0,
		    BDSGlobalConstants::Instance()->GetCheckOverlaps()
		    );

  new G4PVPlacement(_vacRotationMatrix, 
		    G4ThreeVector(_vacDispX2,-(_vacHeight+_vacThickness)/2.0,_vacDispZ2), 
		    vacuumOuterLog, 
		    "awakeScreenLowerVacuumPV", 
		    itsMarkerLogicalVolume, 
		    false, 
		    0,
		    BDSGlobalConstants::Instance()->GetCheckOverlaps()
		    );

  new G4PVPlacement(_vacRotationMatrix, 
		    G4ThreeVector(_vacDispX2,0,_vacDispZ2), 
		    vacuumInnerLog, 
		    "awakeScreenInnerVacuumPV", 
		    itsMarkerLogicalVolume, 
		    false, 
		    0,
		    BDSGlobalConstants::Instance()->GetCheckOverlaps()
		    );


  G4VisAttributes* vacVisAttributes=new G4VisAttributes(G4Colour(0.3,0.0,0.4,0.5));
  vacVisAttributes->SetForceWireframe(true);
  vacVisAttributes->SetVisibility(true);
  vacuumOuterLog->SetVisAttributes(vacVisAttributes);
  vacuumInnerLog->SetVisAttributes(vacVisAttributes);

}


BDSAwakeSpectrometer::~BDSAwakeSpectrometer()
{
  delete _mlScreen;
  delete _camera;
  delete _vacRotationMatrix;
}
