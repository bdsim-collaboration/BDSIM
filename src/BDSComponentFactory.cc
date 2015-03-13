#include "BDSComponentFactory.hh"
#include "BDSExecOptions.hh"
// elements
#include "BDSDrift.hh"
#include "BDSSectorBend.hh"
#include "BDSRBend.hh"
#include "BDSKicker.hh"
#include "BDSQuadrupole.hh"
#include "BDSSextupole.hh"
#include "BDSOctupole.hh"
#include "BDSTMultipole.hh"
#include "BDSRfCavity.hh"
#include "BDSSolenoid.hh"
#include "BDSSampler.hh"
#include "BDSSamplerCylinder.hh"
#include "BDSDump.hh"
#include "BDSLaserWire.hh"
#include "BDSLine.hh"
#include "BDSMuSpoiler.hh"
#include "BDSTransform3D.hh"
#include "BDSElement.hh"
#include "BDSCollimator.hh"
//#include "BDSRealisticCollimator.hh"
#include "BDSScintillatorScreen.hh"
#include "BDSAwakeScintillatorScreen.hh"
#include "BDSTerminator.hh"
#include "BDSTeleporter.hh"
#include "BDSBeamline.hh" //needed to calculate offset at end for teleporter
#include "BDSBeamPipeType.hh"
#include "BDSBeamPipeInfo.hh"

#include "parser/enums.h"
#include "parser/elementlist.h"

#include "BDSDebug.hh"

#include <cmath>
#include <sstream>
#include <string>

#ifdef BDSDEBUG
bool debug1 = true;
#else
bool debug1 = false;
#endif

BDSComponentFactory::BDSComponentFactory(){
  verbose = BDSExecOptions::Instance()->GetVerbose();
  lengthSafety = BDSGlobalConstants::Instance()->GetLengthSafety();
  //
  // compute magnetic rigidity brho
  // formula: B(Tesla)*rho(m) = p(GeV)/(0.299792458 * |charge(e)|)
  //
  // charge (in e units)
  _charge = BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGCharge();
  // momentum (in GeV/c)

  _momentum = BDSGlobalConstants::Instance()->GetBeamMomentum()/CLHEP::GeV;
  // rigidity (in T*m)
  _brho = BDSGlobalConstants::Instance()->GetFFact()*( _momentum / 0.299792458);
  
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
  _bpThick = BDSGlobalConstants::Instance()->GetBeampipeThickness();
  //  _FeRad = _bpRad + _bpThick; //Needs to be the outer beam pipe radius - add the beam pipe thickness.
  // if (verbose || debug1) G4cout<<"Default magnet inner radius= "<<_FeRad/CLHEP::m<< "m"
  // 			      << G4endl;

  _driftStartAper = _bpRad;
  _driftEndAper = _bpRad;
}

BDSComponentFactory::~BDSComponentFactory(){
}

BDSAcceleratorComponent* BDSComponentFactory::createComponent(std::list<struct Element>::iterator elementIter, ElementList& beamline_list){
#ifdef BDSDEBUG
  G4cout << "BDSComponentFactory::createComponent() making iterators" << G4endl;  
#endif
  _elementIter = elementIter;
  _previousElementIter = elementIter; 
  _nextElementIter= elementIter; 
  if(_elementIter != beamline_list.begin()){
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory::createComponent() moving to previous element" << G4endl;  
#endif
    _previousElementIter--;
  } 

  _nextElementIter++;
  if(_nextElementIter == beamline_list.end()){
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory::createComponent() at the end, not moving to next element" << G4endl;  
#endif
    _nextElementIter--;
  } 
#ifdef BDSDEBUG
  G4cout << "BDSComponentFactory::createComponent() creating and returning component..." << G4endl;  
#endif
  return createComponent(*_elementIter, *_previousElementIter, *_nextElementIter);
}

BDSAcceleratorComponent* BDSComponentFactory::createComponent(Element& aElement, Element& previousElement, Element& nextElement){
#ifdef BDSDEBUG
  G4cout << "BDSComponentFactory::createComponent() creating element..." << G4endl;  
#endif
  _element = aElement;
#ifdef BDSDEBUG
  G4cout << "BDSComponentFactory::createComponent() creating previous element..." << G4endl;  
#endif
  _previousElement = previousElement;  
#ifdef BDSDEBUG
  G4cout << "BDSComponentFactory::createComponent() creating next element..." << G4endl;  
#endif
  _nextElement = nextElement;
  return createComponent();
}

BDSAcceleratorComponent* BDSComponentFactory::createComponent(){
#ifdef BDSDEBUG
  G4cout << "BDSComponentFactory::createComponent() element name = " << _element.name << G4endl;  
#endif
  BDSAcceleratorComponent* element = NULL;

  switch(_element.type){
  case _SAMPLER:
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory  - creating sampler" << G4endl;
#endif
    element = createSampler(); break;
  case _DRIFT:
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory  - creating drift" << G4endl;
#endif
    element = createDrift(); break; 
  case _RF:
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory  - creating rf" << G4endl;
#endif
    element = createRF(); break; 
  case _SBEND:
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory  - creating sbend" << G4endl;
#endif
    element = createSBend(); break; 
  case _RBEND:
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory  - creating rbend" << G4endl;
#endif
    element = createRBend(); break; 
  case _HKICK:
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory  - creating hkick" << G4endl;
#endif
    element = createHKick(); break; 
  case _VKICK:
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory  - creating vkick" << G4endl;
#endif
    element = createVKick(); break; 
  case _QUAD:
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory  - creating quadrupole" << G4endl;
#endif
    element = createQuad(); break; 
  case _SEXTUPOLE:
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory  - creating sextupole" << G4endl;
#endif
    element = createSextupole(); break; 
  case _OCTUPOLE:
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory  - creating octupole" << G4endl;
#endif
    element = createOctupole(); break; 
  case _MULT:
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory  - creating multipole" << G4endl;
#endif
    element = createMultipole(); break; 
  case _ELEMENT:    
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory  - creating element" << G4endl;
#endif
    element = createElement(); break; 
  case _CSAMPLER:
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory  - creating csampler" << G4endl;
#endif
    element = createCSampler(); break; 
  case _DUMP:
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory  - creating dump" << G4endl;
#endif
    element = createDump(); break; 
  case _SOLENOID:
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory  - creating solenoid" << G4endl;
#endif
    element = createSolenoid(); break; 
  case _ECOL:
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory  - creating ecol" << G4endl;
#endif
    element = createCollimator(); break; 
  case _RCOL:
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory  - creating rcol" << G4endl;
#endif
    element = createCollimator(); break; 
  case _MUSPOILER:    
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory  - creating muspoiler" << G4endl;
#endif
    element = createMuSpoiler(); break; 
  case _LASER:
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory  - creating laser" << G4endl;
#endif
    element = createLaser(); break; 
  case _SCREEN:
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory  - creating screen" << G4endl;
#endif
    element = createScreen(); break; 
  case _AWAKESCREEN:
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory  - creating awake screen" << G4endl;
#endif
    element = createAwakeScreen(); break; 
  case _TRANSFORM3D:
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory  - creating transform3d" << G4endl;
#endif
    element = createTransform3D(); break;
  case _TELEPORTER:
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory  - creating teleporter" << G4endl;
#endif
    element = createTeleporter(); break;
  case _TERMINATOR:
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory  - creating terminator" << G4endl;
#endif
    element = createTerminator(); break;

    // common types, but nothing to do here
  case _MARKER:
  case _LINE:
  case _REV_LINE:
  case _MATERIAL:
  case _ATOM:
  case _SEQUENCE:
  case _GAS:
  case _TUNNEL:
  case _COLLIMATOR:
    element = NULL;
    break;
  default:
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory: type: " << _element.type << G4endl; 
#endif
    G4Exception("Error: BDSComponentFactory: type not found.", "-1", FatalErrorInArgument, "");   
    exit(1);
    break;
  }

  // add common properties and build geometry
  if (element) {
    // check if element is divided into multiple parts 
    if (element->GetType() == "line") {
      BDSLine* line = dynamic_cast<BDSLine*>(element);
      if (line) {
	//line of components to be added individually
	for (BDSLine::BDSLineIterator i = line->begin(); i != line->end(); ++i) {
	  addCommonProperties(*i);
	  (*i)->Initialise();
	}
      }
    } else {
      addCommonProperties(element);
      element->Initialise();
    }
  }

  return element;
}

void BDSComponentFactory::addCommonProperties(BDSAcceleratorComponent* component) {
  component->SetPrecisionRegion(_element.precisionRegion);
  component->SetType(typestr(_element.type));

  //For the optics file...
  component->SetK1(_element.k1);
  component->SetK2(_element.k2);
  component->SetK3(_element.k3);
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

BDSAcceleratorComponent* BDSComponentFactory::createTeleporter(){
  // This relies on things being added to the beamline immediately
  // after they've been created
  CalculateAndSetTeleporterDelta(BDSBeamline::Instance());
  G4double teleporterlength = BDSGlobalConstants::Instance()->GetTeleporterLength();
  if(teleporterlength < lengthSafety){
      G4cerr << "---->NOT creating Teleporter, "
             << " name = " << _element.name
             << ", LENGTH TOO SHORT:"
             << " l = " << teleporterlength << "m"
             << G4endl;
      return NULL;
    }
  else {
#ifdef BDSDEBUG
    G4cout << "---->creating Teleporter,"
	   << " name        = " << _element.name
	   << " l           = " << teleporterlength/CLHEP::m << "m"
	   << G4endl;
#endif


    return( new BDSTeleporter( _element.name,           //name
			       teleporterlength ));        //length
  }
}

BDSAcceleratorComponent* BDSComponentFactory::createDrift()
{
  //G4double tunnelOffsetX = BDSGlobalConstants::Instance()->GetTunnelOffsetX();
  // like most elements, drift requires l > 4*lengthSafety
  if(_element.l*CLHEP::m < 4*lengthSafety){
    G4cerr << "---->NOT creating element, "
             << " name = " << _element.name
             << ", LENGTH TOO SHORT:"
             << " l = " << _element.l*CLHEP::m << "m"
             << G4endl;
      return NULL;
  }
#ifdef BDSDEBUG
  G4cout << "---->creating Drift,"
	 << " name= " << _element.name
	 << " l= " << _element.l << "m"
	 << G4endl;
#endif
  
  return (new BDSDrift( _element.name,
			_element.l*CLHEP::m,
			PrepareBeamPipeInfo(_element)
			));
}

BDSAcceleratorComponent* BDSComponentFactory::createRF()
{
  if(_element.l*CLHEP::m < 4*lengthSafety){
    G4cerr << "---->NOT creating element, "
             << " name = " << _element.name
             << ", LENGTH TOO SHORT:"
             << " l = " << _element.l*CLHEP::m << "m"
             << G4endl;
      return NULL;
  }
  
  return (new BDSRfCavity( _element.name,
			   _element.l * CLHEP::m,
			   _element.gradient,
			   PrepareBeamPipeInfo(_element),
			   PrepareBoxSize(_element)));	
}

BDSAcceleratorComponent* BDSComponentFactory::createSBend()
{
  if(_element.l*CLHEP::m < 4*lengthSafety){
    G4cerr << "---->NOT creating element, "
             << " name = " << _element.name
             << ", LENGTH TOO SHORT:"
             << " l = " << _element.l*CLHEP::m << "m"
             << G4endl;
      return NULL;
  }
  // arc length
  G4double length = _element.l*CLHEP::m;
  G4double magFieldLength = length;
  
  // magnetic field
  // MAD conventions:
  // 1) a positive bend angle represents a bend to the right, i.e.
  // towards negative x values (even for negative _charges??)
  // 2) a positive K1 = 1/|Brho| dBy/dx means horizontal focusing of
  // positive charges
  // CHECK SIGNS 
  
  G4double bField;
  if(_element.B != 0) {
    bField = _element.B * CLHEP::tesla;
    G4double rho = _brho/bField;
    //    _element.angle  = - 2.0*asin(magFieldLength/2.0/rho);
    _element.angle  = - magFieldLength/rho;
  }
  else {
    _element.angle *= -1;
    //    bField = - 2 * _brho * sin(_element.angle/2.0) / magFieldLength;
    bField = - _brho * _element.angle/magFieldLength * _charge; // charge in e units
    _element.B = bField/CLHEP::tesla;
  }
  
  // B' = dBy/dx = Brho * (1/Brho dBy/dx) = Brho * k1
  // Brho is already in G4 units, but k1 is not -> multiply k1 by m^-2
  G4double bPrime = - _brho * (_element.k1 / CLHEP::m2);

  //calculate number of sbends to split parent into
  //if maximum distance between arc path and straight path larger than 1mm, split sbend into N chunks,
  //this also works when maximum distance is less than 1mm as there will just be 1 chunk!
  double aperturePrecision = 1.0; // in mm
  // from formula: L/2 / N tan (angle/N) < precision. (L=physical length)
  int nSbends = (int) ceil(std::sqrt(std::abs(length*_element.angle/2/aperturePrecision)));
  //nSbends = 1;   //use for debugging
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " splitting sbend into " << nSbends << " sbends" << G4endl;
#endif
  //calculate their angle and length
  double semiangle = _element.angle / (double) nSbends;
  double semilength = length / (double) nSbends;
  //create Line to put them in
  BDSLine* sbendline = new BDSLine("sbendline",	_element.blmLocZ, _element.blmLocTheta);
  //create sbends and put them in the line
  for (int i = 0; i < nSbends; ++i)
    {
      std::stringstream name;
      name << _element.name << "_" << i;
      std::string itsname = name.str();
      sbendline->addComponent( new BDSSectorBend( itsname,
						  semilength,
						  semiangle,
						  bField,
						  bPrime,
						  PrepareBeamPipeInfo(_element),
						  PrepareBoxSize(_element)
						  ));
    }
  return sbendline;
}

BDSAcceleratorComponent* BDSComponentFactory::createRBend()
{
  if(_element.l*CLHEP::m < 4*lengthSafety){
    G4cerr << "---->NOT creating element, "
             << " name = " << _element.name
             << ", LENGTH TOO SHORT:"
             << " l = " << _element.l*CLHEP::m << "m"
             << G4endl;
      return NULL;
  }
  // calculate length of central straight length and edge sections
  // unfortunately, this has to be duplicated here as we need to
  // calculated the magnetic field length (less than the full length)
  // in case we need to calculate the field
  G4double outerRadius = PrepareBoxSize(_element)*0.5;
  G4double angle       = _element.angle;
  G4double chordLength = _element.l*CLHEP::m;
  G4double straightSectionChord = outerRadius / (tan(0.5*fabs(angle)) + tan((0.5*CLHEP::pi) - (0.5*fabs(angle))) );
  G4double magFieldLength = chordLength - (2.0*straightSectionChord);

  // magnetic field
  // CHECK SIGNS OF B, B', ANGLE
  G4double bField;
  if(_element.B != 0){
  // angle = arc length/radius of curvature = L/rho = (B*L)/(B*rho)
    bField = _element.B * CLHEP::tesla;
    G4double rho = _brho/bField;
    //_element.angle  = - bField * length / brho;
    _element.angle  = - 2.0*asin(magFieldLength/2.0/rho);
    G4cout << "calculated angle from field - now " << _element.angle << G4endl;
  }
  else{
    _element.angle *= -1;
    // arc length = radius*angle
    //            = (geometrical length/(2.0*sin(angle/2))*angle
    G4double arclength = 0.5*magFieldLength * fabs(_element.angle) / sin(fabs(_element.angle)*0.5);
    // B = Brho/rho = Brho/(arc length/angle)
    bField = - _brho * _element.angle / arclength * _charge; // charge in e units
    _element.B = bField/CLHEP::tesla;
    G4cout << "calculated field from angle - angle,field = " << _element.angle << " " << _element.B << G4endl;
  }
  
  // B' = dBy/dx = Brho * (1/Brho dBy/dx) = Brho * k1
  // Brho is already in G4 units, but k1 is not -> multiply k1 by m^-2
  G4double bPrime = - _brho * (_element.k1 / CLHEP::m2);

  return (new BDSRBend( _element.name,
			_element.l*CLHEP::m,
			bField,
			bPrime,
			_element.angle,
			PrepareBeamPipeInfo(_element),
			PrepareBoxSize(_element)
			));
}

BDSAcceleratorComponent* BDSComponentFactory::createHKick(){
  if(_element.l*CLHEP::m < 4*lengthSafety){
    G4cerr << "---->NOT creating element, "
             << " name = " << _element.name
             << ", LENGTH TOO SHORT:"
             << " l = " << _element.l*CLHEP::m << "m"
             << G4endl;
      return NULL;
  }

  G4double length = _element.l*CLHEP::m;
  
  // magnetic field
  G4double bField;
  if(_element.B != 0){
    // angle = arc length/radius of curvature = L/rho = (B*L)/(B*rho)
    bField = _element.B * CLHEP::tesla;
    _element.angle  = -bField * length / _brho;
  }
  else{
    // B = Brho/rho = Brho/(arc length/angle)
    bField = - _brho * _element.angle / length * _charge; // charge in e units
    _element.B = bField/CLHEP::tesla;
  }
  
  // B' = dBy/dx = Brho * (1/Brho dBy/dx) = Brho * k1
  // Brho is already in G4 units, but k1 is not -> multiply k1 by m^-2
  G4double bPrime = - _brho * (_element.k1 / CLHEP::m2);

  // LN I think we should build it anyway and the stepper should deal
  // with this - ie so we still have the outer geometry
  /*
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
  }
  */
  return (new BDSKicker( _element.name,
			 _element.l * CLHEP::m,
			 bField,
			 bPrime,
			 _element.angle,
			 false,   // it's a horizontal kicker
			 PrepareBeamPipeInfo(_element),
			 PrepareBoxSize(_element) ));
}

BDSAcceleratorComponent* BDSComponentFactory::createVKick(){
  if(_element.l*CLHEP::m < 4*lengthSafety){
    G4cerr << "---->NOT creating element, "
             << " name = " << _element.name
             << ", LENGTH TOO SHORT:"
             << " l = " << _element.l*CLHEP::m << "m"
             << G4endl;
      return NULL;
  }
  
  G4double length = _element.l*CLHEP::m;
  
  // magnetic field
  G4double bField;
  if(_element.B != 0){
    // angle = arc length/radius of curvature = L/rho = (B*L)/(B*rho)
    bField = _element.B * CLHEP::tesla;
    _element.angle  = -bField * length / _brho;
  }
  else{
    // B = Brho/rho = Brho/(arc length/angle)
    bField = - _brho * _element.angle / length * _charge; // charge in e units
    _element.B = bField/CLHEP::tesla;
  }
  // B' = dBy/dx = Brho * (1/Brho dBy/dx) = Brho * k1
  // Brho is already in G4 units, but k1 is not -> multiply k1 by m^-2
  G4double bPrime = - _brho * (_element.k1 / CLHEP::m2);

  // LN I think we should build it anyway and the stepper should deal
  // with this - ie so we still have the outer geometry
  /*
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
  }
  */
  return (new BDSKicker( _element.name,
			 _element.l * CLHEP::m,
			 bField,
			 bPrime,
			 _element.angle,
			 true,   // it's a vertical kicker
			 PrepareBeamPipeInfo(_element),
			 PrepareBoxSize(_element) ));
}

BDSAcceleratorComponent* BDSComponentFactory::createQuad()
{
  if(_element.l*CLHEP::m < 4*lengthSafety){
    G4cerr << "---->NOT creating element, "
             << " name = " << _element.name
             << ", LENGTH TOO SHORT:"
             << " l = " << _element.l*CLHEP::m << "m"
             << G4endl;
      return NULL;
  }
  // magnetic field
  // B' = dBy/dx = Brho * (1/Brho dBy/dx) = Brho * k1
  // Brho is already in G4 units, but k1 is not -> multiply k1 by m^-2
  G4double bPrime = - _brho * (_element.k1 / CLHEP::m2);
  
  return (new BDSQuadrupole( _element.name,
			     _element.l * CLHEP::m,
			     bPrime,
			     PrepareBeamPipeInfo(_element),
			     PrepareBoxSize(_element)));			     
}  
  
BDSAcceleratorComponent* BDSComponentFactory::createSextupole()
{
  if(_element.l*CLHEP::m < 4*lengthSafety){
    G4cerr << "---->NOT creating element, "
             << " name = " << _element.name
             << ", LENGTH TOO SHORT:"
             << " l = " << _element.l*CLHEP::m << "m"
             << G4endl;
      return NULL;
  }
  // magnetic field 
  // B'' = d^2By/dx^2 = Brho * (1/Brho d^2By/dx^2) = Brho * k2
  // brho is in Geant4 units, but k2 is not -> multiply k2 by m^-3
  G4double bDoublePrime = - _brho * (_element.k2 / CLHEP::m3);
  
#ifdef BDSDEBUG 
  G4cout << "---->creating Sextupole,"
	 << " name= " << _element.name
	 << " l= " << _element.l << "m"
	 << " k2= " << _element.k2 << "m^-3"
	 << " brho= " << fabs(_brho)/(CLHEP::tesla*CLHEP::m) << "T*m"
	 << " B''= " << bDoublePrime/(CLHEP::tesla/CLHEP::m2) << "T/m^2"
	 << " tilt= " << _element.tilt << "rad"
	 << " tunnel material " << _element.tunnelMaterial
	 << " material= " << _element.material
	 << G4endl;
#endif
  
  return (new BDSSextupole( _element.name,
			    _element.l * CLHEP::m,
			    bDoublePrime,
			    PrepareBeamPipeInfo(_element),
			    PrepareBoxSize(_element)));	
  
}

BDSAcceleratorComponent* BDSComponentFactory::createOctupole()
{
  if(_element.l*CLHEP::m < 4*lengthSafety){
    G4cerr << "---->NOT creating element, "
             << " name = " << _element.name
             << ", LENGTH TOO SHORT:"
             << " l = " << _element.l*CLHEP::m << "m"
             << G4endl;
      return NULL;
  }
  // magnetic field  
  // B''' = d^3By/dx^3 = Brho * (1/Brho d^3By/dx^3) = Brho * k3
  // brho is in Geant4 units, but k3 is not -> multiply k3 by m^-4
  G4double bTriplePrime = - _brho * (_element.k3 / (CLHEP::m3*CLHEP::m));
  
#ifdef BDSDEBUG 
  G4cout << "---->creating Octupole,"
	 << " name= " << _element.name
	 << " l= " << _element.l << "m"
	 << " k3= " << _element.k3 << "m^-4"
	 << " brho= " << fabs(_brho)/(CLHEP::tesla*CLHEP::m) << "T*m"
	 << " B'''= " << bTriplePrime/(CLHEP::tesla/CLHEP::m3) << "T/m^3"
	 << " tilt= " << _element.tilt << "rad"
	 << " tunnel material " << _element.tunnelMaterial
	 << " material= " << _element.material
	 << G4endl;
#endif
  
  return ( new BDSOctupole( _element.name,
			    _element.l * CLHEP::m,
			    bTriplePrime,
			    PrepareBeamPipeInfo(_element),
			    PrepareBoxSize(_element)));
}

BDSAcceleratorComponent* BDSComponentFactory::createMultipole()
{
  if(_element.l*CLHEP::m < 4*lengthSafety){
    G4cerr << "---->NOT creating element, "
             << " name = " << _element.name
             << ", LENGTH TOO SHORT:"
             << " l = " << _element.l*CLHEP::m << "m"
             << G4endl;
      return NULL;
  }
#ifdef BDSDEBUG 
  G4cout << "---->creating Multipole,"
	 << " name= " << _element.name
	 << " l= " << _element.l << "m"
	 << " tilt= " << _element.tilt << "rad"
	 << " tunnel material " << _element.tunnelMaterial
	 << " material= " << _element.material
	 << G4endl;
#endif
  // magnetic field
  std::list<double>::iterator kit;
  
#ifdef BDSDEBUG 
  G4cout << " knl={ ";
#endif
  for(kit=(_element.knl).begin();kit!=(_element.knl).end();kit++)
    {
#ifdef BDSDEBUG 
      G4cout<<(*kit)<<", ";
#endif
      (*kit) /= _element.l; 
    }
#ifdef BDSDEBUG 
  G4cout << "}";
#endif
  
#ifdef BDSDEBUG 
  G4cout << " ksl={ ";
#endif
  for(kit=(_element.ksl).begin();kit!=(_element.ksl).end();kit++)
    {
#ifdef BDSDEBUG 
      G4cout<<(*kit)<<" ";
#endif
      (*kit) /= _element.l; 
    }
#ifdef BDSDEBUG 
  G4cout << "}" << G4endl;
#endif

  return (new BDSTMultipole( _element.name,
			     _element.l * CLHEP::m,
			     _element.knl,
			     _element.ksl,
			     PrepareBeamPipeInfo(_element),
			     PrepareBoxSize(_element)));
}

BDSAcceleratorComponent* BDSComponentFactory::createElement(){
  if(_element.l*CLHEP::m < 4*lengthSafety){
    G4cerr << "---->NOT creating element, "
             << " name = " << _element.name
             << ", LENGTH TOO SHORT:"
             << " l = " << _element.l*CLHEP::m << "m"
             << G4endl;
      return NULL;
  }

  // geometry
  G4double aper = _bpRad;
  if( _element.aper > 1.e-10*CLHEP::m ) aper = _element.aper * CLHEP::m;

#ifdef BDSDEBUG 
  G4cout << "---->creating Element,"
	 << " name= " << _element.name
	 << " l= " << _element.l << "m"
	 << " aper= " << aper/CLHEP::m << "m"
	 << " outR= " << _element.outR << "m"
	 << " bmapZOffset = "	<<  _element.bmapZOffset * CLHEP::m
	 << " tunnel material " << _element.tunnelMaterial
	 << " tunnel cavity material " << _element.tunnelCavityMaterial
	 << " precision region " << _element.precisionRegion
	 << G4endl;
#endif
  
  G4double tunnelOffsetX = BDSGlobalConstants::Instance()->GetTunnelOffsetX();
  if(_element.tunnelOffsetX<1e6) {
    tunnelOffsetX = _element.tunnelOffsetX*CLHEP::m;
  }

  return (new BDSElement( _element.name,
			  _element.geometryFile,
			  _element.bmapFile,
			  _element.bmapZOffset * CLHEP::m,
			  _element.l * CLHEP::m,
			  aper,
			  _element.outR * CLHEP::m , _element.tunnelMaterial, _element.tunnelRadius, tunnelOffsetX, _element.tunnelCavityMaterial));
}

BDSAcceleratorComponent* BDSComponentFactory::createSolenoid()
{
  if(_element.l*CLHEP::m < 4*lengthSafety){
    G4cerr << "---->NOT creating element, "
             << " name = " << _element.name
             << ", LENGTH TOO SHORT:"
             << " l = " << _element.l*CLHEP::m << "m"
             << G4endl;
      return NULL;
  }
  // magnetic field
  //
  // B = B/Brho * Brho = ks * Brho
  // brho is in Geant4 units, but ks is not -> multiply ks by m^-1
  G4double bField;
  if(_element.B != 0){
    bField = _element.B * CLHEP::tesla;
    _element.ks  = (bField/_brho) / CLHEP::m;
  }
  else{
    bField = (_element.ks/CLHEP::m) * _brho;
    _element.B = bField/CLHEP::tesla;
  }
  
#ifdef BDSDEBUG 
  G4cout << "---->creating Solenoid,"
	 << " name= " << _element.name
	 << " l= " << _element.l << " m"
	 << " ks= " << _element.ks << " m^-1"
	 << " brho= " << fabs(_brho)/(CLHEP::tesla*CLHEP::m) << " T*m"
	 << " B= " << bField/CLHEP::tesla << " T"
	 << " tunnel material " << _element.tunnelMaterial
	 << " material= " << _element.material
	 << G4endl;
#endif
  
  return (new BDSSolenoid( _element.name,
			   _element.l * CLHEP::m,
			   bField,
			   PrepareBeamPipeInfo(_element),
			   PrepareBoxSize(_element)));
}

BDSAcceleratorComponent* BDSComponentFactory::createCollimator(){
  if(_element.l*CLHEP::m < 4*lengthSafety){
    G4cerr << "---->NOT creating element, "
             << " name = " << _element.name
             << ", LENGTH TOO SHORT:"
             << " l = " << _element.l*CLHEP::m << "m"
             << G4endl;
      return NULL;
  }
  G4Material* theMaterial;
  if(_element.material != "")
    theMaterial = BDSMaterials::Instance()->GetMaterial( _element.material );
  else
    theMaterial = BDSMaterials::Instance()->GetMaterial( "Graphite" );

#ifdef BDSDEBUG 
  G4cout << "---->creating " << typestr(_element.type) << ","
	 << " name= " << _element.name 
	 << " xaper= " << _element.xsize <<"m"
	 << " yaper= " << _element.ysize <<"m"
	 << " flatl= " << _element.flatlength <<"m"
	 << " taper= " << _element.taperlength <<"m"
	 << " material= " << _element.material
	 << " tunnel material " << _element.tunnelMaterial
	 << G4endl;
#endif

  return (new BDSCollimator( _element.name,
			     _element.l * CLHEP::m,
			     _bpRad,
			     _element.xsize * CLHEP::m,
			     _element.ysize * CLHEP::m,
			     theMaterial,
			     _element.outR*CLHEP::m,
			     _element.blmLocZ,
			     _element.blmLocTheta,
			     _element.tunnelMaterial) );
}

BDSAcceleratorComponent* BDSComponentFactory::createMuSpoiler(){
  if(_element.l*CLHEP::m < 4*lengthSafety){
    G4cerr << "---->NOT creating element, "
             << " name = " << _element.name
             << ", LENGTH TOO SHORT:"
             << " l = " << _element.l*CLHEP::m << "m"
             << G4endl;
      return NULL;
  }
#ifdef BDSDEBUG 
  G4cout << "---->creating muspoiler,"
	 << " name= " << _element.name 
	 << " length= " << _element.l
	 << " B= " << _element.B
	 << " tunnel material " << _element.tunnelMaterial
	 << G4endl;
#endif
        
#ifdef BDSDEBUG
  G4cout << "BDSMuSpoiler: " << _element.name << " " << _element.l*CLHEP::m << " " << " " << _element.B*CLHEP::tesla << G4endl;
#endif
  
  return (new BDSMuSpoiler(_element.name,
			   _element.l*CLHEP::m,
			   _element.B * CLHEP::tesla,
			   PrepareBeamPipeInfo(_element),
			   PrepareBoxSize(_element) ));
}

BDSAcceleratorComponent* BDSComponentFactory::createLaser(){
  
  if(_element.l == 0) _element.l = 1e-8;
	
#ifdef BDSDEBUG 
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
	
#ifdef BDSDEBUG 
        G4cout << "---->creating Screen,"
               << " name= "<< _element.name
               << " l=" << _element.l/CLHEP::m<<"m"
               << " tscint=" << _element.tscint/CLHEP::m<<"m"
               << " angle=" << _element.angle/CLHEP::rad<<"rad"
               << " scintmaterial=" << "ups923a"//_element.scintmaterial
               << " airmaterial=" << "vacuum"//_element.airmaterial
               << G4endl;
#endif
	return (new BDSScintillatorScreen( _element.name, _element.tscint*CLHEP::m, (_element.angle-0.78539816339)*CLHEP::rad, "ups923a",BDSGlobalConstants::Instance()->GetVacuumMaterial())); //Name, scintillator thickness, angle in radians (relative to -45 degrees)
}


BDSAcceleratorComponent* BDSComponentFactory::createAwakeScreen(){
	
#ifdef BDSDEBUG 
        G4cout << "---->creating Awake Screen,"
	       << "twindow = " << _element.twindow*1e3/CLHEP::um << " um"
	       << "tscint = " << _element.tscint*1e3/CLHEP::um << " um"
	       << "windowmaterial = " << _element.windowmaterial << " um"
	       << "scintmaterial = " << _element.scintmaterial << " um"
               << G4endl;
#endif
	return (new BDSAwakeScintillatorScreen(_element.name, _element.scintmaterial, _element.tscint*1e3, _element.angle, _element.twindow*1e3, _element.windowmaterial)); //Name
}

BDSAcceleratorComponent* BDSComponentFactory::createTransform3D(){
	
#ifdef BDSDEBUG 
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

BDSAcceleratorComponent* BDSComponentFactory::createTerminator(){
  return (new BDSTerminator(_element.name, 
			    BDSGlobalConstants::Instance()->GetSamplerLength()
			    ));
}



G4Material* BDSComponentFactory::PrepareBeamPipeMaterial(Element& element)
{
  G4Material* beamPipeMaterial;
  if(element.beampipeMaterial == "")
    {
      G4String defaultMaterialName = BDSGlobalConstants::Instance()->GetBeamPipeMaterialName();
      beamPipeMaterial = BDSMaterials::Instance()->GetMaterial(defaultMaterialName);
    }
  else
    { beamPipeMaterial = BDSMaterials::Instance()->GetMaterial(element.beampipeMaterial); }
  return beamPipeMaterial;
}

G4Material* BDSComponentFactory::PrepareVacuumMaterial(Element& /*element*/)
{
  //in future do somethign relating to what's set in element
  //also make some setting available in element
  return BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial());
}

G4double BDSComponentFactory::PrepareBoxSize(Element& element)
{
  G4double boxSize = element.boxSize*CLHEP::m;
  if (boxSize < 1e-6)
    {//boxSize not set - use global option as default
      boxSize = BDSGlobalConstants::Instance()->GetComponentBoxSize();
    }
  // returns in metres
  return boxSize;
}

BDSBeamPipeInfo BDSComponentFactory::PrepareBeamPipeInfo(Element& element)
{
  BDSBeamPipeInfo info;
  info.beamPipeType      = BDS::DetermineBeamPipeType(element.apertureType);

  // note even if aperN in the element is 0 (ie unset), we should use
  // the default aperture model from global constants (already in metres)
  // aper1
  if (element.aper1 == 0)
    {info.aper1 = BDSGlobalConstants::Instance()->GetAper1();}
  else
    {info.aper1 = element.aper1*CLHEP::m;}
  // aper2
  if (element.aper2 == 0)
    {info.aper2 = BDSGlobalConstants::Instance()->GetAper2();}
  else
    {info.aper2 = element.aper2*CLHEP::m;}
  // aper3
  if (element.aper3 == 0)
    {info.aper3 = BDSGlobalConstants::Instance()->GetAper3();}
  else
    {info.aper3 = element.aper3*CLHEP::m;}
  // aper4
  if (element.aper4 == 0)
    {info.aper4 = BDSGlobalConstants::Instance()->GetAper4();}
  else
    {info.aper4 = element.aper4*CLHEP::m;}
  
  info.vacuumMaterial    = PrepareVacuumMaterial(element);
  info.beamPipeThickness = element.beampipeThickness*CLHEP::m;
  info.beamPipeMaterial  = PrepareBeamPipeMaterial(element);
  return info;
}
