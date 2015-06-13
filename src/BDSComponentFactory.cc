#include "BDSComponentFactory.hh"

// elements
#include "BDSAwakeScintillatorScreen.hh"
#include "BDSCollimatorElliptical.hh"
#include "BDSCollimatorRectangular.hh"
#include "BDSDrift.hh"
#include "BDSDump.hh"
#include "BDSElement.hh"
#include "BDSKicker.hh"
#include "BDSLaserWire.hh"
#include "BDSLine.hh"
#include "BDSMuSpoiler.hh"
#include "BDSOctupole.hh"
#include "BDSQuadrupole.hh"
#include "BDSRBend.hh"
#include "BDSRfCavity.hh"
#include "BDSSampler.hh"
#include "BDSSamplerCylinder.hh"
#include "BDSScintillatorScreen.hh"
#include "BDSSectorBend.hh"
#include "BDSSextupole.hh"
#include "BDSSolenoid.hh"
#include "BDSTerminator.hh"
#include "BDSTeleporter.hh"
#include "BDSTiltOffset.hh"
#include "BDSMultipole.hh"
#include "BDSTransform3D.hh"

// general
#include "BDSBeamline.hh"
#include "BDSBeamPipeType.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetGeometryType.hh"
#include "BDSTunnelInfo.hh"

#include "parser/enums.h"
#include "parser/elementlist.h"

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
    element = createEllipticalCollimator(); break; 
  case _RCOL:
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory  - creating rcol" << G4endl;
#endif
    element = createRectangularCollimator(); break; 
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

  if (element)
    {element->Initialise();}
  
  return element;
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
		       BDSGlobalConstants::Instance()->GetSamplerLength()));
}

BDSAcceleratorComponent* BDSComponentFactory::createTeleporter(){
  // This relies on things being added to the beamline immediately
  // after they've been created
  G4double teleporterlength = BDSGlobalConstants::Instance()->GetTeleporterLength();
  G4String name = "teleporter";
#ifdef BDSDEBUG
    G4cout << "---->creating Teleporter,"
	   << " name = " << name
	   << ", l = " << teleporterlength/CLHEP::m << "m"
	   << G4endl;
#endif

    return( new BDSTeleporter(name,
			      teleporterlength ));
  
}

BDSAcceleratorComponent* BDSComponentFactory::createDrift()
{
  if(!HasSufficientMinimumLength(_element))
    {return NULL;}

#ifdef BDSDEBUG
  G4cout << "---->creating Drift,"
	 << " name= " << _element.name
	 << " l= " << _element.l << "m"
	 << G4endl;
#endif
  
  return (new BDSDrift( _element.name,
			_element.l*CLHEP::m,
			PrepareBeamPipeInfo(_element) ));
}

BDSAcceleratorComponent* BDSComponentFactory::createRF()
{
  if(!HasSufficientMinimumLength(_element))
    {return NULL;}
  
  return (new BDSRfCavity( _element.name,
			   _element.l * CLHEP::m,
			   _element.gradient,
			   PrepareBeamPipeInfo(_element),
			   PrepareMagnetOuterInfo(_element)));
}

BDSAcceleratorComponent* BDSComponentFactory::createSBend()
{
  if(!HasSufficientMinimumLength(_element))
    {return NULL;}
  
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
    // charge in e units
    // multiply once more with ffact to not flip fields in bends
    bField = - _brho * _element.angle/magFieldLength * _charge * BDSGlobalConstants::Instance()->GetFFact();
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
  BDSLine* sbendline = new BDSLine("sbendline");
  //create sbends and put them in the line
  BDSBeamPipeInfo* bpInfo    = PrepareBeamPipeInfo(_element);
  BDSMagnetOuterInfo moInfo = PrepareMagnetOuterInfo(_element);
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
						  bpInfo,
						  moInfo) );
    }
  return sbendline;
}

BDSAcceleratorComponent* BDSComponentFactory::createRBend()
{
  if(!HasSufficientMinimumLength(_element))
    {return NULL;}
  
  // calculate length of central straight length and edge sections
  // unfortunately, this has to be duplicated here as we need to
  // calculated the magnetic field length (less than the full length)
  // in case we need to calculate the field
  G4double outerRadius = PrepareOuterDiameter(_element)*0.5;
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
#ifdef BDSDEBUG
    G4cout << "calculated angle from field - now " << _element.angle << G4endl;
#endif
  }
  else{
    _element.angle *= -1;
    // arc length = radius*angle
    //            = (geometrical length/(2.0*sin(angle/2))*angle
    G4double arclength = 0.5*magFieldLength * fabs(_element.angle) / sin(fabs(_element.angle)*0.5);
    // B = Brho/rho = Brho/(arc length/angle)
    // charge in e units
    // multiply once more with ffact to not flip fields in bends
    bField = - _brho * _element.angle / arclength * _charge * BDSGlobalConstants::Instance()->GetFFact();
    _element.B = bField/CLHEP::tesla;
#ifdef BDSDEBUG
    G4cout << "calculated field from angle - angle,field = " << _element.angle << " " << _element.B << G4endl;
#endif
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
			PrepareMagnetOuterInfo(_element)));
}

BDSAcceleratorComponent* BDSComponentFactory::createHKick()
{
  if(!HasSufficientMinimumLength(_element))
    {return NULL;}  
  
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
    // charge in e units
    // multiply once more with ffact to not flip fields in kicks defined with angle
    bField = - _brho * _element.angle / length * _charge * BDSGlobalConstants::Instance()->GetFFact(); // charge in e units
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
	   << " aper= " << aper/CLHEP::m << "
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
			 PrepareMagnetOuterInfo(_element)));
}

BDSAcceleratorComponent* BDSComponentFactory::createVKick()
{
  if(!HasSufficientMinimumLength(_element))
    {return NULL;}
  
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
    // charge in e units
    // multiply once more with ffact to not flip fields in kicks
    bField = - _brho * _element.angle / length * _charge * BDSGlobalConstants::Instance()->GetFFact();
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
	   << " aper= " << aper/CLHEP::m << "
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
			 PrepareMagnetOuterInfo(_element)
			 ));
}

BDSAcceleratorComponent* BDSComponentFactory::createQuad()
{
  if(!HasSufficientMinimumLength(_element))
    {return NULL;}
  
  // magnetic field
  // B' = dBy/dx = Brho * (1/Brho dBy/dx) = Brho * k1
  // Brho is already in G4 units, but k1 is not -> multiply k1 by m^-2
  G4double bPrime = - _brho * (_element.k1 / CLHEP::m2);

  return (new BDSQuadrupole( _element.name,
			     _element.l * CLHEP::m,
			     bPrime,
			     PrepareBeamPipeInfo(_element),
			     PrepareMagnetOuterInfo(_element)));
}  
  
BDSAcceleratorComponent* BDSComponentFactory::createSextupole()
{
  if(!HasSufficientMinimumLength(_element))
    {return NULL;}
  
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
	 << " material= " << _element.outerMaterial
	 << G4endl;
#endif
  
  return (new BDSSextupole( _element.name,
			    _element.l * CLHEP::m,
			    bDoublePrime,
			    PrepareBeamPipeInfo(_element),
			    PrepareMagnetOuterInfo(_element)));
}

BDSAcceleratorComponent* BDSComponentFactory::createOctupole()
{
  if(!HasSufficientMinimumLength(_element))
    {return NULL;}
  
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
	 << " material= " << _element.outerMaterial
	 << G4endl;
#endif
  
  return ( new BDSOctupole( _element.name,
			    _element.l * CLHEP::m,
			    bTriplePrime,
			    PrepareBeamPipeInfo(_element),
			    PrepareMagnetOuterInfo(_element)));
}

BDSAcceleratorComponent* BDSComponentFactory::createMultipole()
{
 if(!HasSufficientMinimumLength(_element))
    {return NULL;}
 
#ifdef BDSDEBUG 
  G4cout << "---->creating Multipole,"
	 << " name= " << _element.name
	 << " l= " << _element.l << "m"
	 << " material= " << _element.outerMaterial
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

  return (new BDSMultipole( _element.name,
			    _element.l * CLHEP::m,
			    _element.knl,
			    _element.ksl,
			    PrepareBeamPipeInfo(_element),
			    PrepareMagnetOuterInfo(_element)));
}

BDSAcceleratorComponent* BDSComponentFactory::createElement()
{
  if(!HasSufficientMinimumLength(_element))
    {return NULL;}
  
#ifdef BDSDEBUG 
  G4cout << "---->creating Element,"
	 << " name = " << _element.name
	 << " l = " << _element.l << "m"
	 << " outerDiameter = "  << 0.5 * _element.outerDiameter << "m"
	 << " bmapZOffset = "	 <<  _element.bmapZOffset * CLHEP::m
	 << " precision region " << _element.precisionRegion
	 << G4endl;
#endif

  return (new BDSElement(_element.name,
			 _element.l * CLHEP::m,
			 _element.geometryFile,
			 _element.bmapFile,
			 _element.bmapZOffset * CLHEP::m
			  ));
}

BDSAcceleratorComponent* BDSComponentFactory::createSolenoid()
{
  if(!HasSufficientMinimumLength(_element))
    {return NULL;}
  
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
	 << " name = " << _element.name
	 << " l = " << _element.l << " m,"
	 << " ks = " << _element.ks << " m^-1,"
	 << " brho = " << fabs(_brho)/(CLHEP::tesla*CLHEP::m) << " T*m,"
	 << " B = " << bField/CLHEP::tesla << " T,"
	 << " material = \"" << _element.outerMaterial << "\""
	 << G4endl;
#endif
  
  return (new BDSSolenoid( _element.name,
			   _element.l * CLHEP::m,
			   bField,
			   PrepareBeamPipeInfo(_element),
			   PrepareMagnetOuterInfo(_element)));
}

BDSAcceleratorComponent* BDSComponentFactory::createRectangularCollimator()
{
  if(!HasSufficientMinimumLength(_element))
    {return NULL;}

#ifdef BDSDEBUG 
  G4cout << "--->creating " << typestr(_element.type) << ","
	 << " name = " << _element.name  << ","
	 << " x half aperture = " << _element.xsize <<" m,"
	 << " y half aperture = " << _element.ysize <<" m,"
	 << " material = \"" << _element.material << "\""
	 << G4endl;
#endif
  
  return new BDSCollimatorRectangular(_element.name,
				      _element.l*CLHEP::m,
				      _element.outerDiameter*CLHEP::m,
				      _element.xsize*CLHEP::m,
				      _element.ysize*CLHEP::m,
				      _element.material);
}

BDSAcceleratorComponent* BDSComponentFactory::createEllipticalCollimator()
{
  if(!HasSufficientMinimumLength(_element))
    {return NULL;}

#ifdef BDSDEBUG 
  G4cout << "--->creating " << typestr(_element.type) << ","
	 << " name = " << _element.name 
	 << " x half aperture = " << _element.xsize <<" m,"
	 << " y half aperture = " << _element.ysize <<" m,"
	 << " material = \"" << _element.material << "\""
	 << G4endl;
#endif
  
  return new BDSCollimatorElliptical(_element.name,
				     _element.l*CLHEP::m,
				     _element.outerDiameter*CLHEP::m,
				     _element.xsize*CLHEP::m,
				     _element.ysize*CLHEP::m,
				     _element.material);
}

BDSAcceleratorComponent* BDSComponentFactory::createMuSpoiler()
{
  if(!HasSufficientMinimumLength(_element))
    {return NULL;}
  
#ifdef BDSDEBUG 
  G4cout << "---->creating muspoiler,"
	 << " name = " << _element.name 
	 << " length = " << _element.l
	 << " B = " << _element.B
	 << G4endl;
#endif
        
#ifdef BDSDEBUG
  G4cout << "BDSMuSpoiler: " << _element.name << " " << _element.l*CLHEP::m << " mm " << " " << _element.B*CLHEP::tesla << " MT " << G4endl;
#endif
  
  return (new BDSMuSpoiler(_element.name,
			   _element.l*CLHEP::m,
			   _element.B * CLHEP::tesla,
			   PrepareBeamPipeInfo(_element),
			   PrepareMagnetOuterInfo(_element)));
}

BDSAcceleratorComponent* BDSComponentFactory::createLaser()
{
  if(!HasSufficientMinimumLength(_element))
    {return NULL;}
	
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

BDSAcceleratorComponent* BDSComponentFactory::createScreen()
{
  if(!HasSufficientMinimumLength(_element))
    {return NULL;}
	
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

BDSAcceleratorComponent* BDSComponentFactory::createTerminator()
{
  G4String name   = "terminator";
  G4double length = BDSGlobalConstants::Instance()->GetSamplerLength();
#ifdef BDSDEBUG
    G4cout << "---->creating Terminator,"
	   << " name = " << name
	   << " l = "    << length / CLHEP::m << "m"
	   << G4endl;
#endif
  
  return (new BDSTerminator("terminator", 
			    length));
}


G4bool BDSComponentFactory::HasSufficientMinimumLength(Element& element)
{
  if(element.l*CLHEP::m < 4*lengthSafety)
    {
      G4cerr << "---->NOT creating element, "
             << " name = " << _element.name
             << ", LENGTH TOO SHORT:"
             << " l = " << _element.l*CLHEP::m << "m"
             << G4endl;
      return false;
    }
  else
    {return true;}
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
  //in future do something relating to what's set in the element
  //also make some setting available in element
  return BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial());
}

BDSMagnetOuterInfo BDSComponentFactory::PrepareMagnetOuterInfo(Element& element)
{
  BDSMagnetOuterInfo info;
  // magnet geometry type
  if (element.magnetGeometryType == "")
    info.geometryType = BDSGlobalConstants::Instance()->GetMagnetGeometryType();
  else
    info.geometryType = BDS::DetermineMagnetGeometryType(element.magnetGeometryType);

  // outer diameter
  G4double outerDiameter = element.outerDiameter*CLHEP::m;
  if (outerDiameter < 1e-6)
    {//outerDiameter not set - use global option as default
      outerDiameter = BDSGlobalConstants::Instance()->GetOuterDiameter();
    }
  info.outerDiameter = outerDiameter;

  // outer material
  G4Material* outerMaterial;
  if(element.outerMaterial == "")
    {
      G4String defaultMaterialName = BDSGlobalConstants::Instance()->GetOuterMaterialName();
      outerMaterial = BDSMaterials::Instance()->GetMaterial(defaultMaterialName);
    }
  else
    {outerMaterial = BDSMaterials::Instance()->GetMaterial(element.outerMaterial);}
  info.outerMaterial = outerMaterial;
  
  return info;
}

G4double BDSComponentFactory::PrepareOuterDiameter(Element& element)
{
  G4double outerDiameter = element.outerDiameter*CLHEP::m;
  if (outerDiameter < 1e-6)
    {//outerDiameter not set - use global option as default
      outerDiameter = BDSGlobalConstants::Instance()->GetOuterDiameter();
    }
  // returns in metres
  return outerDiameter;
}

BDSBeamPipeInfo* BDSComponentFactory::PrepareBeamPipeInfo(Element& element)
{
  BDSBeamPipeInfo* info = new BDSBeamPipeInfo;
  if (element.apertureType == "")
    info->beamPipeType = BDSGlobalConstants::Instance()->GetApertureType();
  else 
    info->beamPipeType = BDS::DetermineBeamPipeType(element.apertureType);

  // note even if aperN in the element is 0 (ie unset), we should use
  // the default aperture model from global constants (already in metres)
  // aper1
  if (element.aper1 == 0)
    {info->aper1 = BDSGlobalConstants::Instance()->GetAper1();}
  else
    {info->aper1 = element.aper1*CLHEP::m;}
  // aper2
  if (element.aper2 == 0)
    {info->aper2 = BDSGlobalConstants::Instance()->GetAper2();}
  else
    {info->aper2 = element.aper2*CLHEP::m;}
  // aper3
  if (element.aper3 == 0)
    {info->aper3 = BDSGlobalConstants::Instance()->GetAper3();}
  else
    {info->aper3 = element.aper3*CLHEP::m;}
  // aper4
  if (element.aper4 == 0)
    {info->aper4 = BDSGlobalConstants::Instance()->GetAper4();}
  else
    {info->aper4 = element.aper4*CLHEP::m;}
  
  info->vacuumMaterial    = PrepareVacuumMaterial(element);
  info->beamPipeThickness = element.beampipeThickness*CLHEP::m;
  if (info->beamPipeThickness < 1e-10)
    {info->beamPipeThickness = BDSGlobalConstants::Instance()->GetBeamPipeThickness();}
  info->beamPipeMaterial  = PrepareBeamPipeMaterial(element);
  return info;
}

BDSTiltOffset* BDSComponentFactory::createTiltOffset(Element& element)
{
  G4double xOffset = element.offsetX;
  G4double yOffset = element.offsetY;
  G4double tilt    = element.tilt;

  return new BDSTiltOffset(xOffset, yOffset, tilt);
}
