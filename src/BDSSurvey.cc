#include "BDSSurvey.hh"

#include "BDSAcceleratorComponent.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSDebug.hh"

#include "parser/element.h"

#include <fstream>
#include <iomanip>
#include <ctime>
#include <string>

using std::setw;

BDSSurvey::BDSSurvey(G4String filename)
{
  G4cout << __METHOD_NAME__ << "Generating Survey: " << filename << " ..." << G4endl;

  survey.open(filename);
}

BDSSurvey::~BDSSurvey()
{}

void BDSSurvey::WriteHeader()
{
  time_t currenttime;
  time(&currenttime);
  std::string timestring = asctime(localtime(&currenttime));
  timestring = timestring.substr(0,timestring.size()-1);

  survey << "### BDSIM output - created "<< timestring << G4endl;
  survey << std::left 
	 << setw(15) << "Type        " << " "
	 << setw(40) << "Name        " << " "
	 << setw(12) << "SStart[m]   " << " "
	 << setw(12) << "SMid[m]     " << " "
	 << setw(12) << "SEnd[m]     " << " "
	 << setw(12) << "Chord_len[m]" << " "
	 << setw(12) << "Arc_len[m]  " << " "
	 << setw(12) << "X[m]        " << " "
	 << setw(12) << "Y[m]        " << " "
	 << setw(12) << "Z[m]        " << " "
	 << setw(12) << "Phi[rad]    " << " "
	 << setw(12) << "Theta[rad]  " << " "
	 << setw(12) << "Psi[rad]    " << " "
	 << setw(12) << "Aper1[m]    " << " "
	 << setw(12) << "Aper2[m]    " << " "
         << setw(12) << "Aper3[m]    " << " "
         << setw(12) << "Aper4[m]    " << " "
	 << setw(15) << "Aper_Type   " << " "
	 << setw(12) << "Angle[rad]  " << " "
	 << setw(12) << "K1[m^-2]    " << " "
	 << setw(12) << "K2[m^-3]    " << " "
	 << setw(12) << "K3[m^-4]    " << " "
	 << setw(12) << "K4[m^-5]    " << " "
	 << G4endl;

  survey.setf(std::ios::fixed, std::ios::floatfield);
  survey.setf(std::ios::showpoint);
  // print up to 7 significant numbers
  survey.precision(7);      
}

void BDSSurvey::Write(std::vector<BDSBeamlineElement*> components, GMAD::Element& element)
{
  for (auto beamlineElement : components)
    {Write(beamlineElement, element);}
}

void BDSSurvey::Write(BDSBeamlineElement* beamlineElement, GMAD::Element& element)
{  
  BDSAcceleratorComponent* acceleratorComponent = beamlineElement->GetAcceleratorComponent();

  G4RotationMatrix* rm = beamlineElement->GetRotationMiddle();
  G4double phi         = rm->getPhi();
  G4double theta       = rm->getTheta();
  G4double psi         = rm->getPsi();
      
  G4double sStart      = beamlineElement->GetSPositionStart() /CLHEP::m;
  G4double sMiddle     = beamlineElement->GetSPositionMiddle()/CLHEP::m;
  G4double sEnd        = beamlineElement->GetSPositionEnd()   /CLHEP::m;
  G4ThreeVector pos    = beamlineElement->GetPositionMiddle();

  BDSBeamPipeInfo* beamPipeInfo = acceleratorComponent->GetBeamPipeInfo();
  
  survey << std::left << std::setprecision(6) << std::fixed
	 << setw(15) << acceleratorComponent->GetType()             << " "
	 << setw(40) << acceleratorComponent->GetName()             << " "
	 << setw(12) << sStart                                      << " "
	 << setw(12) << sMiddle                                     << " "
	 << setw(12) << sEnd                                        << " "
	 << setw(12) << acceleratorComponent->GetChordLength()/CLHEP::m   << " "
	 << setw(12) << acceleratorComponent->GetArcLength()/CLHEP::m     << " "
	 << setw(12) << pos.x()/CLHEP::m                            << " "
	 << setw(12) << pos.y()/CLHEP::m                            << " "
	 << setw(12) << pos.z()/CLHEP::m                            << " "
	 << setw(12) << phi/CLHEP::radian                           << " "
	 << setw(12) << theta/CLHEP::radian                         << " "
	 << setw(12) << psi/CLHEP::radian                           << " "
	 << setw(12) << (beamPipeInfo ? beamPipeInfo->aper1/CLHEP::m : 0) << " "
	 << setw(12) << (beamPipeInfo ? beamPipeInfo->aper2/CLHEP::m : 0) << " "
    	 << setw(12) << (beamPipeInfo ? beamPipeInfo->aper3/CLHEP::m : 0) << " "
    	 << setw(12) << (beamPipeInfo ? beamPipeInfo->aper4/CLHEP::m : 0) << " "
	 << setw(15) << (beamPipeInfo ? beamPipeInfo->beamPipeType : 0)   << " "
	 << setw(12) << acceleratorComponent->GetAngle()                  << " "
	 << setw(12) << element.k1 << " "
	 << setw(12) << element.k2 << " "
	 << setw(12) << element.k3 << " "
	 << setw(12) << element.k4
	 << G4endl;
}

void BDSSurvey::WriteSummary(BDSBeamline* beamline)
{  
  survey << "### Total length = " << beamline->GetTotalChordLength()/CLHEP::m << "m" << G4endl;
  survey << "### Total arc length = " <<  beamline->GetTotalArcLength()/CLHEP::m << "m" << G4endl;
  survey.close();
}
