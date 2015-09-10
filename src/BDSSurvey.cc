#include "BDSSurvey.hh"

#include "BDSAcceleratorComponent.hh"
#include "BDSAcceleratorModel.hh"
#include "BDSBeamline.hh"
#include "BDSDebug.hh"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <ctime>
#include <string>

using std::setw;

BDSSurvey::BDSSurvey(G4String filename):
  itsFileName(filename)
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << "Writing survey file"<<G4endl;
#endif
  Survey();
}

BDSSurvey::~BDSSurvey()
{}

void BDSSurvey::Survey()
{
  std::ofstream survey;

  G4cout << "Generating Survey: " << itsFileName << " ..." << G4endl;

  survey.open(itsFileName);

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
/*	 << setw(12) << "AperX[m]    " << " "
	 << setw(12) << "AperY[m]    " << " "
	 << setw(8)  << "Aper_Type   " << " "
	 << setw(12) << "Angle[rad]  " << " "
	 << setw(12) << "K1[m^-2]    " << " "
	 << setw(12) << "K2[m^-3]    " << " "
	 << setw(12) << "K3[m^-4]    " << " "*/
	 << G4endl;

  BDSBeamline* beamline  = BDSAcceleratorModel::Instance()->GetFlatBeamline();
  BDSBeamline::iterator it = beamline->begin();
  for(; it != beamline->end(); ++it)
    {
      BDSAcceleratorComponent* thecurrentitem = (*it)->GetAcceleratorComponent();
      /*
      G4int aper_type; //1 = rect, 2 = circ, 3 = ellipse
      if(thecurrentitem->GetType() == "rcol" ) //RCOL
	aper_type=1;

      else if(thecurrentitem->GetType() == "ecol" ) //ECOL
	if(thecurrentitem->GetAperX()==thecurrentitem->GetAperY()) 
	  aper_type=2;
	else aper_type=3;
      else aper_type=1;
      */
      G4RotationMatrix* rm = (*it)->GetRotationMiddle();
      G4double phi   = rm->getPhi();
      G4double theta = rm->getTheta();
      G4double psi   = rm->getPsi();
      
      survey.setf(std::ios::fixed, std::ios::floatfield);
      survey.setf(std::ios::showpoint);
      
      survey.precision(7);
      
      G4double sStart    = (*it)->GetSPositionStart() /CLHEP::m;
      G4double sMiddle   = (*it)->GetSPositionMiddle()/CLHEP::m;
      G4double sEnd      = (*it)->GetSPositionEnd()   /CLHEP::m;
      G4ThreeVector pos  = (*it)->GetPositionMiddle();
      
      survey << std::left << std::setprecision(6) << std::fixed
	     << setw(15) << thecurrentitem->GetType()                   << " "
	     << setw(40) << thecurrentitem->GetName()                   << " "
	     << setw(12) << sStart                                      << " "
	     << setw(12) << sMiddle                                     << " "
	     << setw(12) << sEnd                                        << " "
	     << setw(12) << thecurrentitem->GetChordLength()/CLHEP::m   << " "
	     << setw(12) << thecurrentitem->GetArcLength()/CLHEP::m     << " "
	     << setw(12) << pos.x()/CLHEP::m                            << " "
	     << setw(12) << pos.y()/CLHEP::m                            << " "
	     << setw(12) << pos.z()/CLHEP::m                            << " "
	     << setw(12) << phi/CLHEP::radian                           << " "
	     << setw(12) << theta/CLHEP::radian                         << " "
	     << setw(12) << psi/CLHEP::radian                           << " " << G4endl;
	/*
	     << setw(12) << thecurrentitem->GetAperX()/CLHEP::m         << " "
	     << setw(12) << thecurrentitem->GetAperY()/CLHEP::m         << " "
	     << setw(8)  << aper_type                                   << " "
	     << setw(12) << thecurrentitem->GetAngle()                  << " "
	     << setw(12) << thecurrentitem->GetK1()                     << " "
	     << setw(12) << thecurrentitem->GetK2()                     << " "
	     << setw(12) << thecurrentitem->GetK3()                     << " "
	     << G4endl;*/
    }
  survey << "### Total length = " << beamline->GetTotalChordLength()/CLHEP::m << "m" << G4endl;
  survey << "### Total arc length = " <<  beamline->GetTotalArcLength()/CLHEP::m << "m" << G4endl;
  survey.close();
}
