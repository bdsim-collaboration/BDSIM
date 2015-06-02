#include "BDSSpectrVacChamb.hh"
#include "BDSMaterials.hh"
#include "BDSGlobalConstants.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "BDSDebug.hh"

BDSSpectrVacChamb::BDSSpectrVacChamb(){}
BDSSpectrVacChamb::~BDSSpectrVacChamb(){}

BDSSpectrVacChamb::BDSSpectrVacChamb(const G4String &name, G4double lengthZ, G4double magStartZ, G4double vacuumEndZ, G4double screenWidth, G4double screenAngle, G4double sizeX, G4double sizeY, G4double thickness):_name(name),_vacuumEndZ(vacuumEndZ),_magStartZ(magStartZ),_lengthZ(lengthZ){
  SetParameters(name, lengthZ, magStartZ, vacuumEndZ, screenWidth, screenAngle, sizeX, sizeY, thickness);
  Build();
  
  /*
  _innerLogVol = new G4LogicalVolume(_innerSolid,
				     BDSMaterials::Instance()->GetMaterial("vacuum"),
				     _name+"_inner_log");
  */
  

  /*  
  G4double screenAngle2=shapeOuter->screenAngle2();
  G4double deltaMagStart=std::abs(thickness*cos(screenAngle2));
  G4double deltaScreenEnd=-std::abs(thickness*cos(screenAngle));
  BDSSpectrVacChambShape* shapeInner = new BDSSpectrVacChambShape(_name+"_inner", lengthZ, magStartZ+deltaMagStart, vacuumEndZ+deltaScreenEnd, screenWidth, screenAngle, sizeX, sizeY, thickness);
  */


}

void BDSSpectrVacChamb::Build(){
  BuildBox();
  BuildTrap();
  BuildBoxInner();
  BuildTrapInner();
}

void BDSSpectrVacChamb::Place(G4LogicalVolume* motherVolume){

  new G4PVPlacement(_trapRotation,
		    _trapTranslation,
		    _logVolTrap,
		    _name+"_physi_trap",
		    motherVolume,
		    false,
		    0,
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());
  

  new G4PVPlacement(_trapRotationInner,
		    _trapTranslationInner,
		    _innerLogVolTrap,
		    _name+"_physi_trap_inner",
		    _logVolTrap,
		    false,
		    0,
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());

  new G4PVPlacement(_boxRotation,
		   _boxTranslation,
		   _logVolBox,
		   _name+"_physi_box",
		    motherVolume,
		    false,
		    0,
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());

  new G4PVPlacement(_boxRotationInner,
		   _boxTranslationInner,
		   _innerLogVolBox,
		   _name+"_physi_box_inner",
		    _logVolBox,
		    false,
		    0,
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());
}

void BDSSpectrVacChamb::SetParameters(const G4String &name, G4double lengthZ, G4double magStartZ, G4double vacuumEndZ, G4double screenWidth, G4double screenAngle, G4double sizeX, G4double sizeY, G4double thickness){
  _thickness=thickness;
  _name=name;
  _lengthZ=lengthZ;
  _magStartZ = magStartZ;
  _vacuumEndZ = vacuumEndZ;
  _screenWidth = screenWidth;
  _screenAngle = screenAngle;
  _sizeX = sizeX;
  _sizeY = sizeY;
  CalculateGeometry();
}

void BDSSpectrVacChamb::BuildBox(){
  _boxSolid = new G4Box(_name+"_boxSolid", 
			(_sizeX+2*_thickness)/2.0,
			(_sizeY+2*_thickness)/2.0,
			_lengthZ/2.0);

  _logVolBox = new G4LogicalVolume(_boxSolid,
				    BDSMaterials::Instance()->GetMaterial("G4_Fe"),
				    _name+"_log");
  
  G4VisAttributes* BoxVisAtt = new G4VisAttributes(G4Color(1.0,0.0,0.0,0.5));
  BoxVisAtt->SetForceSolid(true);
  BoxVisAtt->SetVisibility(true);
  _logVolBox->SetVisAttributes(BoxVisAtt);
}

void BDSSpectrVacChamb::BuildBoxInner(){
  _boxSolidInner = new G4Box(_name+"_boxSolidInner", 
			     _sizeX/2.0 + _thickness/2.0,
			     _sizeY/2.0,
			     _lengthZ/2.0);
  
  _innerLogVolBox = new G4LogicalVolume(_boxSolidInner,
					BDSMaterials::Instance()->GetMaterial("vacuum"),
					_name+"_log");
  
  G4VisAttributes* BoxInnerVisAtt = new G4VisAttributes(G4Color(0.0,1.0,0.0,0.5));
  BoxInnerVisAtt->SetForceSolid(true);
  BoxInnerVisAtt->SetVisibility(true);
  _innerLogVolBox->SetVisAttributes(BoxInnerVisAtt);
}


void BDSSpectrVacChamb::BuildTrap(){
  _trapSolid = new G4GenericTrap(_name+"_trapSolid",
				 _trapLengthX/2.0, //Half length of the trapezoid component.
				 _trapVertices
				 );

  _logVolTrap = new G4LogicalVolume(_trapSolid,
				    BDSMaterials::Instance()->GetMaterial("G4_Fe"),
				    _name+"_trap_log");
  
  G4VisAttributes* LogVolVisAtt = new G4VisAttributes(G4Color(1.0,0.0,0.0,0.4));
  LogVolVisAtt->SetForceSolid(true);
  LogVolVisAtt->SetVisibility(true);
  _logVolTrap->SetVisAttributes(LogVolVisAtt);
}

void BDSSpectrVacChamb::BuildTrapInner(){
  _trapSolidInner = new G4GenericTrap(_name+"_trapSolidInner",
				 _trapLengthXIn/2.0, //Half length of the trapezoid component.
				 _trapVerticesInner
				 );

  _innerLogVolTrap = new G4LogicalVolume(_trapSolidInner,
				    BDSMaterials::Instance()->GetMaterial("vacuum"),
				    _name+"_trap_inner_log");
  
  G4VisAttributes* TrapInnerVisAtt = new G4VisAttributes(G4Color(0.0,0.0,1.0,0.3));
  TrapInnerVisAtt->SetForceSolid(true);
  TrapInnerVisAtt->SetVisibility(true);
  _innerLogVolTrap->SetVisAttributes(TrapInnerVisAtt);
}

void BDSSpectrVacChamb::CalculateGeometry(){
  _trapLengthZ=_vacuumEndZ-_magStartZ;
  G4double trapLengthZ2=std::abs(_screenWidth*cos(_screenAngle));
  _trapLengthX=std::abs(_screenWidth*sin(_screenAngle));
  
  G4double y1=-_sizeY/2.0-_thickness;
  G4double y2= _sizeY/2.0+_thickness;

  G4double x1=-_trapLengthZ/2.0;
  G4double x3=x1+_trapLengthZ;
  G4double x2=x3-trapLengthZ2;

  _trapVertices.push_back(G4TwoVector(x1,y1));
  _trapVertices.push_back(G4TwoVector(x1,y2));
  _trapVertices.push_back(G4TwoVector(x3,y2));
  _trapVertices.push_back(G4TwoVector(x3,y1));
  _trapVertices.push_back(G4TwoVector(x2,y1));
  _trapVertices.push_back(G4TwoVector(x2,y2));
  _trapVertices.push_back(G4TwoVector(x2,y2));
  _trapVertices.push_back(G4TwoVector(x2,y1));

  //old
  //  _trapTranslation.setZ((-_sizeX-_trapLengthX)/2.0);
  //  _trapTranslation.setX(-_lengthZ/2.0+_magStartZ+_trapLengthZ/2.0);


  //  G4double sizeX=70*CLHEP::mm;
  //  G4double trapLengthX=(sqrt(2)/2.0)*CLHEP::m;
  //  G4double trapLengthZ=_vacuumEndZ-_magStartZ;
  _trapTranslation.setX(  (_trapLengthX+_sizeX)/2.0 + _thickness);
  G4double localMagStartZ = _magStartZ + _lengthZ/2.0;
  _trapTranslation.setZ(-_lengthZ/2.0 + _trapLengthZ/2.0 + localMagStartZ );



  _trapRotation = new G4RotationMatrix();
  _trapRotation->rotateX(CLHEP::pi);
  _trapRotation->rotateY(-CLHEP::pi/2.0);

  //  _trapRotation->rotateY(-CLHEP::pi/4.0);
  
  //  _trapRotation->rotateY(_screenAngle);
  
 
  


  _screenAngle2=atan(_trapLengthX/(x2-x1));

  /*
  G4double deltaZ1=0;//_thickness*std::abs(cos(_screenAngle));
  G4double deltaZ2=_thickness*std::abs(cos(_screenAngle2));
  G4double deltaX1=_thickness*std::abs(sin(_screenAngle))+_thickness*std::abs(cos(_screenAngle2));
  */


  G4double deltaZ1=0;//_thickness*std::abs(cos(_screenAngle));
  G4double deltaZ2=_thickness*std::abs(cos(_screenAngle2));
  G4double deltaX1=_thickness*std::abs(cos(_screenAngle2));
    //deltaZ2*std::abs(sin(_screenAngle));

  //old
  //  _trapLengthZIn=_trapLengthZ-deltaZ1-deltaZ2;
  //  G4double trapLengthZ2In=trapLengthZ2-deltaZ1;
  //  _trapLengthXIn=_trapLengthX-deltaX1 + _thickness;

  _trapLengthZIn=_trapLengthZ-deltaZ2;
  _trapLengthXIn=_trapLengthX-deltaX1;
  

  G4double y1In=-_sizeY/2.0;
  G4double y2In= _sizeY/2.0;

  G4double x1In=x1+deltaZ2;
  G4double x3In=x3-deltaZ2;
  //  G4double x2In=x3In-std::abs((_screenWidth-_thickness)*cos(_screenAngle))+deltaZ2;
  G4double screenLength2 = x2-x1;
  G4double x2In=x2;//+std::abs(cos(_screenAngle))*_thickness;


  _trapVerticesInner.push_back(G4TwoVector(x1In,y1In));
  _trapVerticesInner.push_back(G4TwoVector(x1In,y2In));
  _trapVerticesInner.push_back(G4TwoVector(x3In,y2In));
  _trapVerticesInner.push_back(G4TwoVector(x3In,y1In));
  _trapVerticesInner.push_back(G4TwoVector(x2In,y1In));
  _trapVerticesInner.push_back(G4TwoVector(x2In,y2In));
  _trapVerticesInner.push_back(G4TwoVector(x2In,y2In));
  _trapVerticesInner.push_back(G4TwoVector(x2In,y1In));

  

  _trapTranslationInner.setZ( -deltaX1/2.0  );
  _trapTranslationInner.setX( deltaZ2 );

  _trapRotationInner = new G4RotationMatrix();
  //  _trapRotationInner=_trapRotation;

  printGeom();

  _boxRotation=new G4RotationMatrix();
  _boxRotationInner=new G4RotationMatrix();
  _boxTranslationInner.setX(_thickness/2.0);
}

void BDSSpectrVacChamb::printGeom(){
  G4cout << __METHOD_NAME__ << G4endl;
  printTrapVertices(_trapVertices, "_trapVertices");
  printTrapVertices(_trapVerticesInner, "_trapVerticesInner");
}

void BDSSpectrVacChamb::printTrapVertices(std::vector<G4TwoVector> vertices, const G4String& name){
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout << name << ":" << G4endl;
  G4cout << "(n,  x,  y)" << G4endl;
  int n=0;
  for(std::vector<G4TwoVector>::const_iterator vertIt = vertices.begin();
      vertIt!=vertices.end();
      vertIt++){
    G4cout << "(" << n << ", " << (*vertIt).x() << ", " << (*vertIt).y() << ")" << G4endl;
    n++;
  }
}




