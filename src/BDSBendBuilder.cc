#include "globals.hh" // geant4 globals / types
#include "G4RotationMatrix.hh"

#include "BDSAcceleratorComponent.hh"
#include "BDSAcceleratorComponentRegistry.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSBendBuilder.hh"
#include "BDSComponentFactory.hh"
#include "BDSDebug.hh"
#include "BDSFieldType.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIntegratorSet.hh"
#include "BDSIntegratorType.hh"
#include "BDSLine.hh"
#include "BDSMagnet.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSUtilities.hh"

#include "parser/element.h"
#include "parser/elementtype.h"

class BDSBeamPipeInfo;
class BDSMagnetStrengh;

using namespace GMAD;

BDSLine* BDS::BuildSBendLine(Element*           element,
			     G4double           angleIn,
			     G4double           angleOut,
			     BDSMagnetStrength* st,
			     G4double           brho,
			     const BDSIntegratorSet* integratorSet)
{
  G4bool       includeFringe = BDSGlobalConstants::Instance()->IncludeFringeFields();
  G4double thinElementLength = BDSGlobalConstants::Instance()->ThinElementLength();
  
  // Calculate number of sbends to split parent into
  G4int nSBends = BDS::CalculateNSBendSegments(element);
  
  //Zero angle bend only needs one element.
  std::string thename = element->name + "_1_of_1";
  
  G4double length = element->l * CLHEP::m;
  //copy of angle
  G4double angle = (*st)["angle"];
  
  (*st)["length"] = length/nSBends;
  (*st)["angle"]  = angle/nSBends;  // override copied length and angle
  
  BDSLine* sbendline  = new BDSLine(element->name);
  
  // Single element if no poleface and zero bend angle or dontSplitSBends=1, therefore nSBends = 1
  if (!BDS::IsFinite(angle) || (nSBends == 1))
    {
      BDSIntegratorType intType = integratorSet->Integrator(BDSFieldType::dipole);
      BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::dipole,
						   brho,
						   intType,
						   st);
      // prepare one sbend segment
      auto bpInfo = BDSComponentFactory::PrepareBeamPipeInfo(element, -angleIn, -angleOut);
      auto mgInfo = BDSComponentFactory::PrepareMagnetOuterInfo(element, st, -angleIn, -angleOut);
      BDSMagnet* oneBend = new BDSMagnet(BDSMagnetType::sectorbend,
					 thename,
					 length,
					 bpInfo,
					 mgInfo,
					 vacuumField,
					 angle);
      
      oneBend->SetBiasVacuumList(element->biasVacuumList);
      oneBend->SetBiasMaterialList(element->biasMaterialList);
      
      sbendline->AddComponent(oneBend);
      
      return sbendline;
    }
  
  //calculate their angles and length
  G4double semiangle  = -element->angle / (G4double) nSBends;
  G4double semilength = length / (G4double) nSBends;
  G4double rho        = element->l*CLHEP::m/element->angle;
  
  angle      = element->angle;
  angleIn    = element->e1*CLHEP::rad;
  angleOut   = element->e2*CLHEP::rad;
  
  G4double zExtentIn  = 0;
  G4double zExtentOut = 0;
  G4bool fadeIn = true;
  G4bool fadeOut = true;

  G4double outerDiameter = element->outerDiameter*CLHEP::m;
  if (outerDiameter < 1e-6)
    {//outerDiameter not set - use global option as default
      outerDiameter = BDSGlobalConstants::Instance()->OuterDiameter();
    }
  
  //calculate extent along z due poleface rotation
  if (angleIn > 0)
    {zExtentIn = 0.5*outerDiameter*tan(angleIn - 0.5*std::abs(semiangle));}
  else if (angleIn < 0)
    {zExtentIn = 0.5*outerDiameter*tan(0.5*std::abs(semiangle) + angleIn);}
  if (angleOut > 0)
    {zExtentOut = 0.5*outerDiameter*tan(angleOut - 0.5*std::abs(semiangle));}
  else if (angleOut < 0)
    {zExtentOut = 0.5*outerDiameter*tan(0.5*std::abs(semiangle) + angleOut);}
  
  //decide if wedge angles fade or not depending on the extents
  if (std::abs(zExtentIn) < semilength/4.0)
    {fadeIn = false;}
  if (std::abs(zExtentOut) < semilength/4.0)
    {fadeOut = false;}
  
  //if the element faces fade in, the middle wedges should be numbered as such
  //if not, it'll be repeated from the first segment onwards
  G4int centralWedgeNum = 0.5*(nSBends+1);
  G4String centralName = element->name;
  if (fadeIn)
    {thename += "_"+std::to_string(centralWedgeNum)+"_of_" + std::to_string(nSBends);}
  else
    {thename += "_1_of_" + std::to_string(nSBends);}
  
  // register the central wedge which will always be used as the
  // middle wedge regardless of poleface rotations

  BDSIntegratorType intType = integratorSet->Integrator(BDSFieldType::dipole);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::dipole,
					       brho,
					       intType,
					       st);

  auto bpInfo = BDSComponentFactory::PrepareBeamPipeInfo(element, -0.5*semiangle, -0.5*semiangle);
  auto mgInfo = BDSComponentFactory::PrepareMagnetOuterInfo(element, st, -0.5*semiangle, -0.5*semiangle);
  BDSMagnet* centralWedge = new BDSMagnet(BDSMagnetType::sectorbend,
					  centralName,
					  semilength,
					  bpInfo,
					  mgInfo,
					  vacuumField,
					  semiangle);
  
  centralWedge->SetBiasVacuumList(element->biasVacuumList);
  centralWedge->SetBiasMaterialList(element->biasMaterialList);
  centralWedge->SetRegion(element->region);
  centralWedge->Initialise();
  BDSAcceleratorComponentRegistry::Instance()->RegisterComponent(centralWedge,false);
  
  //oneBend can be accComp or BDSMagnet depending on registration/reusage or new magnet
  BDSAcceleratorComponent *oneBend = nullptr;
  
  BDSMagnetType magType = BDSMagnetType::sectorbend;
  // check magnet outer info
  BDSMagnetOuterInfo* magnetOuterInfoCheck = BDSComponentFactory::PrepareMagnetOuterInfo(element,st,angleIn,angleOut);
  BDSComponentFactory::CheckBendLengthAngleWidthCombo(semilength, semiangle,
						      magnetOuterInfoCheck->outerDiameter,
						      thename);
  // clean up
  delete magnetOuterInfoCheck;
  
  // first element should be fringe if poleface specified
  if (BDS::IsFinite(angleIn) && includeFringe)
    {
      BDSMagnetStrength* fringeStIn  = new BDSMagnetStrength();
      (*fringeStIn)["field"]         = (*st)["field"];
      (*fringeStIn)["length"]        = thinElementLength;
      (*fringeStIn)["angle"]         = -thinElementLength/rho;
      (*fringeStIn)["polefaceangle"] = element->e1;
      (*fringeStIn)["fringecorr"]    = CalculateFringeFieldCorrection(rho,element->e1,element->fint);
      thename                        = element->name + "_e1_fringe";
      angle                          = -element->e1 - 0.5*((*fringeStIn)["angle"]);
      BDSMagnet* startfringe = BDS::BuildDipoleFringe(element, angle, -angle,
						      thename, magType, fringeStIn, brho,
						      integratorSet);
      sbendline->AddComponent(startfringe);
    }
  
  //logic for wedge elements in the beamline:
  //  reuse central wedge for all wedges of in/out half if no poleface angle(s)
  //  if small poleface, new first/last wedge, reuse central wedge for remainder of in/out half
  //  otherwise fade in/out faces for all wedges in app. halves.
  
  for (int i = 0; i < nSBends; ++i)
    {
      G4String thename = element->name + "_"+std::to_string(i+1)+"_of_" + std::to_string(nSBends);
      if (BDSAcceleratorComponentRegistry::Instance()->IsRegistered(thename))
        {oneBend = BDSAcceleratorComponentRegistry::Instance()->GetComponent(thename);}
      else if (i < 0.5*(nSBends-1))
        {
          if (!BDS::IsFinite(element->e1))
            {oneBend = BDSAcceleratorComponentRegistry::Instance()->GetComponent(centralName);}
          else if (fadeIn)
            {
              oneBend = BDS::BuildSBendWedge(element, fadeIn, fadeOut, i, nSBends, st, brho, integratorSet);
              BDSAcceleratorComponentRegistry::Instance()->RegisterComponent(oneBend,false);
            }
          else
            {
              if (i == 0)
                {
                  oneBend = BDS::BuildSBendWedge(element, fadeIn, fadeOut, i, nSBends, st, brho, integratorSet);
                  BDSAcceleratorComponentRegistry::Instance()->RegisterComponent(oneBend,false);
                }
              else
                {oneBend = BDSAcceleratorComponentRegistry::Instance()->GetComponent(centralName);}
            }
        }
      else if (i > 0.5*(nSBends-1))
        {
          if (!BDS::IsFinite(element->e2))
            {oneBend = BDSAcceleratorComponentRegistry::Instance()->GetComponent(centralName);}
          else if (fadeOut)
            {
              oneBend = BDS::BuildSBendWedge(element, fadeIn, fadeOut, i, nSBends, st, brho, integratorSet);
              BDSAcceleratorComponentRegistry::Instance()->RegisterComponent(oneBend,false);
            }
          else
            {
              if (i == (nSBends-1))
                {
                  oneBend = BDS::BuildSBendWedge(element, fadeIn, fadeOut, i, nSBends, st, brho, integratorSet);
                  BDSAcceleratorComponentRegistry::Instance()->RegisterComponent(oneBend,false);
                }
              else
                {oneBend = BDSAcceleratorComponentRegistry::Instance()->GetComponent(centralName);}
            }
        }
      else
        {oneBend = BDSAcceleratorComponentRegistry::Instance()->GetComponent(centralName);}
      
      oneBend->SetBiasVacuumList(element->biasVacuumList);
      oneBend->SetBiasMaterialList(element->biasMaterialList);
      sbendline->AddComponent(oneBend);
      
#ifdef BDSDEBUG
      G4cout << "---->creating sbend line,"
	     << " element= " << thename
	     << " angleIn= " << angleIn
	     << " angleOut= " << angleOut << "m"
	     << G4endl;
#endif
    }
  
  //Last element should be fringe if poleface specified
  if (BDS::IsFinite(element->e2) && includeFringe)
    {
      BDSMagnetStrength* fringeStOut  = new BDSMagnetStrength();
      (*fringeStOut)["angle"]         = -thinElementLength/rho;
      (*fringeStOut)["field"]         = (*st)["field"];
      (*fringeStOut)["polefaceangle"] = element->e2;
      (*fringeStOut)["fringecorr"]    = CalculateFringeFieldCorrection(rho,element->e2,element->fintx);
      (*fringeStOut)["length"]        = thinElementLength;
      angle                           = element->e2+ 0.5*((*fringeStOut)["angle"]);
      thename                         = element->name + "_e2_fringe";
      
      BDSMagnet* endfringe = BDS::BuildDipoleFringe(element, angle, -angle,
						    thename, magType, fringeStOut, brho,
						    integratorSet);
      sbendline->AddComponent(endfringe);
    }
  return sbendline;
}

BDSLine* BDS::BuildRBendLine(Element*           element,
			     Element*           prevElement,
			     Element*           nextElement,
			     G4double           angleIn,
			     G4double           angleOut,
			     G4double           brho,
			     BDSMagnetStrength* st,
			     const BDSIntegratorSet* integratorSet)
{
  G4bool       includeFringe = BDSGlobalConstants::Instance()->IncludeFringeFields();
  G4double thinElementLength = BDSGlobalConstants::Instance()->ThinElementLength();
  BDSLine* rbendline  = new BDSLine(element->name);
  
  BDSComponentFactory::PoleFaceRotationsNotTooLarge(element);
  
  G4double angle       = element->angle;
  G4double length      = element->l*CLHEP::m;
  G4String thename     = element->name;
  G4double rho         = element->l*CLHEP::m/element->angle;
  G4bool prevModifies  = false;
  G4bool nextModifies  = false;
  
  BDSMagnetType magType = BDSMagnetType::rectangularbend;
  
  // booleans for modification by previous/next element
  if ((prevElement) && (prevElement->type == ElementType ::_RBEND))
    {prevModifies = true;}
  if ((nextElement) && (nextElement->type == ElementType ::_RBEND))
    {nextModifies = true;}
  
  // poleface angles
  G4double polefaceAngleIn = element->e1 + 0.5*(length-thinElementLength)/rho;
  G4double polefaceAngleOut = element->e2 + 0.5*(length-thinElementLength)/rho;
  
  // poleface angles and main element angles are modified if next/previous is an rbend
  if ((prevElement) && (prevElement->type == ElementType::_RBEND))
    {
      polefaceAngleIn -= 0.5*element->angle;
      angleIn += 0.5*(thinElementLength)/rho;
    }
  if ((nextElement) && (nextElement->type == ElementType::_RBEND))
    {
      polefaceAngleOut -= 0.5*element->angle;
      angleOut += 0.5*(thinElementLength)/rho;
    }
  
  // first element should be fringe if poleface specified
  if (BDS::IsFinite(element->e1) && includeFringe &&(!prevModifies))
    {
      BDSMagnetStrength* fringeStIn  = new BDSMagnetStrength();
      (*fringeStIn)["field"]         = (*st)["field"];
      (*fringeStIn)["polefaceangle"] = element->e1;
      (*fringeStIn)["length"]        = thinElementLength;
      (*fringeStIn)["angle"]         = -thinElementLength/rho;
      (*fringeStIn)["fringecorr"]    = CalculateFringeFieldCorrection(rho,element->e1,element->fint);
      thename                        = element->name + "_e1_fringe";
      angle                          = polefaceAngleIn;
      
      BDSMagnet* startfringe = BDS::BuildDipoleFringe(element, -angle, angle,
						      thename, magType, fringeStIn, brho,
						      integratorSet);
      rbendline->AddComponent(startfringe);
    }
  
  // subtract thinElementLength from main rbend element if fringe & poleface(s) specified
  if (BDS::IsFinite(element->e1) && includeFringe && (!prevModifies))
    {
      length   -= thinElementLength;
      angleIn  += 0.5*(thinElementLength)/rho;
      angleOut -= 0.5*(thinElementLength)/rho;
    }
  if (BDS::IsFinite(element->e2) && includeFringe && (!nextModifies))
    {
      length   -= thinElementLength;
      angleOut += 0.5*(thinElementLength)/rho;
      angleIn  -= 0.5*(thinElementLength)/rho;
    }
  
  angle = -length/rho;
  
  //change angle in the case that the next/prev element modifies
  if (nextModifies)
    {angleOut -= 0.5*(thinElementLength)/rho;}
  if (prevModifies)
    {angleIn  -= 0.5*(thinElementLength)/rho;}
  
  // override copied length and angle
  (*st)["length"] = length;
  (*st)["angle"]  = angle;

  BDSIntegratorType intType = integratorSet->Integrator(BDSFieldType::dipole);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::dipole,
					       brho,
					       intType,
					       st);

  auto bpInfo = BDSComponentFactory::PrepareBeamPipeInfo(element, angleIn, angleOut);
  auto mgInfo = BDSComponentFactory::PrepareMagnetOuterInfo(element, st, angleIn, angleOut);
  BDSMagnet* oneBend = new BDSMagnet(magType,
				     element->name,
				     length,
				     bpInfo,
				     mgInfo,
				     vacuumField,
				     angle,
				     nullptr);
  
  rbendline->AddComponent(oneBend);
  
  //Last element should be fringe if poleface specified
  if (BDS::IsFinite(element->e2) && includeFringe && (!nextModifies))
    {
      BDSMagnetStrength* fringeStOut  = new BDSMagnetStrength();
      (*fringeStOut)["field"]         = (*st)["field"];
      (*fringeStOut)["polefaceangle"] = element->e2;
      (*fringeStOut)["length"]        = thinElementLength;
      (*fringeStOut)["angle"]         = -thinElementLength / rho;
      (*fringeStOut)["fringecorr"]    = CalculateFringeFieldCorrection(rho,element->e2,element->fintx);
      thename                         = element->name + "_e2_fringe";
      angle                           = polefaceAngleOut;
      
      BDSMagnet* endfringe = BDS::BuildDipoleFringe(element, angle, -angle,
						    thename, magType, fringeStOut, brho,
						    integratorSet);
      rbendline->AddComponent(endfringe);
    }
  
  return rbendline;
}

BDSMagnet* BDS::BuildDipoleFringe(GMAD::Element*     element,
				  G4double           angleIn,
				  G4double           angleOut,
				  G4String           name,
				  BDSMagnetType      magType,
				  BDSMagnetStrength* st,
				  G4double           brho,
				  const BDSIntegratorSet* integratorSet)
{
  BDSBeamPipeInfo* beamPipeInfo = BDSComponentFactory::PrepareBeamPipeInfo(element, angleIn, angleOut);
  beamPipeInfo->beamPipeType = BDSBeamPipeType::circularvacuum;
  auto magnetOuterInfo = BDSComponentFactory::PrepareMagnetOuterInfo(element, st, angleIn, angleOut);
  magnetOuterInfo->geometryType = BDSMagnetGeometryType::none;

  //magnet total vertical aperture size
  G4double vertGap = 0;
  if (element->apertureType == "racetrack")
    {vertGap = beamPipeInfo->aper3 + beamPipeInfo->aper2;}
  else if ((element->apertureType == "circular") || (element->apertureType ==""))
    {vertGap = beamPipeInfo->aper1;}
  else
    {vertGap = beamPipeInfo->aper2;}
  (*st)["fringecorr"] *= vertGap;


  BDSIntegratorType intType = integratorSet->dipolefringe;
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::dipole,
					       brho,
					       intType,
					       st);

  return new BDSMagnet(magType,
		       name,
		       (*st)["length"],
		       beamPipeInfo,
		       magnetOuterInfo,
		       vacuumField,
		       (*st)["angle"],
		       nullptr);
}

G4int BDS::CalculateNSBendSegments(GMAD::Element const* element,
				   const G4double aperturePrecision)
{
  // Split a bend into equal segments such that the maximum distance between the
  // chord and arc is 1mm.

  G4double length = element->l*CLHEP::m;
  // from formula: L/2 / N tan (angle/N) < precision. (L=physical length)
  // add poleface rotations onto angle as absolute number (just to be safe)
  G4double totalAngle = std::abs(element->angle) + std::abs(element->e1) + std::abs(element->e2);
  G4int nSBends = (G4int) ceil(std::sqrt(length*totalAngle/2/aperturePrecision));
  if (nSBends==0)
    {nSBends = 1;} // can happen in case angle = 0
  if (BDSGlobalConstants::Instance()->DontSplitSBends())
    {nSBends = 1;}  // use for debugging
  if (nSBends % 2 == 0)
    {nSBends += 1;} // always have odd number of poles for poleface rotations
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " splitting sbend into " << nSBends << " sbends" << G4endl;
#endif
  return nSBends;
}

BDSMagnet* BDS::BuildSBendWedge(Element*           element,
				G4bool             fadeIn,
				G4bool             fadeOut,
				G4int              index,
				G4int              nSBends,
				BDSMagnetStrength* st,
				G4double           brho,
				const BDSIntegratorSet* integratorSet)
{
  G4bool       includeFringe = BDSGlobalConstants::Instance()->IncludeFringeFields();
  G4double thinElementLength = BDSGlobalConstants::Instance()->ThinElementLength();
  
  //calculate their angles and length
  G4double length     = element->l*CLHEP::m;
  G4double semiangle  = -element->angle / (G4double) nSBends;
  G4double semilength = length / (G4double) nSBends;
  G4double rho        = element->l*CLHEP::m/element->angle;
  
  // angle increment for sbend elements with poleface rotation(s) specified
  G4double deltastart = -element->e1/(0.5*(nSBends-1));
  G4double deltaend   = -element->e2/(0.5*(nSBends-1));

  G4String thename = element->name + "_"+std::to_string(index+1)+"_of_" + std::to_string(nSBends);
  
  // subtract thinElementLength from first and last elements if fringe & poleface specified
  length = semilength;
  if ((BDS::IsFinite(element->e1)) && (index == 0) && includeFringe)
    {length -= thinElementLength;}
  if ((BDS::IsFinite(element->e2)) && (index == nSBends-1) && includeFringe)
    {length -= thinElementLength;}
  semiangle = -length/rho;
  
  G4double angleIn  = 0;
  G4double angleOut = 0;
  
  // Input and output angles added to or subtracted from the default as appropriate
  // Note: case of i == 0.5*(nSBends-1) is just the default central wedge.
  // More detailed methodology/reasons in developer manual
  if ((BDS::IsFinite(element->e1))||(BDS::IsFinite(element->e2)))
    {
      if (index < 0.5*(nSBends-1))
	{
	  angleIn  = -semiangle*0.5 - (element->e1 + (index*deltastart));
	  angleOut = -semiangle*0.5 - ((0.5*(nSBends-3)-index)*deltastart);
	}
      else if (index > 0.5*(nSBends-1))
	{
	  angleIn  = -semiangle*0.5 + (0.5*(nSBends+1)-index)*deltaend;
	  angleOut = -semiangle*0.5 - (0.5*(nSBends-1)-index)*deltaend;
	}
    }
  
  if ((BDS::IsFinite(element->e1)) && (index == 0) && includeFringe)
    {angleIn += thinElementLength/rho;}
  if ((BDS::IsFinite(element->e2)) && (index == nSBends-1) && includeFringe)
    {angleOut += thinElementLength/rho;}
  
  //set face angles to default if faces do not fade.
  if (!fadeIn && (index == 0))
    {angleOut = -0.5*semiangle;}
  if (!fadeOut && (index == (nSBends-1)))
    {angleIn = -0.5*semiangle;}
  
  // Check for intersection of angled faces.
  G4double intersectionX = BDS::CalculateFacesOverlapRadius(angleIn,angleOut,semilength);
  auto  magnetOuterInfo = BDSComponentFactory::PrepareMagnetOuterInfo(element, st, angleIn, angleOut);
  G4double magnetRadius= 0.625*magnetOuterInfo->outerDiameter*CLHEP::mm;
  // Every geometry type has a completely arbitrary factor of 1.25 except cylindrical
  if (magnetOuterInfo->geometryType == BDSMagnetGeometryType::cylindrical)
    {magnetRadius= 0.5*magnetOuterInfo->outerDiameter*CLHEP::mm;}
  
  //Check if intersection is within radius
  if ((BDS::IsFinite(intersectionX)) && (std::abs(intersectionX) < magnetRadius))
    {
      G4cerr << __METHOD_NAME__ << "Angled faces of element "<< thename
	     << " intersect within the magnet radius." << G4endl;
      exit(1);
    }
  
  BDSMagnetStrength* stSemi = new BDSMagnetStrength(*st); // copy field strength - ie B
  (*stSemi)["length"] = length;
  (*stSemi)["angle"]  = semiangle;  // override copied length and angle

  BDSIntegratorType intType = integratorSet->Integrator(BDSFieldType::dipole);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::dipole,
					       brho,
					       intType,
					       stSemi);

  auto bpInfo = BDSComponentFactory::PrepareBeamPipeInfo(element, angleIn, angleOut);
  BDSMagnet* oneBend = new BDSMagnet(BDSMagnetType::sectorbend,
				     thename,
				     length,
				     bpInfo,
				     magnetOuterInfo,
				     vacuumField,
				     semiangle);
  
  return oneBend;
}

G4double BDS::CalculateFringeFieldCorrection(G4double rho,
					     G4double polefaceAngle,
					     G4double fint)
{
  G4double term1 = fint/rho;
  G4double term2 = (1.0 + pow(sin(polefaceAngle),2)) / cos(polefaceAngle);
  G4double corrValue = term1*term2;
  return corrValue;
}
