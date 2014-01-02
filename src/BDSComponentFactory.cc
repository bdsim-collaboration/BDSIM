#include "BDSComponentFactory.hh"
#include "BDSExecOptions.hh"
// elements
//#include "BDSBeamPipe.hh"
#include "BDSDrift.hh"
#include "BDSPCLDrift.hh"
#include "BDSSectorBend.hh"
#include "BDSRBend.hh"
#include "BDSKicker.hh"
#include "BDSQuadrupole.hh"
#include "BDSSextupole.hh"
//#include "BDSSkewSextupole.hh"
#include "BDSOctupole.hh"
#include "BDSDecapole.hh"
#include "BDSTMultipole.hh"
#include "BDSRfCavity.hh"
#include "BDSSolenoid.hh"
#include "BDSSampler.hh"
#include "BDSSamplerCylinder.hh"
#include "BDSDump.hh"
#include "BDSLaserWire.hh"
#include "BDSLWCalorimeter.hh"
#include "BDSMuSpoiler.hh"
#include "BDSTransform3D.hh"
#include "BDSElement.hh"
#include "BDSComponentOffset.hh"
#include "BDSCollimator.hh"
//#include "BDSRealisticCollimator.hh"
#include "BDSScintillatorScreen.hh"
#include "BDSAwakeScintillatorScreen.hh"

extern G4bool outline;

<<<<<<< HEAD
=======


>>>>>>> processFlags
#ifdef DEBUG
bool debug1 = true;
#else
bool debug1 = false;
#endif

BDSComponentFactory::BDSComponentFactory(){
  verbose = BDSExecOptions::Instance()->GetVerbose();
  //
  // compute magnetic rigidity brho
  // formula: B(Tesla)*rho(m) = p(GeV)/(0.299792458 * |charge(e)|)
  //
  // charge (in |e| units)
  _charge = BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGCharge();
  // momentum (in GeV/c)
  _momentum = BDSGlobalConstants::Instance()->GetBeamMomentum()/CLHEP::GeV;
  // rigidity (in T*m)
  _brho = BDSGlobalConstants::Instance()->GetFFact()*( _momentum / (0.299792458 * _charge));
  
  // rigidity (in Geant4 units)
  _brho *= (CLHEP::tesla*CLHEP::m);

  if (verbose || debug1) G4cout << "Rigidity (Brho) : "<< fabs(_brho)/(CLHEP::tesla*CLHEP::m) << " T*m"<<G4endl;
  //
  // beampipe default outer radius (if not overridden by "aper" option)
  //
  _bpRad=BDSGlobalConstants::Instance()->GetBeampipeRadius();
  if (verbose || debug1) G4cout<<"Default pipe outer radius= "<<_bpRad/CLHEP::m<< "m"
			      << G4endl;

  // I suspect FeRad is planned to be offered as an option for the inner radius
  // of the iron in case it is different from the beampipe outer radius
  // Not done yet.
  _FeRad = _bpRad;
  if (verbose || debug1) G4cout<<"Default magnet inner radius= "<<_FeRad/CLHEP::m<< "m"
			      << G4endl;

   // stuff for rescaling due to synchrotron radiation, IGNORING
  _synch_factor = 1;
  //
  _driftStartAper = _bpRad;
  _driftEndAper = _bpRad;
}

BDSComponentFactory::~BDSComponentFactory(){
}

BDSAcceleratorComponent* BDSComponentFactory::createComponent(std::list<struct Element>::iterator elementIter, std::list<struct Element>& beamline_list){
#ifdef DEBUG
  G4cout << "BDSComponentFactory::createComponent() making iterators" << G4endl;  
#endif
  _elementIter = elementIter;
  _previousElementIter = elementIter; 
  _nextElementIter= elementIter; 
  if(_elementIter != beamline_list.begin()){
#ifdef DEBUG
    G4cout << "BDSComponentFactory::createComponent() moving to previous element" << G4endl;  
#endif
    _previousElementIter--;
  } 

  _nextElementIter++;
  if(_nextElementIter == beamline_list.end()){
#ifdef DEBUG
    G4cout << "BDSComponentFactory::createComponent() at the end, not moving to next element" << G4endl;  
#endif
    _nextElementIter--;
  } 
#ifdef DEBUG
  G4cout << "BDSComponentFactory::createComponent() creating and returning component..." << G4endl;  
#endif
  return createComponent(*_elementIter, *_previousElementIter, *_nextElementIter);
}

									 
BDSAcceleratorComponent* BDSComponentFactory::createComponent(Element aElement, Element previousElement, Element nextElement){
#ifdef DEBUG
  G4cout << "BDSComponentFactory::createComponent() creating element..." << G4endl;  
#endif
  _element = aElement;
#ifdef DEBUG
  G4cout << "BDSComponentFactory::createComponent() creating previous element..." << G4endl;  
#endif
  _previousElement = previousElement;  
#ifdef DEBUG
  G4cout << "BDSComponentFactory::createComponent() creating next element..." << G4endl;  
#endif
  _nextElement = nextElement;
  return createComponent();
}

BDSAcceleratorComponent* BDSComponentFactory::createComponent(){
#ifdef DEBUG
  G4cout << "BDSComponentFactory::createComponent() element name = " << _element.name << G4endl;  
#endif
  switch(_element.type){
  case _SAMPLER:
#ifdef DEBUG
    G4cout << "BDSComponentFactory  - creating sampler" << G4endl;
#endif
    return createSampler(); break;
  case _DRIFT:
#ifdef DEBUG
    G4cout << "BDSComponentFactory  - creating drift" << G4endl;
#endif
    return createDrift(); break; 
  case _PCLDRIFT:
#ifdef DEBUG
    G4cout << "BDSComponentFactory  - creating pcl drift" << G4endl;
#endif
    return createPCLDrift(); break; 
  case _RF:
#ifdef DEBUG
    G4cout << "BDSComponentFactory  - creating rf" << G4endl;
#endif
    return createRF(); break; 
  case _SBEND:
#ifdef DEBUG
    G4cout << "BDSComponentFactory  - creating sbend" << G4endl;
#endif
    return createSBend(); break; 
  case _RBEND:
#ifdef DEBUG
    G4cout << "BDSComponentFactory  - creating rbend" << G4endl;
#endif
    return createRBend(); break; 
  case _HKICK:
#ifdef DEBUG
    G4cout << "BDSComponentFactory  - creating hkick" << G4endl;
#endif
    return createHKick(); break; 
  case _VKICK:
#ifdef DEBUG
    G4cout << "BDSComponentFactory  - creating vkick" << G4endl;
#endif
    return createVKick(); break; 
  case _QUAD:
#ifdef DEBUG
    G4cout << "BDSComponentFactory  - creating quadrupole" << G4endl;
#endif
    return createQuad(); break; 
  case _SEXTUPOLE:
#ifdef DEBUG
    G4cout << "BDSComponentFactory  - creating sextupole" << G4endl;
#endif
    return createSextupole(); break; 
  case _OCTUPOLE:
#ifdef DEBUG
    G4cout << "BDSComponentFactory  - creating octupole" << G4endl;
#endif
    return createOctupole(); break; 
  case _MULT:
#ifdef DEBUG
    G4cout << "BDSComponentFactory  - creating multipole" << G4endl;
#endif
    return createMultipole(); break; 
  case _ELEMENT:    
#ifdef DEBUG
    G4cout << "BDSComponentFactory  - creating element" << G4endl;
#endif
    return createElement(); break; 
  case _CSAMPLER:
#ifdef DEBUG
    G4cout << "BDSComponentFactory  - creating csampler" << G4endl;
#endif
    return createCSampler(); break; 
  case _DUMP:
#ifdef DEBUG
    G4cout << "BDSComponentFactory  - creating dump" << G4endl;
#endif
    return createDump(); break; 
  case _SOLENOID:
#ifdef DEBUG
    G4cout << "BDSComponentFactory  - creating solenoid" << G4endl;
#endif
    return createSolenoid(); break; 
  case _ECOL:
#ifdef DEBUG
    G4cout << "BDSComponentFactory  - creating ecol" << G4endl;
#endif
    return createECol(); break; 
  case _RCOL:
#ifdef DEBUG
    G4cout << "BDSComponentFactory  - creating rcol" << G4endl;
#endif
    return createRCol(); break; 
  case _MUSPOILER:    
#ifdef DEBUG
    G4cout << "BDSComponentFactory  - creating muspoiler" << G4endl;
#endif
    return createMuSpoiler(); break; 
  case _LASER:
#ifdef DEBUG
    G4cout << "BDSComponentFactory  - creating laser" << G4endl;
#endif
    return createLaser(); break; 
  case _SCREEN:
#ifdef DEBUG
    G4cout << "BDSComponentFactory  - creating screen" << G4endl;
#endif
    return createScreen(); break; 
  case _AWAKESCREEN:
#ifdef DEBUG
    G4cout << "BDSComponentFactory  - creating awake screen" << G4endl;
#endif
    return createAwakeScreen(); break; 
  case _TRANSFORM3D:
#ifdef DEBUG
    G4cout << "BDSComponentFactory  - creating transform3d" << G4endl;
#endif
    return createTransform3D(); break;  
  default:
#ifdef DEBUG
    G4cout << "BDSComponentFactory: type: " << _element.type << G4endl; 
#endif
    //    G4Exception("Error: BDSComponentFactory: type not found.", "-1", FatalErrorInArgument, "");   
    return NULL;
    break;
  }
  
}

BDSAcceleratorComponent* BDSComponentFactory::createSampler(){
  return (new BDSSampler(_element.name, BDSGlobalConstants::Instance()->GetSamplerLength()));
}

BDSAcceleratorComponent* BDSComponentFactory::createCSampler(){
  if( _element.l < 1.E-4 ) _element.l = 1.0 ;
  return (new BDSSamplerCylinder( _element.name,
						  _element.l * CLHEP::m,
						  _element.r * CLHEP::m ));
}

BDSAcceleratorComponent* BDSComponentFactory::createDump(){
  return (new BDSDump( _element.name,
				       BDSGlobalConstants::Instance()->GetSamplerLength(),_element.tunnelMaterial ));
}

BDSAcceleratorComponent* BDSComponentFactory::createDrift(){
  double aper(0), aperX(0), aperY(0);
  _element.phiAngleIn=0;

  if(_element.l < BDSGlobalConstants::Instance()->GetLengthSafety()) // skip too short elements
    {
      G4cerr << "---->NOT creating Drift,"
             << " name= " << _element.name
             << ", TOO SHORT LENGTH:"
             << " l= " << _element.l << "m"
             << G4endl;
      return NULL;
    }

  if( _element.aper > 0 ) aper = _element.aper * CLHEP::m; //Set if aper specified for element
  if( _element.aperX > 0 ) aperX = _element.aperX * CLHEP::m; //Set if aperX specified for element
  if( _element.aperY > 0 ) aperY = _element.aperY * CLHEP::m; //Set if aperY specified for element
  if( (aperX>0) || (aperY>0)){  //aperX or aperY override aper, aper set to the largest of aperX or aperY
    aper=std::max(_element.aperX,_element.aperY);
  }
  
  if ( (aperX !=0) || (aperY != 0) || (aper != 0) || _element.phiAngleIn != 0 || _element.phiAngleOut !=0){
    if (aperX==0 && aperY==0 && aper==0){
      aperX=BDSGlobalConstants::Instance()->GetBeampipeRadius()/CLHEP::m;
      aperY=BDSGlobalConstants::Instance()->GetBeampipeRadius()/CLHEP::m;
      aper=BDSGlobalConstants::Instance()->GetBeampipeRadius()/CLHEP::m;
    }
    
#ifdef DEBUG
    G4cout << "---->creating Drift,"
	   << " name= " << _element.name
	   << " l= " << _element.l << "m"
	   << " aperX= " << aperX << "m"
	   << " aperY= " << aperY << "m"
	   << " aper = " << aper << "m"
	   << " phiAngleIn= " << _element.phiAngleIn 
	   << " phiAngleOut= " << _element.phiAngleOut 
	   << G4endl;
#endif
	
    //Create the phiAngleIn using BDSTransform3D
    
    G4bool aperset=true;
    if(!(_element.tunnelOffsetX)<1e6){
      return (new BDSDrift( _element.name,
			    _element.l*CLHEP::m,
			    _element.blmLocZ,
			    _element.blmLocTheta,
			    aperX, aperY, _element.tunnelMaterial, aperset, aper, BDSGlobalConstants::Instance()->GetTunnelOffsetX(), _element.phiAngleIn, _element.phiAngleOut));
    } else {
      return (new BDSDrift( _element.name,
			    _element.l*CLHEP::m,
			    _element.blmLocZ,
			    _element.blmLocTheta,
			    aperX, aperY, _element.tunnelMaterial, aperset, aper,_element.tunnelOffsetX*CLHEP::m, _element.phiAngleIn, _element.phiAngleOut) );
    }
  } else {
    //Taper circular beam pipe between elements.                                                                                                                        
    _driftStartAper = _bpRad;
    _driftEndAper = _bpRad;
    if((_previousElement.type!=_ECOL)&&(_previousElement.type!=_RCOL)&&(_previousElement.type!=_MUSPOILER)){
      if( _previousElement.aper > 1.e-10*CLHEP::m ) _driftStartAper = _previousElement.aper * CLHEP::m;
    }
    if((_nextElement.type!=_ECOL)&&(_nextElement.type!=_RCOL)&&(_nextElement.type!=_MUSPOILER)){
      if( _nextElement.aper > 1.e-10*CLHEP::m ) _driftEndAper = _nextElement.aper * CLHEP::m;
    }

#ifdef DEBUG
    G4cout << "---->creating Drift,"
	   << " name= " << _element.name
	   << " l= " << _element.l << "m"
	   << " startAper= " << _bpRad/CLHEP::m << "m"
	   << " endAper= " << _bpRad/CLHEP::m << "m"
	   << G4endl;
#endif
    if(!(_element.tunnelOffsetX<1e6)){
      return (new BDSDrift( _element.name,
			    _element.l*CLHEP::m,
			    _element.blmLocZ,
			    _element.blmLocTheta,
			    _driftStartAper, _driftEndAper, _element.tunnelMaterial, false));
    } else {
      return (new BDSDrift( _element.name,
			    _element.l*CLHEP::m,
			    _element.blmLocZ,
			    _element.blmLocTheta,
			    _driftStartAper, _driftEndAper, _element.tunnelMaterial, false, 0, _element.tunnelOffsetX ) );
    }
  }
  G4cerr << "NOT creating drift..." << G4endl;
  return NULL;
}

BDSAcceleratorComponent* BDSComponentFactory::createPCLDrift(){
  G4double aper=0;
  //Check all apertures are set.
  if (_element.aperY>BDSGlobalConstants::Instance()->GetLengthSafety()){
    _element.aperYUp = _element.aperY;
    _element.aperYDown = _element.aperY;
  }
  
  if (_element.aperX<BDSGlobalConstants::Instance()->GetLengthSafety()){
    G4cerr << "Error: BDSDetectorConstruction.cc, in building PCLDrift, aperX = " << _element.aperX << " is less than lengthSafety." << G4endl;
    exit(1);
  } 
  if (_element.aperYUp<BDSGlobalConstants::Instance()->GetLengthSafety()){
    G4cerr << "Error: BDSDetectorConstruction.cc, in building PCLDrift, aperYUp = " << _element.aperYUp << " is less than lengthSafety." << G4endl;
    exit(1);
  } 
  if (_element.aperYDown<BDSGlobalConstants::Instance()->GetLengthSafety()){
    G4cerr << "Error: BDSDetectorConstruction.cc, in building PCLDrift, aperYDown = " << _element.aperYDown << " is less than lengthSafety." << G4endl;
    exit(1);
  } 
  
  if( (_element.aperX>0) || (_element.aperY>0)){  //aperX or aperY override aper, aper set to the largest of aperX or aperY
    aper=std::max(_element.aperX,_element.aperYUp+_element.aperDy);
    aper=std::max(aper,_element.aperYDown+_element.aperDy);
  }
  
  if(_element.l > BDSGlobalConstants::Instance()->GetLengthSafety()) // skip too short elements                                                                                                         
    {
      
      if(!(_element.tunnelOffsetX<1e6)){
	return (new BDSPCLDrift( _element.name,
					       _element.l*CLHEP::m,
					       _element.blmLocZ,
					       _element.blmLocTheta,
					       _element.aperX*CLHEP::m, _element.aperYUp*CLHEP::m, _element.aperYDown*CLHEP::m,_element.aperDy*CLHEP::m, _element.tunnelMaterial, aper, _element.tunnelRadius*CLHEP::m));
      } else {
	return (new BDSPCLDrift( _element.name,
					       _element.l*CLHEP::m,
					       _element.blmLocZ,
					       _element.blmLocTheta,
					       _element.aperX*CLHEP::m, _element.aperYUp*CLHEP::m, _element.aperYDown*CLHEP::m,_element.aperDy*CLHEP::m, _element.tunnelMaterial, aper, _element.tunnelRadius*CLHEP::m, _element.tunnelOffsetX*CLHEP::m));
      }
    } else {
    G4cerr << "Element too short!" << G4endl;
    return NULL;
  }
}

BDSAcceleratorComponent* BDSComponentFactory::createRF(){
  G4double aper = _bpRad;
  if( _element.aper > 1.e-10*CLHEP::m ) aper = _element.aper * CLHEP::m;
  
  if(_element.l > 0) // skip zero-length elements
    {
      return (new BDSRfCavity( _element.name,
					     _element.l * CLHEP::m,
					     aper,
					     _element.gradient,
					     _element.tunnelMaterial,
					     _element.material ) );
    } else {
    G4cerr << "---->NOT creating RF,"
	   << " name= " << _element.name
	   << ", TOO SHORT LENGTH:"
	   << " l= " << _element.l << "m"
	   << G4endl;
    return NULL;
  }
}

BDSAcceleratorComponent* BDSComponentFactory::createSBend(){
  //
  // geometry
  //
  G4double aper = _bpRad;
  if( _element.aper > 0 ) aper = _element.aper * CLHEP::m; //Set if aper specified for element
  if( (_element.aperX>0) || (_element.aperY>0)){  //aperX or aperY override aper, aper set to the largest of aperX or aperY
    aper=std::max(_element.aperX,_element.aperY);
  }
  _FeRad = aper;
  
  if( _element.outR < aper/CLHEP::m)
    {
#ifdef DEBUG
      G4cout << _element.name << ": outer radius smaller than aperture: "
	     << "aper= "<<aper/CLHEP::m<<"m outR= "<<_element.outR<<"m"<<G4endl;
      G4cout << _element.name << ": setting outer radius to default = "
	     << BDSGlobalConstants::Instance()->GetComponentBoxSize()/(2*CLHEP::m)<< "m" << G4endl;
#endif
      _element.outR = BDSGlobalConstants::Instance()->GetComponentBoxSize()/(2*CLHEP::m);
    }
  
  // arc length
  G4double length = _element.l*CLHEP::m;
  G4double magFieldLength = length;
  
  //
  // magnetic field
  //
  
  // MAD conventions:
  // 1) a positive bend angle represents a bend to the right, i.e.
  // towards negative x values (even for negative _charges??)
  // 2) a positive K1 = 1/|Brho| dBy/dx means horizontal focusing of
  // positive charges
  // CHECK SIGNS 
  //
  
  if( fabs(_element.angle) < 1.e-7 * CLHEP::rad ) {
    _element.angle=1e-7 * CLHEP::rad;
  }
  
  if(_element.B != 0){
    _bField = _element.B * CLHEP::tesla;
    G4double rho = _brho/_bField;
    _element.angle  = - 2.0*asin(magFieldLength/2.0/rho);
  }
  else{
    _element.angle *= -1;
    _bField = - 2 * _brho * sin(_element.angle/2.0) / magFieldLength;
    _element.B = _bField/CLHEP::tesla;
  }
  
  // synch factor??
  // B' = dBy/dx = Brho * (1/Brho dBy/dx) = Brho * k1
  // Brho is already in G4 units, but k1 is not -> multiply k1 by m^-2
  _bPrime = - _brho * (_element.k1 / CLHEP::m2) * _synch_factor;
  //Should keep the correct geometry, therefore keep dipole with zero angle.
  if( fabs(_element.angle) < 1.e-7 * CLHEP::rad ) { // not possible due to check earlier - JS
    return createDrift();
    //return (new BDSDrift( _element.name,
    //					_element.l*CLHEP::m, _element.blmLocZ, _element.blmLocTheta,
    //					aper, aper, _element.tunnelMaterial ) );
  }
  else {
    /*
      return (new BDSRBend( _element.name,
      _element.l*CLHEP::m,
      aper,
      _FeRad,
      _bField,
      _element.angle,
      _element.outR * CLHEP::m,
      _element.blmLocZ,
      _element.blmLocTheta,
      _element.tilt * CLHEP::rad,
      _bPrime,
      _element.material ) );
      
    */
    return (new BDSSectorBend( _element.name,
					     length,
					     aper,
					     _FeRad,
					     _bField,
					     _element.angle,
					     _element.outR * CLHEP::m,
					     _element.blmLocZ,
					     _element.blmLocTheta,
					     _element.tilt,
					     _bPrime,
					     _element.tunnelMaterial,
					     _element.material, _element.aperX*CLHEP::m, _element.aperY*CLHEP::m ) );
  }
}

BDSAcceleratorComponent* BDSComponentFactory::createRBend(){
  //
  // geometry
  //
  G4double aper = 2*_bpRad;
  if( _element.aper > 1.e-10*CLHEP::m ) aper = _element.aper * CLHEP::m;
  _FeRad = aper;
  
  if( _element.outR < aper/CLHEP::m)
    {
#ifdef DEBUG
      G4cout << _element.name << ": outer radius smaller than aperture: "
	     << "aper= "<<aper/CLHEP::m<<"m outR= "<<_element.outR<<"m"<<G4endl;
      G4cout << _element.name << ": setting outer radius to default = "
	     << BDSGlobalConstants::Instance()->GetComponentBoxSize()/(2*CLHEP::m)<< "m" << G4endl;
#endif
      _element.outR = BDSGlobalConstants::Instance()->GetComponentBoxSize()/(2*CLHEP::m);
    }
  
  G4double length = _element.l*CLHEP::m; //geometrical length
  G4double magFieldLength = 2*std::min ( //length of magnetic field
					((_element.l/_element.angle)*sin(_element.angle/2)
					 - fabs(cos(_element.angle/2))*_element.outR*tan(_element.angle/2)/2), 
					((_element.l/_element.angle)*sin(_element.angle/2)
					 + fabs(cos(_element.angle/2))*_element.outR*tan(_element.angle/2)/2)
					)*CLHEP::m;
  
  //
  // magnetic field
  //
  
  // CHECK SIGNS OF B, B', ANGLE
  if(_element.B != 0){
    // angle = arc length/radius of curvature = L/rho = (B*L)/(B*rho)
    _bField = _element.B * CLHEP::tesla;
    G4double rho = _brho/_bField;
    //_element.angle  = - _bField * length / brho;
    _element.angle  = - 2.0*asin(length/2.0/rho);
  }
  else{
    _element.angle *= -1;
    // arc length = radius*angle
    //            = (geometrical length/(2.0*sin(angle/2))*angle
    G4double arclength = 0.5*magFieldLength * _element.angle / sin(_element.angle/2.0);
    // B = Brho/rho = Brho/(arc length/angle)
    _bField = - _brho * _element.angle / arclength;
    _element.B = _bField/CLHEP::tesla;
  }
  
  // synch factor???
  
  // B' = dBy/dx = Brho * (1/Brho dBy/dx) = Brho * k1
  // Brho is already in G4 units, but k1 is not -> multiply k1 by m^-2
  _bPrime = - _brho * (_element.k1 / CLHEP::m2) * _synch_factor;
  
  if( fabs(_element.angle) < 1.e-7 * CLHEP::rad ) {
    return createDrift();
    // return (new BDSDrift( _element.name,
    // 					length,
    // 					_element.blmLocZ,
    // 					_element.blmLocTheta,
    // 					aper, aper, _element.tunnelMaterial ) );
  }
  else {
    return (new BDSRBend( _element.name,
					length,
					aper,
					_FeRad,
					_bField,
					_element.angle,
					_element.outR * CLHEP::m,
					_element.blmLocZ,
					_element.blmLocTheta,
					_element.tilt * CLHEP::rad,
					_bPrime,
					_element.tunnelMaterial,
					_element.material ) );
  }
}

BDSAcceleratorComponent* BDSComponentFactory::createHKick(){
  //
  // geometry
  //
  G4double aper = _bpRad;
  if( _element.aper > 1.e-10*CLHEP::m ) aper = _element.aper * CLHEP::m;
  _FeRad = aper;
  
  if( _element.outR < aper/CLHEP::m)
    {
#ifdef DEBUG
      G4cout << _element.name << ": outer radius smaller than aperture: "
	     << "aper= "<<aper/CLHEP::m<<"m outR= "<<_element.outR<<"m"<<G4endl;
      G4cout << _element.name << ": setting outer radius to default = "
	     << BDSGlobalConstants::Instance()->GetComponentBoxSize()/(2*CLHEP::m)<< "m" << G4endl;
#endif
      _element.outR = BDSGlobalConstants::Instance()->GetComponentBoxSize()/(2*CLHEP::m);
    }
  
  G4double length = _element.l*CLHEP::m;
  //
  // magnetic field
  //
  if(_element.B != 0){
    // angle = arc length/radius of curvature = L/rho = (B*L)/(B*rho)
    _bField = _element.B * CLHEP::tesla;
    _element.angle  = -_bField * length / _brho;
  }
  else{
    // B = Brho/rho = Brho/(arc length/angle)
    _bField = - _brho * _element.angle / length;
    _element.B = _bField/CLHEP::tesla;
  }
  
  // synch factor??
  // B' = dBy/dx = Brho * (1/Brho dBy/dx) = Brho * k1
  // Brho is already in G4 units, but k1 is not -> multiply k1 by m^-2
  _bPrime = - _brho * (_element.k1 / CLHEP::m2) * _synch_factor;
  
  if( fabs(_element.angle) < 1.e-7 * CLHEP::rad ) {
    G4cerr << "---->NOT creating Hkick,"
	   << " name= " << _element.name
	   << ", TOO SMALL ANGLE"
	   << " angle= " << _element.angle << "rad"
	   << ": REPLACED WITH Drift,"
	   << " l= " << length/CLHEP::m << "m"
	   << " aper= " << aper/CLHEP::m << "m"
	   << " tunnel material " << _element.tunnelMaterial
	   << G4endl;
    return createDrift();
    // return (new BDSDrift( _element.name,
    // 					length,
    // 					_element.blmLocZ,
    // 					_element.blmLocTheta,
    // 					aper, aper, _element.tunnelMaterial ) );
  } 
  else {
    return (new BDSKicker( _element.name,
					 length,
					 aper,
					 _FeRad,
					 _bField,
					 _element.angle,
					 _element.outR * CLHEP::m,
					 _element.tilt * CLHEP::rad,
					 _bPrime,
					 _element.tunnelMaterial,
					 _element.material ) );
  }
}

BDSAcceleratorComponent* BDSComponentFactory::createVKick(){
  //
  // geometry
  //
  G4double aper = _bpRad;
  if( _element.aper > 1.e-10*CLHEP::m ) aper = _element.aper * CLHEP::m;
  G4double _FeRad = aper;
  
  if( _element.outR < aper/CLHEP::m)
    {
#ifdef DEBUG
      G4cout << _element.name << ": outer radius smaller than aperture: "
	     << "aper= "<<aper/CLHEP::m<<"m outR= "<<_element.outR<<"m"<<G4endl;
      G4cout << _element.name << ": setting outer radius to default = "
	     << BDSGlobalConstants::Instance()->GetComponentBoxSize()/(2*CLHEP::m)<< "m" << G4endl;
#endif
      _element.outR = BDSGlobalConstants::Instance()->GetComponentBoxSize()/(2*CLHEP::m);
    }
  G4double length = _element.l*CLHEP::m;
  //
  // magnetic field
  //
  if(_element.B != 0){
    // angle = arc length/radius of curvature = L/rho = (B*L)/(B*rho)
    _bField = _element.B * CLHEP::tesla;
    _element.angle  = -_bField * length / _brho;
  }
  else{
    // B = Brho/rho = Brho/(arc length/angle)
    _bField = - _brho * _element.angle / length;
    _element.B = _bField/CLHEP::tesla;
  }
  // synch factor???
  // B' = dBy/dx = Brho * (1/Brho dBy/dx) = Brho * k1
  // Brho is already in G4 units, but k1 is not -> multiply k1 by m^-2
  _bPrime = - _brho * (_element.k1 / CLHEP::m2) * _synch_factor;
  
  if( fabs(_element.angle) < 1.e-7 * CLHEP::rad ) {
    G4cerr << "---->NOT creating Vkick,"
	   << " name= " << _element.name
	   << ", TOO SMALL ANGLE"
	   << " angle= " << _element.angle << "rad"
	   << ": REPLACED WITH Drift,"
	   << " l= " << _element.l << "m"
	   << " aper= " << aper/CLHEP::m << "m"
	   << " tunnel material " << _element.tunnelMaterial
	   << G4endl;

    return createDrift();
    // return (new BDSDrift( _element.name,
    // 					_element.l * CLHEP::m,
    // 					_element.blmLocZ,
    // 					_element.blmLocTheta,
    //                                            aper, aper, _element.tunnelMaterial ) );
  } 
  
  return (new BDSKicker( _element.name,
			 _element.l * CLHEP::m,
			 aper,
			 _FeRad,
			 _bField,
			 _element.angle,
			 _element.outR * CLHEP::m,
			 (_element.tilt+CLHEP::pi/2)*CLHEP::rad,
			 _bPrime,
			 _element.tunnelMaterial,
			 _element.material ) );
}

BDSAcceleratorComponent* BDSComponentFactory::createQuad(){
  //
  // geometry
  //
  G4double aper = _bpRad;
  if( _element.aper > 1.e-10*CLHEP::m ) aper = _element.aper * CLHEP::m;
  _FeRad = aper;
  if( _element.outR < aper/CLHEP::m)
    {
#ifdef DEBUG
      G4cout << _element.name << ": outer radius smaller than aperture: "
	     << "aper= "<<aper/CLHEP::m<<"m outR= "<<_element.outR<<"m"<<G4endl;
      G4cout << _element.name << ": setting outer radius to default = "
	     << BDSGlobalConstants::Instance()->GetComponentBoxSize()/(2*CLHEP::m)<< "m" << G4endl;
#endif
      _element.outR = BDSGlobalConstants::Instance()->GetComponentBoxSize()/(2*CLHEP::m);
    }
  
	//
	// magnetic field
	//
	// B' = dBy/dx = Brho * (1/Brho dBy/dx) = Brho * k1
	// Brho is already in G4 units, but k1 is not -> multiply k1 by m^-2
  _bPrime = - _brho * (_element.k1 / CLHEP::m2) * _synch_factor;
  
        return (new BDSQuadrupole( _element.name,
						 _element.l * CLHEP::m,
						 aper,
						 _FeRad,
						 _bPrime, 
						 _element.tilt * CLHEP::rad,
                                                 _element.outR * CLHEP::m, 
                                                 _element.blmLocZ,
                                                 _element.blmLocTheta,
                                                 _element.tunnelMaterial,
						 _element.material,
						 _element.spec ) );
	
  }  
  
BDSAcceleratorComponent* BDSComponentFactory::createSextupole(){
	//
	// geometry
	//
	G4double aper = _bpRad;
        if( _element.aper > 1.e-10*CLHEP::m ) aper = _element.aper * CLHEP::m;
	_FeRad = aper;

	if( _element.outR < aper/CLHEP::m)
	  {
#ifdef DEBUG
	    G4cout << _element.name << ": outer radius smaller than aperture: "
		   << "aper= "<<aper/CLHEP::m<<"m outR= "<<_element.outR<<"m"<<G4endl;
	    G4cout << _element.name << ": setting outer radius to default = "
		   << BDSGlobalConstants::Instance()->GetComponentBoxSize()/(2*CLHEP::m)<< "m" << G4endl;
#endif
	    _element.outR = BDSGlobalConstants::Instance()->GetComponentBoxSize()/(2*CLHEP::m);
	  }

	//
	// magnetic field
	//

	// B'' = d^2By/dx^2 = Brho * (1/Brho d^2By/dx^2) = Brho * k2
	// brho is in Geant4 units, but k2 is not -> multiply k2 by m^-3
	_bDoublePrime = - _brho * (_element.k2 / CLHEP::m3) * _synch_factor;

#ifdef DEBUG 
        G4cout << "---->creating Sextupole,"
               << " name= " << _element.name
               << " l= " << _element.l << "m"
               << " k2= " << _element.k2 << "m^-3"
               << " brho= " << fabs(_brho)/(CLHEP::tesla*CLHEP::m) << "T*m"
               << " B''= " << _bDoublePrime/(CLHEP::tesla/CLHEP::m2) << "T/m^2"
               << " tilt= " << _element.tilt << "rad"
               << " aper= " << aper/CLHEP::m << "m"
               << " outR= " << _element.outR << "m"
               << " FeRad= " << _FeRad/CLHEP::m << "m"
               << " tunnel material " << _element.tunnelMaterial
               << " material= " << _element.material
               << G4endl;
#endif

	 return (new BDSSextupole( _element.name,
						_element.l * CLHEP::m,
						aper,
						_FeRad,
						_bDoublePrime,
						_element.tilt * CLHEP::rad,
						_element.outR * CLHEP::m,
                                                 _element.blmLocZ,
                                                 _element.blmLocTheta,
                                                 _element.tunnelMaterial,
						_element.material ) );
      
}

BDSAcceleratorComponent* BDSComponentFactory::createOctupole(){

	//
	// geometry
	//
	G4double aper = _bpRad;
	if( _element.aper > 1.e-10*CLHEP::m ) aper = _element.aper * CLHEP::m;
	_FeRad = aper;

	if( _element.outR < aper/CLHEP::m)
	  {
#ifdef DEBUG
	    G4cout << _element.name << ": outer radius smaller than aperture: "
		   << "aper= "<<aper/CLHEP::m<<"m outR= "<<_element.outR<<"m"<<G4endl;
	    G4cout << _element.name << ": setting outer radius to default = "
		   << BDSGlobalConstants::Instance()->GetComponentBoxSize()/(2*CLHEP::m)<< "m" << G4endl;
#endif
	    _element.outR = BDSGlobalConstants::Instance()->GetComponentBoxSize()/(2*CLHEP::m);
	  }

	//
	// magnetic field
	//

	// B''' = d^3By/dx^3 = Brho * (1/Brho d^3By/dx^3) = Brho * k3
	// brho is in Geant4 units, but k3 is not -> multiply k3 by m^-4
	_bTriplePrime = - _brho * (_element.k3 / (CLHEP::m3*CLHEP::m)) * _synch_factor;

#ifdef DEBUG 
        G4cout << "---->creating Octupole,"
               << " name= " << _element.name
               << " l= " << _element.l << "m"
               << " k3= " << _element.k3 << "m^-4"
               << " brho= " << fabs(_brho)/(CLHEP::tesla*CLHEP::m) << "T*m"
               << " B'''= " << _bTriplePrime/(CLHEP::tesla/CLHEP::m3) << "T/m^3"
               << " tilt= " << _element.tilt << "rad"
               << " aper= " << aper/CLHEP::m << "m"
               << " outR= " << _element.outR << "m"
               << " FeRad= " << _FeRad/CLHEP::m << "m"
               << " tunnel material " << _element.tunnelMaterial
               << " material= " << _element.material
               << G4endl;
#endif

	return (new BDSOctupole( _element.name,
					       _element.l * CLHEP::m,
					       aper,
					       _FeRad,
					       _bTriplePrime,
					       _element.tilt * CLHEP::rad,
					       _element.outR * CLHEP::m,
					       _element.blmLocZ,
					       _element.blmLocTheta,
                                                _element.tunnelMaterial,
					       _element.material ) );
      
}

BDSAcceleratorComponent* BDSComponentFactory::createMultipole(){
  
  //
  // geometry
  //
  G4double aper = _bpRad;
  if( _element.aper > 1.e-10*CLHEP::m ) aper = _element.aper * CLHEP::m;
  
  _FeRad = aper;
  
  if( _element.outR < aper/CLHEP::m)
    {
#ifdef DEBUG
      G4cout << _element.name << ": outer radius smaller than aperture: "
	     << "aper= "<<aper/CLHEP::m<<"m outR= "<<_element.outR<<"m"<<G4endl;
      G4cout << _element.name << ": setting outer radius to default = "
	     << BDSGlobalConstants::Instance()->GetComponentBoxSize()/(2*CLHEP::m)<< "m" << G4endl;
#endif
      _element.outR = BDSGlobalConstants::Instance()->GetComponentBoxSize()/(2*CLHEP::m);
    }
  
#ifdef DEBUG 
  G4cout << "---->creating Multipole,"
	 << " name= " << _element.name
	 << " l= " << _element.l << "m"
	 << " tilt= " << _element.tilt << "rad"
	 << " aper= " << aper/CLHEP::m << "m"
	 << " outR= " << _element.outR << "m"
	 << " FeRad= " << _FeRad/CLHEP::m << "m"
	 << " tunnel material " << _element.tunnelMaterial
	 << " material= " << _element.material
	 << G4endl;
#endif
  
  //
  // magnetic field
  //
  std::list<double>::iterator kit;
  
#ifdef DEBUG 
  G4cout << " knl={ ";
#endif
  for(kit=(_element.knl).begin();kit!=(_element.knl).end();kit++)
    {
#ifdef DEBUG 
      G4cout<<(*kit)<<", ";
#endif
      (*kit) /= _element.l; 
    }
#ifdef DEBUG 
  G4cout << "}";
#endif
  
#ifdef DEBUG 
  G4cout << " ksl={ ";
#endif
  for(kit=(_element.ksl).begin();kit!=(_element.ksl).end();kit++)
    {
#ifdef DEBUG 
      G4cout<<(*kit)<<" ";
#endif
      (*kit) /= _element.l; 
    }
#ifdef DEBUG 
  G4cout << "}" << G4endl;
#endif
  
  return (new BDSTMultipole( _element.name,
					   _element.l * CLHEP::m,
					   aper,
					   _FeRad,
					   _element.tilt * CLHEP::rad,
					   _element.outR * CLHEP::m,
					   _element.knl,
					   _element.ksl,
					   _element.blmLocZ,
					   _element.blmLocTheta,
					   _element.tunnelMaterial, 
					   _element.material 
					   ) );
  
}

BDSAcceleratorComponent* BDSComponentFactory::createElement(){

	//
	// geometry
	//
        G4double aper = _bpRad;
        if( _element.aper > 1.e-10*CLHEP::m ) aper = _element.aper * CLHEP::m;
        
/* Fix for element volume overlaps - do not set default outR!
	if( _element.outR < aper/CLHEP::m)
	  {
#ifdef DEBUG
	    G4cout << _element.name << ": outer radius smaller than aperture: "
		   << "aper= "<<aper/CLHEP::m<<"m outR= "<<_element.outR<<"m"<<G4endl;
	    G4cout << _element.name << ": setting outer radius to default = "
		   << "aper+22*cm"<<G4endl;
#endif
	    _element.outR = 0.22;
	  }
*/
#ifdef DEBUG 
        G4cout << "---->creating Element,"
               << " name= " << _element.name
               << " l= " << _element.l << "m"
               << " aper= " << aper/CLHEP::m << "m"
               << " outR= " << _element.outR << "m"
               << " tunnel material " << _element.tunnelMaterial
               << " tunnel cavity material " << _element.tunnelCavityMaterial
               << " precision region " << _element.precisionRegion
               << " field z offset " << _element.fieldZOffset
               << G4endl;
#endif

	if(_element.tunnelOffsetX<1e6){
	  
	  return (new BDSElement( _element.name,
						_element.geometryFile,
						_element.bmapFile,
						_element.l * CLHEP::m,
						aper,
<<<<<<< HEAD
						_element.outR * CLHEP::m , _element.tunnelMaterial, _element.tunnelRadius, _element.tunnelOffsetX, _element.tunnelCavityMaterial, _element.precisionRegion ));
=======
				  _element.outR * m , _element.tunnelMaterial, _element.tunnelRadius, _element.tunnelOffsetX, _element.tunnelCavityMaterial, _element.precisionRegion,_element.fieldZOffset ));
>>>>>>> processFlags
	} 
	else {
	  return (new BDSElement( _element.name,
						_element.geometryFile,
						_element.bmapFile,
						_element.l * CLHEP::m,
						aper,
<<<<<<< HEAD
						_element.outR * CLHEP::m , _element.tunnelMaterial, _element.tunnelRadius, (G4double)0, _element.tunnelCavityMaterial, _element.precisionRegion));
=======
				  _element.outR * m , _element.tunnelMaterial, _element.tunnelRadius, (G4double)0, _element.tunnelCavityMaterial, _element.precisionRegion, _element.fieldZOffset));
>>>>>>> processFlags
	}
	

}

  BDSAcceleratorComponent* BDSComponentFactory::createSolenoid(){

	//
	// geometry
	//
        G4double aper = _bpRad;
        if( _element.aper > 1.e-10*CLHEP::m ) aper = _element.aper * CLHEP::m;
        
	_FeRad = aper;

	if( _element.outR < aper/CLHEP::m)
	  {
#ifdef DEBUG
	    G4cout << _element.name << ": outer radius smaller than aperture: "
		   << "aper= "<<aper/CLHEP::m<<"m outR= "<<_element.outR<<"m"<<G4endl;
	    G4cout << _element.name << ": setting outer radius to default = "
		   << BDSGlobalConstants::Instance()->GetComponentBoxSize()/(2*CLHEP::m)<< "m" << G4endl;
#endif
	    _element.outR = BDSGlobalConstants::Instance()->GetComponentBoxSize()/(2*CLHEP::m);
	  }

	//
	// magnetic field
	//
	// B = B/Brho * Brho = ks * Brho
	// brho is in Geant4 units, but ks is not -> multiply ks by m^-1
	G4double _bField;
        if(_element.B != 0){
          _bField = _element.B * CLHEP::tesla;
          _element.ks  = (_bField/_brho) / CLHEP::m;
        }
        else{
	  _bField = (_element.ks/CLHEP::m) * _brho;
	  _element.B = _bField/CLHEP::tesla;
        }

#ifdef DEBUG 
        G4cout << "---->creating Solenoid,"
               << " name= " << _element.name
               << " l= " << _element.l << "m"
               << " ks= " << _element.ks << "m^-1"
               << " brho= " << fabs(_brho)/(CLHEP::tesla*CLHEP::m) << "T*m"
               << " B= " << _bField/CLHEP::tesla << "T"
               << " aper= " << aper/CLHEP::m << "m"
               << " outR= " << _element.outR << "m"
               << " FeRad= " << _FeRad/CLHEP::m << "m"
               << " tunnel material " << _element.tunnelMaterial
               << " material= " << _element.material
               << G4endl;
#endif
	 return (new BDSSolenoid( _element.name,
					       _element.l * CLHEP::m,
					       aper,
					       _FeRad,
					       _bField,
					       _element.outR*CLHEP::m,
                                                _element.blmLocZ,
                                                _element.blmLocTheta,
                                                _element.tunnelMaterial,
                                                _element.material
                                                ) );
	
}

  BDSAcceleratorComponent* BDSComponentFactory::createECol(){

	G4Material* theMaterial;
	if(_element.material != "")
	  theMaterial = BDSMaterials::Instance()->GetMaterial( _element.material );
	else
	  theMaterial = BDSMaterials::Instance()->GetMaterial( "Graphite" );

#ifdef DEBUG 
        G4cout << "---->creating Ecol,"
               << " name= " << _element.name 
               << " xaper= " << _element.xsize <<"m"
               << " yaper= " << _element.ysize <<"m"
               << " material= " << _element.material
               << " tunnel material " << _element.tunnelMaterial
               << G4endl;
#endif

	return (new BDSCollimator(_element.name,
						_element.l * CLHEP::m,
						_bpRad,
						_element.xsize * CLHEP::m,
						_element.ysize * CLHEP::m,
						_ECOL,
						theMaterial,
						_element.outR*CLHEP::m,
						_element.blmLocZ,
						_element.blmLocTheta,
						_element.tunnelMaterial) );
	
  }


  BDSAcceleratorComponent* BDSComponentFactory::createRCol(){

	G4Material* theMaterial;
	if(_element.material != "")
	  theMaterial = BDSMaterials::Instance()->GetMaterial( _element.material );
	else
	  theMaterial = BDSMaterials::Instance()->GetMaterial( "Graphite" );

#ifdef DEBUG 
        G4cout << "---->creating Rcol,"
               << " name= " << _element.name 
               << " xaper= " << _element.xsize <<"m"
               << " yaper= " << _element.ysize <<"m"
               << " flatl= " << _element.flatlength <<"m"
               << " taper= " << _element.taperlength <<"m"
               << " material= " << _element.material
               << " tunnel material " << _element.tunnelMaterial
               << G4endl;
#endif
/*
	return (new BDSRealisticCollimator(
						_element.name,
						_bpRad,
						_element.xsize * CLHEP::m,
						_element.ysize * CLHEP::m,
						_RCOL,
						_element.flatlength * CLHEP::m,
						_element.taperlength * CLHEP::m,
						theMaterial,
						_element.outR*CLHEP::m) );

*/
        return (new BDSCollimator( _element.name,
                                                  _element.l * CLHEP::m,
                                                  _bpRad,
                                                  _element.xsize * CLHEP::m,
                                                  _element.ysize * CLHEP::m,
						 _RCOL,
                                                  theMaterial,
                                                  _element.outR*CLHEP::m,
                                                 _element.blmLocZ,
                                                 _element.blmLocTheta,
                                                  _element.tunnelMaterial) );
      
  }


  BDSAcceleratorComponent* BDSComponentFactory::createMuSpoiler(){

#ifdef DEBUG 
        G4cout << "---->creating muspoiler,"
               << " name= " << _element.name 
               << " length= " << _element.l
               << " B= " << _element.B
               << " tunnel material " << _element.tunnelMaterial
               << G4endl;
#endif
        G4String name = _element.name;
        G4double length = _element.l*CLHEP::m;
        G4double _bField = _element.B * CLHEP::tesla;
        G4double beamPipeRadius;
        //        if(_element.aperSet){
        beamPipeRadius = _element.aper*CLHEP::m;
          //        } else {
          //          beamPipeRadius = BDSGlobalConstants::Instance()->GetBeampipeRadius();
          //        }
        G4double innerRadius;
        //        if (_element.inRset){
        innerRadius = _element.inR*CLHEP::m;
        //        } else {
        //          innerRadius = beamPipeRadius;
        //        }
        G4double outerRadius = _element.outR*CLHEP::m;
        
#ifdef DEBUG
        G4cout << "BDSMuSpoiler: " << name << " " << length/CLHEP::m << " " << outerRadius/CLHEP::m << " " << innerRadius/CLHEP::m << " " << _bField/CLHEP::tesla << " " << beamPipeRadius/CLHEP::m << G4endl;
#endif

        return (new BDSMuSpoiler(name,
                                               length,
                                               beamPipeRadius,
                                               innerRadius,
                                               outerRadius,
                                               _bField, 
                                               _element.blmLocZ,
                                               _element.blmLocTheta,
                                               _element.tunnelMaterial));
      
  }

BDSAcceleratorComponent* BDSComponentFactory::createLaser(){
	if(_element.l == 0) _element.l = 1e-8;
	
#ifdef DEBUG 
        G4cout << "---->creating Laser,"
               << " name= "<< _element.name
               << " l=" << _element.l <<"m"
               << " lambda= " << _element.waveLength << "m"
               << " xSigma= " << _element.xsize << "m"
               << " ySigma= " << _element.ysize << "m"
               << " xdir= " << _element.xdir
               << " ydir= " << _element.ydir
               << " zdir= " << _element.zdir
               << G4endl;
#endif

        G4double length = _element.l*CLHEP::m;
        G4double lambda = _element.waveLength*CLHEP::m;

	
	G4ThreeVector direction = 
	  G4ThreeVector(_element.xdir,_element.ydir,_element.zdir);
	G4ThreeVector position  = G4ThreeVector(0,0,0);
	
	 return (new BDSLaserWire( _element.name,
						length,
						lambda,
						direction) );
	
}

BDSAcceleratorComponent* BDSComponentFactory::createScreen(){
  if(_element.l == 0) _element.l = 1e-8;
	
#ifdef DEBUG 
        G4cout << "---->creating Screen,"
               << " name= "<< _element.name
               << " l=" << _element.l/CLHEP::m<<"m"
               << " tscint=" << _element.tscint/CLHEP::m<<"m"
               << " angle=" << _element.angle/CLHEP::rad<<"rad"
               << " scintmaterial=" << "ups923a"//_element.scintmaterial
               << " airmaterial=" << "vacuum"//_element.airmaterial
               << G4endl;
#endif
	return (new BDSScintillatorScreen( _element.name, _element.tscint*CLHEP::m, (_element.angle-0.78539816339)*CLHEP::rad, "ups923a","vacuum")); //Name, scintillator thickness, angle in radians (relative to -45 degrees)
}


BDSAcceleratorComponent* BDSComponentFactory::createAwakeScreen(){
	
#ifdef DEBUG 
        G4cout << "---->creating Awake Screen,"
               << G4endl;
#endif
	return (new BDSAwakeScintillatorScreen(_element.name, _element.scintmaterial, _element.tscint)); //Name
}

BDSAcceleratorComponent* BDSComponentFactory::createTransform3D(){
	
#ifdef DEBUG 
        G4cout << "---->creating Transform3d,"
               << " name= " << _element.name
               << " xdir= " << _element.xdir/CLHEP::m << "m"
               << " ydir= " << _element.ydir/CLHEP::m << "m"
               << " zdir= " << _element.zdir/CLHEP::m << "m"
               << " phi= " << _element.phi/CLHEP::rad << "rad"
               << " theta= " << _element.theta/CLHEP::rad << "rad"
               << " psi= " << _element.psi/CLHEP::rad << "rad"
               << G4endl;
#endif
	
	 return (new BDSTransform3D( _element.name,
						  _element.xdir *CLHEP::m,
						  _element.ydir *CLHEP::m,
						  _element.zdir *CLHEP::m,
						  _element.phi *CLHEP::rad,
						  _element.theta *CLHEP::rad,
						  _element.psi *CLHEP::rad ) );
	
      
}


