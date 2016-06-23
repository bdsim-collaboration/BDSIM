#include "BDSSurvey.hh"

#include "BDSAcceleratorComponent.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSDebug.hh"
#include "BDSMagnet.hh"
#include "BDSMagnetStrength.hh"

#include <fstream>
#include <iomanip>
#include <ctime>
#include <string>

using std::setw;

BDSSurvey::BDSSurvey(G4String filename):
  nullStrength(new BDSMagnetStrength())
{
  magnetKeys = nullStrength->AllKeys();
  
  G4cout << __METHOD_NAME__ << "Generating Survey: " << filename << " ..." << G4endl;
  survey.open(filename);
  WriteHeader();
}

BDSSurvey::~BDSSurvey()
{
  if (survey.is_open())
    {survey.close();}
  delete nullStrength;
}

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
	 << setw(12) << "Angle[rad]  ";

    for (auto const key : magnetKeys)
      {survey << " " << setw(12) << key;}

    survey << G4endl;

  survey.setf(std::ios::fixed, std::ios::floatfield);
  survey.setf(std::ios::showpoint);
  // print up to 7 significant numbers
  survey.precision(7);      
}

void BDSSurvey::Write(BDSBeamline* beamline)
{
  for (auto element : *beamline)
    {Write(element);}

  survey << "### Total length = " << beamline->GetTotalChordLength()/CLHEP::m << "m" << G4endl;
  survey << "### Total arc length = " <<  beamline->GetTotalArcLength()/CLHEP::m << "m" << G4endl;
  survey.close();
}

void BDSSurvey::Write(BDSBeamlineElement* beamlineElement)
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
	 << setw(12) << acceleratorComponent->GetAngle();

  BDSMagnetStrength const* st;
  if (BDSMagnet* magCast = dynamic_cast<BDSMagnet*>(acceleratorComponent))
    {st = magCast->MagnetStrength();}
  else
    {st = nullStrength;}
  
  for (auto const key : magnetKeys)
    {survey << " " << setw(12) << (*st)[key];}

  survey << G4endl;
}

void BDSSurvey::WriteSummary(BDSBeamline* beamline)
{  
  survey << "### Total length = " << beamline->GetTotalChordLength()/CLHEP::m << "m" << G4endl;
  survey << "### Total arc length = " <<  beamline->GetTotalArcLength()/CLHEP::m << "m" << G4endl;
  survey.close();
}
