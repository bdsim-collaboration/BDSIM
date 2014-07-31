// Geometry Interface Class
// ========================
//
// A class of functions to output Geant4/Mokka/BDSIM parameters for elements
// - to include geometry, optics, fields, etc.

#include "BDSGlobalConstants.hh"
#include "BDSGeometryInterface.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSBeamline.hh"

#include <fstream>

using std::setw;

BDSGeometryInterface::BDSGeometryInterface(G4String filename):
  itsFileName(filename)
{}

BDSGeometryInterface::~BDSGeometryInterface()
{}

void BDSGeometryInterface::Optics()
{
  std::ofstream optics;
  
  G4cout << "Generating Optics file to: " << itsFileName << " ..." << G4endl;

  optics.open(itsFileName);

  optics << std::left
	 << setw(15) << "Type"
         << setw(40) << "Name" 
	 << setw(15) << "Length [m]" 
	 << setw(15) << "S [m]" 
	 << setw(15) << "Angle [rad]" 
	 << setw(15) << "K1 [m^-2]" 
	 << setw(15) << "K2 [m^-3]" 
	 << setw(15) << "K3 [m^-4]"
	 << setw(15) << "TILT"
	 << setw(15) << "AperX [m]"
	 << setw(15) << "AperY [m]"
	 << setw(15) << "Aper_Type"
	 << G4endl;

  for(BDSBeamline::Instance()->first();!BDSBeamline::Instance()->isDone();BDSBeamline::Instance()->next())
    { 
      G4int aper_type; //1 = rect, 2 = circ, 3 = ellipse
      if(BDSBeamline::Instance()->currentItem()->GetType() == "rcol" ) //RCOL
	aper_type=1;
      else if(BDSBeamline::Instance()->currentItem()->GetType() == "ecol") //ECOL
	{
	  if(BDSBeamline::Instance()->currentItem()->GetAperX()==BDSBeamline::Instance()->currentItem()->GetAperY()) 
	    aper_type=2;
	  else aper_type=3;
	}
      else aper_type=2; // circular is default
      
      optics.setf(std::ios::fixed, std::ios::floatfield);
      optics.setf(std::ios::showpoint);
      
      optics.precision(8);
      
      optics << std::left 
	     << setw(15) << BDSBeamline::Instance()->currentItem()->GetType() << " "
	     << setw(40) << BDSBeamline::Instance()->currentItem()->GetName() << " "
	     << setw(15) << BDSBeamline::Instance()->currentItem()->GetLength()/CLHEP::m  << " "
	     << setw(15) << BDSBeamline::Instance()->positionS()/CLHEP::m  << " "
	     << setw(15) << BDSBeamline::Instance()->currentItem()->GetAngle()   << " "
	     << setw(15) << BDSBeamline::Instance()->currentItem()->GetK1()   << " "
	     << setw(15) << BDSBeamline::Instance()->currentItem()->GetK2()   << " "
	     << setw(15) << BDSBeamline::Instance()->currentItem()->GetK3()   << " "
	     << setw(15) << BDSBeamline::Instance()->currentItem()->GetTilt() << " "
	     << setw(15) << BDSBeamline::Instance()->currentItem()->GetAperX()/CLHEP::m   << " "
	     << setw(15) << BDSBeamline::Instance()->currentItem()->GetAperY()/CLHEP::m   << " "
	     << setw(15) << aper_type   << " "
	     << G4endl;
	}
      optics.close();

}


void BDSGeometryInterface::Survey()
{
  std::ofstream survey;

  G4cout << "Generating Survey: " << itsFileName << " ..." << G4endl;

  survey.open(itsFileName);
  survey << std::left 
	 << setw(15) << "Type        " << " "
	 << setw(40) << "Name        " << " "
	 << setw(15) << "Length [m]  " << " "
	 << setw(15) << "Arc_len [m] " << " "
	 << setw(15) << "X [m]       " << " "
	 << setw(15) << "Y [m]       " << " "
	 << setw(15) << "Z [m]       " << " "
	 << setw(15) << "Phi [rad]   " << " "
	 << setw(15) << "Theta [rad] "
	 << setw(15) << "Psi [rad]   " << " "
	 << setw(15) << "AperX [m]   " << " "
	 << setw(15) << "AperY [m]   " << " "
	 << setw(15) << "Aper_Type   " << " " 
	 << setw(15) << "Angle [rad] " << " "
	 << setw(15) << "K1 [m^-2]   " << " " 
	 << setw(15) << "K2 [m^-3]   " << " " 
	 << setw(15) << "K3 [m^-4]   " << " "

	 << G4endl;
  
  G4double length(0.0);
  G4double arc_length(0.0);
  for(BDSBeamline::Instance()->first();!BDSBeamline::Instance()->isDone();BDSBeamline::Instance()->next())
    { 
      G4int aper_type; //1 = rect, 2 = circ, 3 = ellipse
      if(BDSBeamline::Instance()->currentItem()->GetType() == "rcol" ) //RCOL
	aper_type=1;
      else if(BDSBeamline::Instance()->currentItem()->GetType() == "ecol" ) //ECOL
	if(BDSBeamline::Instance()->currentItem()->GetAperX()==BDSBeamline::Instance()->currentItem()->GetAperY()) 
	  aper_type=2;
	else aper_type=3;
      else aper_type=1;
      
      G4double phi=0.0, theta=0.0, psi=0.0;
      if(BDSBeamline::Instance()->currentItem()->GetRotation())
	{
	  phi = BDSBeamline::Instance()->currentItem()->GetRotation()->getPhi();
	  theta = BDSBeamline::Instance()->currentItem()->GetRotation()->getTheta();
	  psi = BDSBeamline::Instance()->currentItem()->GetRotation()->getPsi();
	}
      
      survey.setf(std::ios::fixed, std::ios::floatfield);
      survey.setf(std::ios::showpoint);
      
      survey.precision(7);
      
      survey << std::left
	     << setw(15) << BDSBeamline::Instance()->currentItem()->GetType() << " "
	     << setw(40) << BDSBeamline::Instance()->currentItem()->GetName() << " "
	     << setw(15) << BDSBeamline::Instance()->currentItem()->GetLength()/CLHEP::m  << " "
	     << setw(15) << BDSBeamline::Instance()->currentItem()->GetArcLength()/CLHEP::m  << " "
	     << setw(15) << BDSBeamline::Instance()->currentItem()->GetPosition().x()/CLHEP::m  << " "
	     << setw(15) << BDSBeamline::Instance()->currentItem()->GetPosition().y()/CLHEP::m  << " "
	     << setw(15) << BDSBeamline::Instance()->currentItem()->GetPosition().z()/CLHEP::m  << " "
	     << setw(15) << phi/CLHEP::radian  << " "
	     << setw(15) << theta/CLHEP::radian  << " "
	     << setw(15) << psi/CLHEP::radian  << " "
	     << setw(15) << BDSBeamline::Instance()->currentItem()->GetAperX()/CLHEP::m   << " "
	     << setw(15) << BDSBeamline::Instance()->currentItem()->GetAperY()/CLHEP::m   << " "
	     << setw(15) << aper_type  << " "
	     << setw(15) << BDSBeamline::Instance()->currentItem()->GetAngle()   << " "
	     << setw(15) << BDSBeamline::Instance()->currentItem()->GetK1()   << " "
	     << setw(15) << BDSBeamline::Instance()->currentItem()->GetK2()   << " "
	     << setw(15) << BDSBeamline::Instance()->currentItem()->GetK3()   << " "
	     << G4endl;
      length+=BDSBeamline::Instance()->currentItem()->GetLength()/CLHEP::m;
      arc_length+=BDSBeamline::Instance()->currentItem()->GetArcLength()/CLHEP::m;
    }
  survey << "Total length = " << length << "m" << G4endl;
  survey << "Total arc length = " <<  arc_length << "m" << G4endl;
  survey.close();
}
