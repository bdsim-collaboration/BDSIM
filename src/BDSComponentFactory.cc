#include "BDSComponentFactory.hh"

// elements
#include "BDSAwakeScintillatorScreen.hh"
#include "BDSCavityRF.hh"
#include "BDSCollimatorElliptical.hh"
#include "BDSCollimatorRectangular.hh"
#include "BDSDegrader.hh"
#include "BDSDrift.hh"
#include "BDSDump.hh"
#include "BDSElement.hh"
#include "BDSLaserWire.hh"
#include "BDSLine.hh"
#include "BDSMagnet.hh"
#include "BDSRBend.hh"
#include "BDSRfCavity.hh"
#include "BDSSampler.hh"
#include "BDSSamplerCylinder.hh"
#include "BDSScintillatorScreen.hh"
#include "BDSTerminator.hh"
#include "BDSTeleporter.hh"
#include "BDSTiltOffset.hh"
#include "BDSTransform3D.hh"

// general
#include "BDSAcceleratorComponentRegistry.hh"
#include "BDSBeamline.hh"
#include "BDSBeamPipeType.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSCavityInfo.hh"
#include "BDSCavityType.hh"
#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldType.hh"
#include "BDSIntegratorType.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetGeometryType.hh"
#include "BDSMagnetStrength.hh"
#include "BDSMagnetType.hh"
#include "BDSParser.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4GeometryTolerance.hh"
#include "G4Transform3D.hh"

#include "parser/elementtype.h"
#include "parser/cavitymodel.h"

#include <cmath>
#include <string>

#ifdef BDSDEBUG
bool debug1 = true;
#else
bool debug1 = false;
#endif

using namespace GMAD;


BDSComponentFactory::BDSComponentFactory()
{
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
  _brho *= - (CLHEP::tesla*CLHEP::m);

  if (verbose || debug1)
    {G4cout << "Rigidity (Brho) : "<< fabs(_brho)/(CLHEP::tesla*CLHEP::m) << " T*m"<<G4endl;}

  // prepare rf cavity model info from parser
  PrepareCavityModels();
}

BDSComponentFactory::~BDSComponentFactory()
{
  for(auto info : cavityInfos)
    {delete info.second;}
}

BDSAcceleratorComponent* BDSComponentFactory::CreateComponent(Element& elementIn)
{
  _element = elementIn;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "named: \"" << _element.name << "\"" << G4endl;  
#endif
  // check if the component already exists and return that
  // do check for output elements since reliant on unique name
  if (_element.type != ElementType::_SAMPLER &&
      _element.type != ElementType::_CSAMPLER &&
      _element.type != ElementType::_DUMP &&
      BDSAcceleratorComponentRegistry::Instance()->IsRegistered(_element.name))
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "using already manufactured component" << G4endl;
#endif
      return BDSAcceleratorComponentRegistry::Instance()->GetComponent(_element.name);
    }
  
  BDSAcceleratorComponent* element = nullptr;
#ifdef BDSDEBUG
  G4cout << "BDSComponentFactory - creating " << _element.type << G4endl;
#endif
  switch(_element.type){
  case ElementType::_SAMPLER:
    element = CreateSampler(); break;
  case ElementType::_DRIFT:
    element = CreateDrift(); break; 
  case ElementType::_RF:
    element = CreateRF(); break; 
  case ElementType::_SBEND:
    element = CreateSBend(); break; 
  case ElementType::_RBEND:
    element = CreateRBend(); break; 
  case ElementType::_HKICK:
    element = CreateKicker(false); break; 
  case ElementType::_VKICK:
    element = CreateKicker(true); break; 
  case ElementType::_QUAD:
    element = CreateQuad(); break; 
  case ElementType::_SEXTUPOLE:
    element = CreateSextupole(); break; 
  case ElementType::_OCTUPOLE:
    element = CreateOctupole(); break; 
  case ElementType::_DECAPOLE:
    element = CreateDecapole(); break; 
  case ElementType::_MULT:
    element = CreateMultipole(); break; 
  case ElementType::_ELEMENT:    
    element = CreateElement(); break; 
  case ElementType::_CSAMPLER:
    element = CreateCSampler(); break; 
  case ElementType::_DUMP:
    element = CreateDump(); break; 
  case ElementType::_SOLENOID:
    element = CreateSolenoid(); break; 
  case ElementType::_ECOL:
    element = CreateEllipticalCollimator(); break; 
  case ElementType::_RCOL:
    element = CreateRectangularCollimator(); break; 
  case ElementType::_MUSPOILER:    
    element = CreateMuSpoiler(); break;
  case ElementType::_DEGRADER:
    element = CreateDegrader(); break;
  case ElementType::_LASER:
    element = CreateLaser(); break; 
  case ElementType::_SCREEN:
    element = CreateScreen(); break; 
  case ElementType::_AWAKESCREEN:
    element = CreateAwakeScreen(); break; 
  case ElementType::_TRANSFORM3D:
    element = CreateTransform3D(); break;

    // common types, but nothing to do here
  case ElementType::_MARKER:
  case ElementType::_LINE:
  case ElementType::_REV_LINE:
  case ElementType::_MATERIAL:
  case ElementType::_ATOM:
    element = nullptr;
    break;
  default:
#ifdef BDSDEBUG
    G4cout << "BDSComponentFactory: type: " << _element.type << G4endl; 
#endif
    G4Exception("Error: BDSComponentFactory: type not found.", "-1", FatalErrorInArgument, "");   
    exit(1);
    break;
  }

  // note this test will only be reached (and therefore the component registered)
  // if it both didn't exist and has been constructed
  if (element)
    {
      element->SetBiasVacuumList(_element.biasVacuumList);
      element->SetBiasMaterialList(_element.biasMaterialList);
      element->SetPrecisionRegion(_element.precisionRegion);
      element->Initialise();
      BDSAcceleratorComponentRegistry::Instance()->RegisterComponent(element);
    }
  
  return element;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateSampler()
{
  return (new BDSSampler(_element.name));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateCSampler()
{
  if( _element.l < 1.E-4 ) _element.l = 1.0 ;
  return (new BDSSamplerCylinder( _element.name,
				  _element.l * CLHEP::m,
				  _element.r * CLHEP::m ));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateDump()
{
  return (new BDSDump( _element.name,
		       BDSGlobalConstants::Instance()->GetSamplerLength()));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateTeleporter(const G4ThreeVector teleporterDetla)
{
  // This relies on things being added to the beamline immediately
  // after they've been created
  G4double teleporterLength = BDSGlobalConstants::Instance()->GetTeleporterLength() - 1e-8;

  if (teleporterLength < 10*G4GeometryTolerance::GetInstance()->GetSurfaceTolerance())
    {
      G4cout << G4endl << __METHOD_NAME__ << "WARNING - no space to put in teleporter - skipping it!" << G4endl << G4endl;
      return nullptr;
    }
  
  G4String name = "teleporter";
#ifdef BDSDEBUG
    G4cout << "---->creating Teleporter,"
	   << " name = " << name
	   << ", l = " << teleporterLength/CLHEP::m << "m"
	   << G4endl;
#endif

    return( new BDSTeleporter(name,
			      teleporterLength,
			      teleporterDetla));
  
}

BDSAcceleratorComponent* BDSComponentFactory::CreateDrift()
{
  if(!HasSufficientMinimumLength(_element))
    {return nullptr;}

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

BDSAcceleratorComponent* BDSComponentFactory::CreateRF()
{
  if(!HasSufficientMinimumLength(_element))
    {return nullptr;}

  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::rfcavity,
					       _brho,
					       BDSIntegratorType::g4classicalrk4,
					       nullptr,
					       true,
					       G4Transform3D(),
					       PrepareCavityModelInfo(_element));

  return new BDSCavityRF(_element.name,
			 _element.l*CLHEP::m,
			 vacuumField);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateSBend()
{
  if(!HasSufficientMinimumLength(_element))
    {return nullptr;}

  G4double length = _element.l * CLHEP::m;
  BDSMagnetStrength* st = new BDSMagnetStrength();
  if (BDS::IsFinite(_element.B))
    {
      (*st)["field"] = _element.B * CLHEP::tesla;
      (*st)["angle"] = (*st)["field"] * length / _brho;
    }
  else
    {
      G4double ffact = BDSGlobalConstants::Instance()->GetFFact();
      (*st)["field"] = - _brho *  _element.angle / length * _charge * ffact / CLHEP::tesla / CLHEP::m;
      (*st)["angle"] = - _element.angle;
    }
  // Quadrupole component
  if (BDS::IsFinite(_element.k1))
    {(*st)["k1"] = _element.k1 / CLHEP::m2;}

  // Calculate number of sbends to split parent into. If the maximum distance
  // between the arc and chord at the centre is > 1mm, split sbend into N chunks,
  // such that the maximum distance between the chord and the arc is 1mm, ie
  // the maximum possible error on the aperture.
  // This also works when maximum distance is less than 1mm as there will just be 1 chunk!
  double aperturePrecision = 1.0; // in mm
  // from formula: L/2 / N tan (angle/N) < precision. (L=physical length)
  int nSbends = (int) ceil(std::sqrt(std::abs(length*_element.angle/2/aperturePrecision)));
  if (nSbends==0 || BDSGlobalConstants::Instance()->DontSplitSBends())
    {nSbends = 1;} // can happen in case angle = 0
      
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " splitting sbend into " << nSbends << " sbends" << G4endl;
#endif
  // prepare one name for all that makes sense
  std::string thename = _element.name + "_1_of_" + std::to_string(nSbends);
  //calculate their angle and length
  (*st)["angle"] /= (G4double) nSbends; // field stays the same
  G4double semilength = length / (G4double) nSbends;
  //create Line to put them in
  BDSLine* sbendline = new BDSLine(_element.name);
  //create sbends and put them in the line
  BDSBeamPipeInfo*    bpInfo = PrepareBeamPipeInfo(_element);
  BDSMagnetOuterInfo* moInfo = PrepareMagnetOuterInfo(_element);
  moInfo->angle = (*st)["angle"]; // already been set to semiangle by this point

  CheckBendLengthAngleWidthCombo(semilength, (*st)["angle"], moInfo->outerDiameter, thename);

  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::dipole,
					       _brho,
					       BDSIntegratorType::dipole,
					       st);
  
  // prepare one sbend segment
  BDSMagnet* oneBend = new BDSMagnet(BDSMagnetType::sectorbend,
				     thename,
				     semilength,
				     bpInfo,
				     moInfo,
				     vacuumField,
				     nullptr);

  oneBend->SetBiasVacuumList(_element.biasVacuumList);
  oneBend->SetBiasMaterialList(_element.biasMaterialList);
  // create a line of this sbend repeatedly
  for (G4int i = 0; i < nSbends; ++i)
    {sbendline->AddComponent(oneBend);}
  return sbendline;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateRBend()
{
  if(!HasSufficientMinimumLength(_element))
    {return nullptr;}
  
  // calculate length of central straight length and edge sections
  // unfortunately, this has to be duplicated here as we need to
  // calculated the magnetic field length (less than the full length)
  // in case we need to calculate the field
  G4double outerRadius = PrepareOuterDiameter(_element)*0.5;
  G4double angle       = _element.angle;
  G4double chordLength = _element.l*CLHEP::m;
  G4double straightSectionChord = outerRadius / (tan(0.5*fabs(angle)) + tan((0.5*CLHEP::pi) - (0.5*fabs(angle))) );
  G4double magFieldLength = chordLength - (2.0*straightSectionChord);

  CheckBendLengthAngleWidthCombo(chordLength, angle, 2*outerRadius, _element.name);

  BDSMagnetStrength* st = new BDSMagnetStrength();
  if (BDS::IsFinite(_element.B))
    {
      (*st)["field"] = _element.B * CLHEP::tesla;
      (*st)["angle"] = -2.0*asin(magFieldLength*0.5 / (_brho / (*st)["field"]));
      //(*st)["angle"] = (*st)["field"] * length / _brho; // for poleface branch merge
    }
  else
    {
      G4double arclength;
      if (BDS::IsFinite(_element.angle))
	{arclength = 0.5*magFieldLength * fabs(_element.angle) / sin(fabs(_element.angle)*0.5);}
      else
	{arclength = magFieldLength;}
      G4double ffact = BDSGlobalConstants::Instance()->GetFFact();
      (*st)["field"] = - _brho *  _element.angle / arclength * _charge * ffact / CLHEP::tesla;
      (*st)["angle"] = - _element.angle;
    }
  // Quadrupole component
  if (BDS::IsFinite(_element.k1))
    {(*st)["k1"] = _element.k1 / CLHEP::m2;}
  
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::dipole,
					       _brho,
					       BDSIntegratorType::dipole,
					       st);

  BDSMagnetOuterInfo* moInfo = PrepareMagnetOuterInfo(_element);
  moInfo->angle = (*st)["angle"];
  return new BDSRBend(_element.name,
		      _element.l*CLHEP::m,
		      _element.angle,
		      PrepareBeamPipeInfo(_element),
		      moInfo,
		      vacuumField,
		      nullptr);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateKicker(G4bool isVertical)
{
  if(!HasSufficientMinimumLength(_element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  G4double length = _element.l*CLHEP::m;
  if(BDS::IsFinite(_element.B))
    {
      (*st)["field"] = _element.B * CLHEP::tesla;
      (*st)["angle"] = (*st)["field"] * length / _brho;
    }
  else
    {
      G4double ffact = BDSGlobalConstants::Instance()->GetFFact();
      (*st)["angle"] = _element.angle;
      (*st)["field"] = _brho * (*st)["angle"] / length * _charge * ffact / CLHEP::tesla;
    }
  G4Transform3D fieldRotation = G4Transform3D();
  
  BDSMagnetType t = BDSMagnetType::hkicker;
  if (isVertical)
    {
      t = BDSMagnetType::vkicker;
      fieldRotation = G4RotateZ3D(CLHEP::halfpi);
    }
  
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::muonspoiler,
					       _brho,
					       BDSIntegratorType::g4classicalrk4,
					       st,
					       true,
					       fieldRotation);
  
  return new BDSMagnet(t,
		       _element.name,
		       _element.l*CLHEP::m,
		       PrepareBeamPipeInfo(_element),
		       PrepareMagnetOuterInfo(_element),
		       vacuumField,
		       nullptr);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateQuad()
{
  if(!HasSufficientMinimumLength(_element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  (*st)["k1"] = _element.k1 / CLHEP::m2;
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::quadrupole,
					       _brho,
					       BDSIntegratorType::quadrupole,
					       st);

  return new BDSMagnet(BDSMagnetType::quadrupole,
		       _element.name,
		       _element.l * CLHEP::m,
		       PrepareBeamPipeInfo(_element),
		       PrepareMagnetOuterInfo(_element),
		       vacuumField,
		       nullptr);
}  
  
BDSAcceleratorComponent* BDSComponentFactory::CreateSextupole()
{
  if(!HasSufficientMinimumLength(_element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  (*st)["k2"] = _element.k2 / CLHEP::m3;
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::sextupole,
					       _brho,
					       BDSIntegratorType::sextupole,
					       st);

  return new BDSMagnet(BDSMagnetType::sextupole,
		       _element.name,
		       _element.l * CLHEP::m,
		       PrepareBeamPipeInfo(_element),
		       PrepareMagnetOuterInfo(_element),
		       vacuumField,
		       nullptr);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateOctupole()
{
  if(!HasSufficientMinimumLength(_element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  (*st)["k3"] = _element.k3 / (CLHEP::m3*CLHEP::m);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::octupole,
					       _brho,
					       BDSIntegratorType::octupole,
					       st);

  return new BDSMagnet(BDSMagnetType::octupole,
		       _element.name,
		       _element.l * CLHEP::m,
		       PrepareBeamPipeInfo(_element),
		       PrepareMagnetOuterInfo(_element),
		       vacuumField,
		       nullptr);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateDecapole()
{
  if(!HasSufficientMinimumLength(_element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  (*st)["k4"] = _element.k4 / (CLHEP::m3*CLHEP::m2);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::decapole,
					       _brho,
					       BDSIntegratorType::decapole,
					       st);
  

  return new BDSMagnet(BDSMagnetType::decapole,
		       _element.name,
		       _element.l * CLHEP::m,
		       PrepareBeamPipeInfo(_element),
		       PrepareMagnetOuterInfo(_element),
		       vacuumField,
		       nullptr);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateMultipole()
{
 if(!HasSufficientMinimumLength(_element))
    {return nullptr;}

 BDSMagnetStrength* st = new BDSMagnetStrength();
 std::list<double>::iterator kn = _element.knl.begin();
 std::list<double>::iterator ks = _element.ksl.begin();
 std::vector<G4String> normKeys = st->NormalComponentKeys();
 std::vector<G4String> skewKeys = st->SkewComponentKeys();
 std::vector<G4String>::iterator nkey = normKeys.begin();
 std::vector<G4String>::iterator skey = skewKeys.begin();
 for (; kn != _element.knl.end(); kn++, ks++, nkey++, skey++)
   {
     (*st)[*nkey] = (*kn) / _element.l;
     (*st)[*skey] = (*ks) / _element.l;
   }
 BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::multipole,
					       _brho,
					       BDSIntegratorType::g4classicalrk4,
					       st);

 return new BDSMagnet(BDSMagnetType::multipole,
		      _element.name,
		      _element.l * CLHEP::m,
		      PrepareBeamPipeInfo(_element),
		      PrepareMagnetOuterInfo(_element),
		      vacuumField,
		      nullptr);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateElement()
{
  if(!HasSufficientMinimumLength(_element)) 
    {return nullptr;}

  if(!BDS::IsFinite(_element.outerDiameter))
    {
      G4cerr << __METHOD_NAME__ << "\"outerDiameter\" must be set for component named \""
	     << _element.name << "\"" << G4endl;
      exit(1);
    }

  G4ThreeVector bFieldOffset = G4ThreeVector(_element.bmapXOffset * CLHEP::m,
					     _element.bmapYOffset * CLHEP::m,
					     _element.bmapZOffset * CLHEP::m);
#ifdef BDSDEBUG 
  G4cout << "---->creating Element,"
	 << " name = " << _element.name
	 << " l = " << _element.l << "m"
	 << " outerDiameter = "  << _element.outerDiameter << "m"
	 << " B Field Offset = " << bFieldOffset << " m"
	 << " precision region " << _element.precisionRegion
	 << G4endl;
#endif

  return (new BDSElement(_element.name,
			 _element.l * CLHEP::m,
			 _element.outerDiameter * CLHEP::m,
			 _element.geometryFile,
			 _element.bmapFile,
			 bFieldOffset));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateSolenoid()
{
  if(!HasSufficientMinimumLength(_element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  if (BDS::IsFinite(_element.B))
    {
      (*st)["field"] = _element.B * CLHEP::tesla;
      (*st)["ks"]    = (*st)["field"] / _brho / CLHEP::m;
    }
  else
    {
      (*st)["field"] = (_element.ks / CLHEP::m) * _brho;
      (*st)["ks"]    = _element.ks / CLHEP::m;
    }
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::solenoid,
					       _brho,
					       BDSIntegratorType::solenoid,
					       st);

  return new BDSMagnet(BDSMagnetType::solenoid,
		       _element.name,
		       _element.l*CLHEP::m,
		       PrepareBeamPipeInfo(_element),
		       PrepareMagnetOuterInfo(_element),
		       vacuumField,
		       nullptr);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateRectangularCollimator()
{
  if(!HasSufficientMinimumLength(_element))
    {return nullptr;}

#ifdef BDSDEBUG 
  G4cout << "--->creating " << _element.type << ","
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
                      _element.xsizeOut*CLHEP::m,
                      _element.ysizeOut*CLHEP::m,
				      _element.material);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateEllipticalCollimator()
{
  if(!HasSufficientMinimumLength(_element))
    {return nullptr;}

#ifdef BDSDEBUG 
  G4cout << "--->creating " << _element.type << ","
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
				     _element.xsizeOut*CLHEP::m,
				     _element.ysizeOut*CLHEP::m,
				     _element.material);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateMuSpoiler()
{
  if(!HasSufficientMinimumLength(_element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  (*st)["field"] = _element.B * CLHEP::tesla;
  BDSFieldInfo* outerField = new BDSFieldInfo(BDSFieldType::muonspoiler,
					      _brho,
					      BDSIntegratorType::g4classicalrk4,
					      st);

  return new BDSMagnet(BDSMagnetType::muonspoiler,
		       _element.name,
		       _element.l*CLHEP::m,
		       PrepareBeamPipeInfo(_element),
		       PrepareMagnetOuterInfo(_element),
		       nullptr,
		       outerField);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateDegrader()
{
  if(!HasSufficientMinimumLength(_element))
    {return nullptr;}

#ifdef BDSDEBUG
  G4cout << "---->creating degrader,"
	 << " name = "   << _element.name
	 << " length = " << _element.l
	 << G4endl;
#endif
  

  G4double degraderOffset;
    
  if ((_element.materialThickness <= 0) && (_element.degraderOffset <= 0))
    {
        G4cerr << __METHOD_NAME__ << "Error: Both \"materialThickness\" and \"degraderOffset\" are either undefined or <= 0" <<  G4endl;
        exit(1);
    }

  if ((_element.materialThickness <= 0) && (_element.degraderOffset > 0))
    {
        degraderOffset = _element.degraderOffset*CLHEP::m;
    }
    
  else
    {
        //Width of wedge base
        G4double wedgeBasewidth = (_element.l*CLHEP::m /_element.numberWedges) - lengthSafety;
        
        //Angle between hypotenuse and height (in the triangular wedge face)
        G4double theta = atan(wedgeBasewidth / (2.0*_element.wedgeLength*CLHEP::m));
        
        //Overlap distance of wedges
        G4double overlap = (_element.materialThickness*CLHEP::m/_element.numberWedges - wedgeBasewidth) * (sin(CLHEP::pi/2.0 - theta) / sin(theta));

        degraderOffset = overlap * -0.5;
        
    }
    
  return (new BDSDegrader(_element.name,
			  _element.l*CLHEP::m,
			  _element.outerDiameter*CLHEP::m,
			  _element.numberWedges,
			  _element.wedgeLength*CLHEP::m,
			  _element.degraderHeight*CLHEP::m,
			  degraderOffset,
			  _element.material));

}

BDSAcceleratorComponent* BDSComponentFactory::CreateLaser()
{
  if(!HasSufficientMinimumLength(_element))
    {return nullptr;}
	
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
	
  G4ThreeVector direction = G4ThreeVector(_element.xdir,_element.ydir,_element.zdir);
  G4ThreeVector position  = G4ThreeVector(0,0,0);
	
  return new BDSLaserWire(_element.name, length, lambda, direction);       
}

BDSAcceleratorComponent* BDSComponentFactory::CreateScreen()
{
  if(!HasSufficientMinimumLength(_element))
    {return nullptr;}
	
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
  //Name, scintillator thickness, angle in radians (relative to -45 degrees)
  return new BDSScintillatorScreen(_element.name,
				   _element.tscint*CLHEP::m,
				   (_element.angle-0.78539816339)*CLHEP::rad,
				   "ups923a",
				   BDSGlobalConstants::Instance()->GetVacuumMaterial()); 
}


BDSAcceleratorComponent* BDSComponentFactory::CreateAwakeScreen()
{	
#ifdef BDSDEBUG 
  G4cout << "---->creating Awake Screen,"
	 << "twindow = " << _element.twindow*1e3/CLHEP::um << " um"
	 << "tscint = " << _element.tscint*1e3/CLHEP::um << " um"
	 << "windowmaterial = " << _element.windowmaterial << " um"
	 << "scintmaterial = " << _element.scintmaterial << " um"
	 << G4endl;
#endif
  return new BDSAwakeScintillatorScreen(_element.name,
					_element.scintmaterial,
					_element.tscint*1e3,
					_element.angle,
					_element.twindow*1e3,
					_element.windowmaterial);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateTransform3D()
{	
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
	
  return new BDSTransform3D(_element.name,
			    _element.xdir  *CLHEP::m,
			    _element.ydir  *CLHEP::m,
			    _element.zdir  *CLHEP::m,
			    _element.phi   *CLHEP::rad,
			    _element.theta *CLHEP::rad,
			    _element.psi   *CLHEP::rad);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateTerminator()
{
  G4String name   = "terminator";
  G4double length = BDSGlobalConstants::Instance()->GetSamplerLength();
#ifdef BDSDEBUG
    G4cout << "---->creating Terminator,"
	   << " name = " << name
	   << " l = "    << length / CLHEP::m << "m"
	   << G4endl;
#endif
  
  return new BDSTerminator("terminator", 
			   length);
}


G4bool BDSComponentFactory::HasSufficientMinimumLength(Element& element)
{
  if(element.l*CLHEP::m < 4*lengthSafety)
    {
      G4cerr << "---->NOT creating element, "
             << " name = " << _element.name
             << ", LENGTH TOO SHORT:"
             << " l = " << _element.l*CLHEP::um << "um"
             << G4endl;
      return false;
    }
  else
    {return true;}
}

BDSMagnetOuterInfo* BDSComponentFactory::PrepareMagnetOuterInfo(Element& element)
{
  BDSMagnetOuterInfo* info = new BDSMagnetOuterInfo();

  // angle - we can't set here as we can't rely on the angle being specified in element
  // as only the field may be specified. Therefore, must be set in above CreateXXXX methods
  
  // name
  info->name = element.name;
  
  // magnet geometry type
  if (element.magnetGeometryType == "")
    info->geometryType = BDSGlobalConstants::Instance()->GetMagnetGeometryType();
  else
    info->geometryType = BDS::DetermineMagnetGeometryType(element.magnetGeometryType);

  // outer diameter
  G4double outerDiameter = element.outerDiameter*CLHEP::m;
  if (outerDiameter < 1e-6)
    {//outerDiameter not set - use global option as default
      outerDiameter = BDSGlobalConstants::Instance()->GetOuterDiameter();
    }
  info->outerDiameter = outerDiameter;

  // outer material
  G4Material* outerMaterial;
  if(element.outerMaterial == "")
    {
      G4String defaultMaterialName = BDSGlobalConstants::Instance()->GetOuterMaterialName();
      outerMaterial = BDSMaterials::Instance()->GetMaterial(defaultMaterialName);
    }
  else
    {outerMaterial = BDSMaterials::Instance()->GetMaterial(element.outerMaterial);}
  info->outerMaterial = outerMaterial;
  
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
  BDSBeamPipeInfo* defaultModel = BDSGlobalConstants::Instance()->GetDefaultBeamPipeModel();
  BDSBeamPipeInfo* info = new BDSBeamPipeInfo(defaultModel,
					      element.apertureType,
					      element.aper1 * CLHEP::m,
					      element.aper2 * CLHEP::m,
					      element.aper3 * CLHEP::m,
					      element.aper4 * CLHEP::m,
					      element.vacuumMaterial,
					      element.beampipeThickness * CLHEP::m,
					      element.beampipeMaterial);  
  return info;
}

BDSTiltOffset* BDSComponentFactory::CreateTiltOffset(Element& element)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "offsetX,Y: " << element.offsetX << " " << element.offsetY << " tilt: " << element.tilt << G4endl;
#endif
  G4double xOffset = element.offsetX * CLHEP::m;
  G4double yOffset = element.offsetY * CLHEP::m;
  G4double tilt    = element.tilt;

  return new BDSTiltOffset(xOffset, yOffset, tilt);
}

void BDSComponentFactory::CheckBendLengthAngleWidthCombo(G4double chordLength,
							 G4double angle,
							 G4double outerDiameter,
							 G4String name)
{
  G4double radiusFromAngleLength =  std::abs(chordLength / angle); // s = r*theta -> r = s/theta
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "radius from angle and length: " << radiusFromAngleLength << G4endl;
#endif
  if (outerDiameter > 2*radiusFromAngleLength)
    {
      G4cerr << "Error: the combination of length, angle and outerDiameter in element named \""
	     << name
	     << "\" will result in overlapping faces!" << G4endl << "Please correct!" << G4endl;
      exit(1);
    }
}

void BDSComponentFactory::PrepareCavityModels()
{
  for (auto model : BDSParser::Instance()->GetCavityModels())
    {
      auto info = new BDSCavityInfo(BDS::DetermineCavityType(model.type),
				    nullptr, //construct without material as stored in element
				    nullptr,
				    model.frequency, // TBC - units
				    model.phase,
				    model.irisRadius*CLHEP::m,
				    model.thickness*CLHEP::m,
				    model.equatorRadius*CLHEP::m,
				    model.halfCellLength*CLHEP::m,
				    model.numberOfPoints,
				    model.numberOfCells,
				    model.equatorEllipseSemiAxis*CLHEP::m,
				    model.irisHorizontalAxis*CLHEP::m,
				    model.irisVerticalAxis*CLHEP::m,
				    model.tangentLineAngle);
      
      cavityInfos[model.name] = info;
    }
}

BDSCavityInfo* BDSComponentFactory::PrepareCavityModelInfo(const Element& element)
{
  // If the cavity model name (identifier) has been defined, return a *copy* of
  // that model - so that the component will own that info object.
  auto result = cavityInfos.find(element.cavityModel);
  if (result == cavityInfos.end())
    {
      G4cout << "Unknown cavity model identifier \"" << element.cavityModel << "\" - please define it" << G4endl;
      exit(1);
    }

  // ok to use compiler provided copy constructor as doesn't own materials
  // which are the only pointers in this class
  BDSCavityInfo* info = new BDSCavityInfo(*(result->second));
  // update materials in info with valid materials - only element has material info
  if (!element.material.empty())
    {info->material       = BDSMaterials::Instance()->GetMaterial(element.material);}
  else
    {
      G4cout << "ERROR: Cavity material is not defined for cavity \"" << element.name << "\" - please define it" << G4endl;
      exit(1);
    }
  if(!element.vacuumMaterial.empty())
    {info->vacuumMaterial = BDSMaterials::Instance()->GetMaterial(element.vacuumMaterial);}
  else
    {info->vacuumMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial());}

  return info;
}
