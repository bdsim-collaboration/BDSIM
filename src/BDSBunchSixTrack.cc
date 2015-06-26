#include "BDSBunchSixTrack.hh"
#include "BDSDebug.hh"
#include <iostream>

#include <fstream>
#if __cplusplus>199711 // test for c++11 features
#include <regex>
#endif 

BDSBunchSixTrack::BDSBunchSixTrack() { 
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  this->iPart  = 0;


}

BDSBunchSixTrack::~BDSBunchSixTrack() { 
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  for(std::vector<double*>::iterator i = this->sixtrackData.begin();i!=this->sixtrackData.end();++i) {
    delete *i;
  }
}

void BDSBunchSixTrack::LoadSixTrackFile() { 
  // SixTrack file is always LPI file
  // header LPI : 1=sixtrackParticleID, 2=turn, 3=s [m], 4=x[mm], 5=xp[mrad], 6=y[mm], 7=yp[mrad], 8=dE/E, 9=type, 10=turns in machine after first hits on collimators

#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  // open file and read line by line and extract values
  std::ifstream infile(this->fileName);

  if (!infile)
    { G4cout << __METHOD_NAME__ << "\"" << this->fileName << "\" file doesn't exist - exiting as no input" << G4endl;
      exit(1);
    }
  else
    G4cout << __METHOD_NAME__ << "Reading as SixTrack input " << this->fileName << "\"." << G4endl;
  
  // variable for storage
  double sixtrackParticleID=0.;
  double turn = 0.;
  double s = 0.0;
  double x = 0.0; 
  double xp= 0.0;
  double y = 0.0;
  double yp= 0.0;
  double en= 0.0;
  double type = 0.;
  double turns = 0.;

  double *values =  new double[10];

  // read single line 
  while(infile >> sixtrackParticleID >> turn >> s >> x >> xp >> y >> yp >> en >> type >> turns){
    
    // append values to storage vector
    values[0] = sixtrackParticleID;
    values[1] = turn;
    values[2] = s;
    values[3] = x;
    values[4] = xp;
    values[5] = y;
    values[6] = yp;
    values[7] = en;
    values[8] = type;
    values[9] = turns;
  }

  this->sixtrackData.push_back(values);
  return;
}

void BDSBunchSixTrack::SetOptions(struct Options& opt) {
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << " " << opt.distribFile << G4endl;
#endif

  BDSBunchInterface::SetOptions(opt);
  this->SetDistribFile((G4String)opt.distribFile); 
  this->LoadSixTrackFile();
}

void BDSBunchSixTrack::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
				  G4double& xp, G4double& yp, G4double& zp,
				  G4double& t , G4double&  E, G4double& weight) {
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif


  x0     = this->sixtrackData[this->iPart][0]*CLHEP::m+X0;
  y0     = this->sixtrackData[this->iPart][2]*CLHEP::m+Y0;
  z0     = this->sixtrackData[this->iPart][4]+Z0;
  xp     = this->sixtrackData[this->iPart][1]*CLHEP::rad+Xp0;
  yp     = this->sixtrackData[this->iPart][3]*CLHEP::rad+Yp0;
  t      = (z0-Z0)/CLHEP::c_light+T0;
  E      = BDSGlobalConstants::Instance()->GetParticleKineticEnergy() * (this->sixtrackData[this->iPart][5]+1.0);
  zp     = CalculateZp(xp,yp,Zp0);
  weight = 1.0; 

  this->iPart++;


  return;
}

