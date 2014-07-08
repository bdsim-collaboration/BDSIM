#include "BDSBunch.hh"

#include <iostream>
#include <cmath>
#include <cstdlib>

#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh"
//#include "parser/gmad.h"
#include "parser/options.h"

#include "G4ParticleTable.hh"

// CLHEP from Geant4
#include "Randomize.hh"
// CLHEP
#include "CLHEP/RandomObjects/RandMultiGauss.h"

// distribution type
namespace {
  enum {
    _REFERENCE = 0,
    _GAUSSIAN = 1,
    _RING = 2,
    _SQUARE = 3,
    _CIRCLE = 4,
    _GUINEAPIG_BUNCH = 5,
    _GUINEAPIG_PAIRS = 6,
    _GUINEAPIG_SLAC = 7,
    _CAIN = 8,
    _ESHELL = 9,
    _GAUSSIAN_TWISS = 10,
    _GAUSSIAN_MATRIX = 11,
    _UDEF = 32
  };
}

BDSBunch::BDSBunch():  
  distribType(-1),X0(0.0),Y0(0.0),Z0(0.0),T0(0.0),Xp0(0.0),Yp0(0.0),Zp0(1.0),
  envelopeX(0.0),envelopeY(0.0),envelopeT(0.0),
  envelopeXp(0.0),envelopeYp(0.0),envelopeE(0.0),
  sigmaX(0.0),sigmaY(0.0),sigmaT(0.0),sigmaXp(0.0),sigmaYp(0.0),
  rMin(0.0),rMax(0.0),shellx(0.0),shelly(0.0),shellxp(0.0),shellyp(0.0),
  betaX(0.0),betaY(0.0),alphaX(0.0),alphaY(0.0),emitX(0.0),emitY(0.0),
  energySpread(0.0),nlinesIgnore(0),partId(0)
{ 
  verbose            = BDSExecOptions::Instance()->GetVerbose();
  verboseStep        = BDSExecOptions::Instance()->GetVerboseStep();
  verboseEvent       = BDSExecOptions::Instance()->GetVerboseEvent();
  verboseEventNumber = BDSExecOptions::Instance()->GetVerboseEventNumber();
  nptwiss            = BDSExecOptions::Instance()->GetNPTwiss();

  // Instantiate random number generators
  GaussGen = new CLHEP::RandGauss(*CLHEP::HepRandom::getTheEngine());
  FlatGen  = new CLHEP::RandFlat(*CLHEP::HepRandom::getTheEngine());
  GaussMultiGen = NULL;

  // Instantiate vector and matrix for gaussian sigma matrix generation
  meansGM = CLHEP::HepVector(6);
  sigmaGM = CLHEP::HepSymMatrix(6);

}

BDSBunch::~BDSBunch()
{
  // Delete random number generators
  delete GaussGen;
  delete FlatGen;
  delete GaussMultiGen;
}

// set options from gmad

void BDSBunch::skip(G4int nvalues){
  G4double dummy_val;
  for(G4int i=0;i<nvalues;i++) ReadValue(dummy_val);
}

void BDSBunch::SetOptions(struct Options& opt)
{
  std::map<const G4String, int, strCmp> distType;
  distType["reference"]=_REFERENCE;             // Reference orbit
  distType["gauss"]=_GAUSSIAN;                  // Gaussian with only diagonal sigma matrix
  distType["ring"]=_RING;                       // Ring in cannonical phase space
  distType["square"]=_SQUARE;                   // Square phase space (flat)
  distType["circle"]=_CIRCLE;                   // Circular phase space (flat) 
  distType["guineapig_bunch"]=_GUINEAPIG_BUNCH; // (LC) Bunch bunch collision 
  distType["guineapig_pairs"]=_GUINEAPIG_PAIRS; // (LC) Electron and positron pair from bunch bunch collision
  distType["guineapig_slac"]=_GUINEAPIG_SLAC;   // (LC) SLAC variant of same code
  distType["cain"]=_CAIN;                       // (LC) Bunch bunch collision
  distType["eshell"]=_ESHELL;                   // ?? 
  distType["gausstwiss"]=_GAUSSIAN_TWISS;       // Normal Gaussian Twiss 
  distType["gaussmatrix"]=_GAUSSIAN_MATRIX;     // Normal Gaussian sigma matrix

  nlinesIgnore = opt.nlinesIgnore;
  inputfile=opt.distribFile;
  //#define _skip(nvalues) for(G4int i=0;i<nvalues;i++) ReadValue(dummy_val);
  
  // twiss parameters - set always if present
  SetBetaX(opt.betx);
  SetBetaY(opt.bety);
  SetAlphaX(opt.alfx);
  SetAlphaY(opt.alfy);
  SetEmitX(opt.emitx);
  SetEmitY(opt.emity);
  
  std::map<const G4String,int>::iterator iter;
  iter = distType.find(opt.distribType);
  if(iter!=distType.end()) 
    distribType = (*iter).second;
  else {
    distribType = _UDEF;
    G4cerr << "WARNING bunch distribution type UNKNOWN or USER DEFINED: " << opt.distribType << G4endl;
  }
#ifdef DEBUG 
  G4cout<< "BDSBunch::SetOptions> distrType : "<<opt.distribType<<G4endl;
#endif
  //
  // global parameters
  //
  X0 = opt.X0;
  Y0 = opt.Y0;
  Z0 = opt.Z0;
  T0 = opt.T0;
  Xp0 = opt.Xp0;
  Yp0 = opt.Yp0;
  if (opt.Zp0 < 0)
    Zp0 = -sqrt(1.-Xp0*Xp0-Yp0*Yp0);
  else
    Zp0 = sqrt(1.-Xp0*Xp0-Yp0*Yp0);
  
  
  //
  // specific parameters which depend on distribution type
  //
  switch(distribType){
  
  case _REFERENCE :
    {
      SetSigmaX(0.0); 
      SetSigmaY(0.0);
      SetSigmaXp(0.0);
      SetSigmaYp(0.0);
      SetSigmaT(0.0);
      SetEnergySpread(0.0);          
      break;
    }

  case _GAUSSIAN:
    {
      SetSigmaX(opt.sigmaX); 
      SetSigmaY(opt.sigmaY);
      SetSigmaXp(opt.sigmaXp);
      SetSigmaYp(opt.sigmaYp);
      SetSigmaT(opt.sigmaT);
      SetEnergySpread(opt.sigmaE);
      break;
    } 

  case _SQUARE:
    {
      SetEnvelopeX(opt.envelopeX); 
      SetEnvelopeY(opt.envelopeY);
      SetEnvelopeXp(opt.envelopeXp);
      SetEnvelopeYp(opt.envelopeYp);
      SetEnvelopeT(opt.envelopeT);
      SetEnergySpread(opt.envelopeE);
      break;
    } 

  case _GAUSSIAN_TWISS:
    {
      SetSigmaT(opt.sigmaT);
      SetEnergySpread(opt.sigmaE);
      break;
    } 

  case _GAUSSIAN_MATRIX:
    {       
#ifdef DEBUG
      G4cout<< "BDSBunch::SetOptions> case _GAUSSIAN_MATRIX " << G4endl;      
      G4cout<< "BDSBunch::SetOptions> " << X0 << " " << Xp0 << " " << Y0 << " " << Yp0 << " " << T0 << G4endl;
#endif      


      // set means
      meansGM[0] = X0;
      meansGM[1] = Xp0;
      meansGM[2] = Y0;
      meansGM[3] = Yp0;
      meansGM[4] = T0;
      meansGM[5] = 1;
	
      // set sigma matrix for generation
      sigmaGM[0][0] = opt.sigma11;
      sigmaGM[0][1] = opt.sigma12;
      sigmaGM[0][2] = opt.sigma13;
      sigmaGM[0][3] = opt.sigma14;
      sigmaGM[0][4] = opt.sigma15;
      sigmaGM[0][5] = opt.sigma16;

      sigmaGM[1][1] = opt.sigma22;
      sigmaGM[1][2] = opt.sigma23;
      sigmaGM[1][3] = opt.sigma24;
      sigmaGM[1][4] = opt.sigma25;
      sigmaGM[1][5] = opt.sigma26;

      sigmaGM[2][2] = opt.sigma33;
      sigmaGM[2][3] = opt.sigma34;
      sigmaGM[2][4] = opt.sigma35;
      sigmaGM[2][5] = opt.sigma36;

      sigmaGM[3][3] = opt.sigma44;
      sigmaGM[3][4] = opt.sigma45;
      sigmaGM[3][5] = opt.sigma46;

      sigmaGM[4][4] = opt.sigma55;
      sigmaGM[4][5] = opt.sigma56;

      sigmaGM[5][5] = opt.sigma66;

      // Set gauss sigmas for consistency 
      

      // Set sigma T 
      SetSigmaT(sqrt(opt.sigma55));
       
      // Set energy spread
      SetEnergySpread(sqrt(opt.sigma66));
      
      // make gaussian generator 
      GaussMultiGen = new CLHEP::RandMultiGauss(*CLHEP::HepRandom::getTheEngine(),
						meansGM,sigmaGM);   

#ifdef DEBUG
      G4cout<< "BDSBunch::SetOptions> case _GAUSSIAN_MATRIX break " << G4endl;      
#endif      					       

      break;
    }
    
  case _RING:
    {
      rMin = opt.Rmin;
      rMax = opt.Rmax;
      SetEnergySpread(opt.sigmaE);
      break;
    } 
    
  case _ESHELL:
    {
      shellx = opt.shellX;
      shelly = opt.shellY;
      shellxp = opt.shellXp;
      shellyp = opt.shellYp;
      SetEnergySpread(opt.sigmaE);
      break;
    }
    
  case _GUINEAPIG_BUNCH:
    {
      OpenBunchFile();
#ifdef DEBUG 
      G4cout<< "BDSBunch : " <<"GUINEAPIG_BUNCH: skipping "<<nlinesIgnore<<"  lines"<<G4endl;
#endif
      skip(nlinesIgnore * 6);
      break;
    } 
    
  case _GUINEAPIG_SLAC:
    {
      OpenBunchFile();
#ifdef DEBUG 
      G4cout<< "BDSBunch : " <<"GUINEAPIG_SLAC: skipping "<<nlinesIgnore<<"  lines"<<G4endl;
#endif
      skip(nlinesIgnore * 6);
      break;
    } 

  case _GUINEAPIG_PAIRS:
    {
      OpenBunchFile();
#ifdef DEBUG 
      G4cout<< "BDSBunch : " <<"GUINEAPIG_PAIRS: skipping "<<nlinesIgnore<<"  lines"<<G4endl;
#endif
      skip(nlinesIgnore * 7);
      break;
    }
    
  case _CAIN:
    {
      OpenBunchFile();
#ifdef DEBUG 
      G4cout<< "BDSBunch : " <<"CAIN: skipping "<<nlinesIgnore<<"  lines"<<G4endl;
#endif
      skip(nlinesIgnore * 14);
      break;
    } 
    //else
    //assuming the format is "field[unit]:field[unit]:..." - User Defined
  case _UDEF:
  default:
    {
      distribType = _UDEF; 
      
      // construct the list of read attributes
      
      G4String unparsed_str = opt.distribType; 
      G4int pos = unparsed_str.find(":");
      
      struct BDSBunch::Doublet sd;
      
      while(pos > 0)
	{
	  pos = unparsed_str.find(":");
	  G4String token = unparsed_str.substr(0,pos);


	  unparsed_str = unparsed_str.substr(pos+1);
#ifdef DEBUG 
          G4cout<< "BDSBunch : " <<"token ->"<<token<<G4endl;
	  G4cout<< "BDSBunch : token.substr(0,1) -> " << token.substr(0,1) << G4endl;
	  G4cout<< "BDSBunch : " <<"unparsed_str ->"<<unparsed_str<<G4endl;
          G4cout<< "BDSBunch : " <<"pos ->"<<pos<<G4endl;
#endif
	  if(token.substr(0,1)=="E") {
#ifdef DEBUG 
	    G4cout<< "BDSBunch : " <<"E!"<<G4endl;
#endif
	    G4String rest = token.substr(1);
#ifdef DEBUG 
	    G4cout<< "BDSBunch : " <<"rest ->"<<rest<<G4endl;
#endif
	    G4int pos1 = rest.find("[");
	    G4int pos2 = rest.find("]");
	    if(pos1 < 0 || pos2 < 0) {
	      G4cerr<<"unit format wrong!!!"<<G4endl;
	    } else {
	      G4String fmt = rest.substr(pos1+1,pos2-1);
#ifdef DEBUG 
	      G4cout<< "BDSBunch : " <<"fmt ->"<<fmt<<G4endl;
#endif
	      sd.name = "E"; 
              
	      if(fmt=="GeV") sd.unit=1;
	      if(fmt=="MeV") sd.unit=1.e-3;
	      if(fmt=="KeV") sd.unit=1.e-6;
	      if(fmt=="eV") sd.unit=1.e-9;
	      
	      fields.push_back(sd);
	    }
	  } else if(token.substr(0,1)=="P") {
#ifdef DEBUG 
	    G4cout<< "BDSBunch : " <<"P!"<<G4endl;
#endif
	    G4String rest = token.substr(1);
#ifdef DEBUG 
	    G4cout<< "BDSBunch : " <<"rest ->"<<rest<<G4endl;
#endif
	    G4int pos1 = rest.find("[");
	    G4int pos2 = rest.find("]");
	    if(pos1 < 0 || pos2 < 0) {
	      G4cerr<<"unit format wrong!!!"<<G4endl;
	    } else {
	      G4String fmt = rest.substr(pos1+1,pos2-1);
#ifdef DEBUG 
	      G4cout<< "BDSBunch : " <<"fmt ->"<<fmt<<G4endl;
#endif
	      sd.name = "P"; 
              
	      if(fmt=="GeV") sd.unit=1;
	      if(fmt=="MeV") sd.unit=1.e-3;
	      if(fmt=="KeV") sd.unit=1.e-6;
	      if(fmt=="eV") sd.unit=1.e-9;
	      
	      fields.push_back(sd);
	    }
	  } else if(token.substr(0,1)=="t") {
#ifdef DEBUG 
	    G4cout<< "BDSBunch : " <<"t!"<<G4endl;
#endif
	    G4String rest = token.substr(1);
#ifdef DEBUG 
	    G4cout<< "BDSBunch : " <<"rest ->"<<rest<<G4endl;
#endif
	    G4int pos1 = rest.find("[");
	    G4int pos2 = rest.find("]");
	    if(pos1 < 0 || pos2 < 0) {
	      G4cerr<<"unit format wrong!!!"<<G4endl;
	    } else {
	      G4String fmt = rest.substr(pos1+1,pos2-1);
#ifdef DEBUG 
	      G4cout<< "BDSBunch : " <<"fmt ->"<<fmt<<G4endl;
#endif
	      sd.name = "t"; 
              
	      if(fmt=="s") sd.unit=1;
	      if(fmt=="ms") sd.unit=1.e-3;
	      if(fmt=="mus") sd.unit=1.e-6;
	      if(fmt=="ns") sd.unit=1.e-9;
	      if(fmt=="mm/c") sd.unit=(CLHEP::mm/CLHEP::c_light)/CLHEP::s;
	      if(fmt=="nm/c") sd.unit=(CLHEP::nm/CLHEP::c_light)/CLHEP::s;
              
	      fields.push_back(sd);
	      
	    }
	  } else if( (token.substr(0,1)=="x") && (token.substr(1,1)!="p") ) {
#ifdef DEBUG 
	    G4cout<< "BDSBunch : " <<"x!"<<G4endl;
#endif
	    G4String rest = token.substr(1);
#ifdef DEBUG 
	    G4cout<< "BDSBunch : " <<"rest ->"<<rest<<G4endl;
#endif
	    G4int pos1 = rest.find("[");
	    G4int pos2 = rest.find("]");
	    if(pos1 < 0 || pos2 < 0) {
	      G4cerr<<"unit format wrong!!!"<<G4endl;
	    } else {
	      G4String fmt = rest.substr(pos1+1,pos2-1);
#ifdef DEBUG 
	      G4cout<< "BDSBunch : " <<"fmt ->"<<fmt<<G4endl;
#endif
	      sd.name="x";
	      
	      if(fmt=="m") sd.unit=1;
	      if(fmt=="cm") sd.unit=1.e-2;
	      if(fmt=="mm") sd.unit=1.e-3;
	      if(fmt=="mum") sd.unit=1.e-6;
	      if(fmt=="nm") sd.unit=1.e-9;
	      
	      fields.push_back(sd);
	      
	    }
	  }else if(token.substr(0,1)=="y" && token.substr(1,1)!="p" ) {
#ifdef DEBUG 
	    G4cout<< "BDSBunch : " <<"y!"<<G4endl;
#endif
	    G4String rest = token.substr(1);
#ifdef DEBUG 
	    G4cout<< "BDSBunch : " <<"rest ->"<<rest<<G4endl;
#endif
	    G4int pos1 = rest.find("[");
	    G4int pos2 = rest.find("]");
	    if(pos1 < 0 || pos2 < 0) {
	      G4cerr<<"unit format wrong!!!"<<G4endl;
	    } else {
	      G4String fmt = rest.substr(pos1+1,pos2-1);
#ifdef DEBUG 
	      G4cout<< "BDSBunch : " <<"fmt ->"<<fmt<<G4endl;
#endif
	      sd.name="y";
	      
	      if(fmt=="m") sd.unit=1;
	      if(fmt=="cm") sd.unit=1.e-2;
	      if(fmt=="mm") sd.unit=1.e-3;
	      if(fmt=="mum") sd.unit=1.e-6;
	      if(fmt=="nm") sd.unit=1.e-9;
	      
	      fields.push_back(sd);
	    }
	  }else if(token.substr(0,1)=="z" && token.substr(1,1)!="p" ) {
#ifdef DEBUG 
              G4cout<< "BDSBunch : " <<"z!"<<G4endl;
#endif
	      G4String rest = token.substr(1);
#ifdef DEBUG 
              G4cout<< "BDSBunch : " <<"rest ->"<<rest<<G4endl;
#endif
	      G4int pos1 = rest.find("[");
	      G4int pos2 = rest.find("]");
	      if(pos1 < 0 || pos2 < 0) {
		G4cerr<<"unit format wrong!!!"<<G4endl;
	      } else {
		G4String fmt = rest.substr(pos1+1,pos2-1);
#ifdef DEBUG 
                G4cout<< "BDSBunch : " <<"fmt ->"<<fmt<<G4endl;
#endif
		sd.name="z";
                
		if(fmt=="m") sd.unit=1;
		if(fmt=="cm") sd.unit=1.e-2;
		if(fmt=="mm") sd.unit=1.e-3;
		if(fmt=="mum") sd.unit=1.e-6;
		if(fmt=="nm") sd.unit=1.e-9;
		
		fields.push_back(sd);
	      }
	  } else if(token.substr(0,2)=="xp") {
#ifdef DEBUG 
	    G4cout<< "BDSBunch : " <<"xp!"<<G4endl;
#endif
	    G4String rest = token.substr(2);
#ifdef DEBUG 
	    G4cout<< "BDSBunch : " <<"rest ->"<<rest<<G4endl;
#endif
	    G4int pos1 = rest.find("[");
	    G4int pos2 = rest.find("]");
	    if(pos1 < 0 || pos2 < 0) {
	      G4cerr<<"unit format wrong!!!"<<G4endl;
	    } else {
	      G4String fmt = rest.substr(pos1+1,pos2-1);
#ifdef DEBUG 
	      G4cout<< "BDSBunch : " <<"fmt ->"<<fmt<<G4endl;
#endif
	      sd.name="xp";
              
	      if(fmt=="rad") sd.unit=1;
	      if(fmt=="mrad") sd.unit=1.e-3;
	      if(fmt=="murad") sd.unit=1.e-6;
	      
	      fields.push_back(sd);
	      
	    }
	  }else if(token.substr(0,2)=="yp") {
#ifdef DEBUG 
	    G4cout<< "BDSBunch : " <<"yp!"<<G4endl;
#endif
	    G4String rest = token.substr(2);
#ifdef DEBUG 
	    G4cout<< "BDSBunch : " <<"rest ->"<<rest<<G4endl;
#endif
	    G4int pos1 = rest.find("[");
	    G4int pos2 = rest.find("]");
	    if(pos1 < 0 || pos2 < 0) {
	      G4cerr<<"unit format wrong!!!"<<G4endl;
	    } else {
	      G4String fmt = rest.substr(pos1+1,pos2-1);
#ifdef DEBUG 
	      G4cout<< "BDSBunch : " <<"fmt ->"<<fmt<<G4endl;
#endif
	      sd.name="yp";
	      
	      if(fmt=="rad") sd.unit=1;
	      if(fmt=="mrad") sd.unit=1.e-3;
	      if(fmt=="murad") sd.unit=1.e-6;
	      
	      fields.push_back(sd);
	    }
	  } else if(token.substr(0,2)=="zp") {
#ifdef DEBUG 
	    G4cout<< "BDSBunch : " <<"zp!"<<G4endl;
#endif
	    G4String rest = token.substr(2);
#ifdef DEBUG 
	    G4cout<< "BDSBunch : " <<"rest ->"<<rest<<G4endl;
#endif
	    G4int pos1 = rest.find("[");
	    G4int pos2 = rest.find("]");
	    if(pos1 < 0 || pos2 < 0) {
	      G4cerr<<"unit format wrong!!!"<<G4endl;
	    } else {
	      G4String fmt = rest.substr(pos1+1,pos2-1);
#ifdef DEBUG 
	      G4cout<< "BDSBunch : " <<"fmt ->"<<fmt<<G4endl;
#endif
	      sd.name="zp";
	      
	      if(fmt=="rad") sd.unit=1;
	      if(fmt=="mrad") sd.unit=1.e-3;
	      if(fmt=="murad") sd.unit=1.e-3;
	      
	      fields.push_back(sd);
	    }
	  }else if(token.substr(0,2)=="pt") {
#ifdef DEBUG 
	    G4cout<< "BDSBunch : " <<"pt!"<<G4endl;
#endif
	    sd.name="pt";
	    sd.unit=1;
	    fields.push_back(sd);
	  } else if(token.substr(0,1)=="w") {
#ifdef DEBUG 
	    G4cout<< "BDSBunch : " <<"weight!"<<G4endl;
#endif
	    sd.name="weight";
	    sd.unit=1;
	    fields.push_back(sd);
	  } else {
	    G4cerr << "Cannot determine bunch data format" << G4endl; exit(1);
	  }
	} 
      OpenBunchFile();
    }
  }
  return;  
}

// get initial bunch distribution parameters in square/circle case 
G4double BDSBunch::GetEnvelopeT() 
{
  return envelopeT;
} 

G4double BDSBunch::GetEnvelopeX()
{

  return envelopeX;
}
 
G4double BDSBunch::GetEnvelopeY()
{
  return envelopeY;
} 

G4double BDSBunch::GetEnvelopeXp()
{
  return envelopeXp;
}

G4double BDSBunch::GetEnvelopeYp()
{
  return envelopeYp;
}

// get initial bunch distribution parameters in Gaussian case 
G4double BDSBunch::GetSigmaT() 
{
  return sigmaT;
} 

G4double BDSBunch::GetSigmaX()
{

  return sigmaX;
}
 
G4double BDSBunch::GetSigmaY()
{
  return sigmaY;
} 

G4double BDSBunch::GetSigmaXp()
{
  return sigmaXp;
}

G4double BDSBunch::GetSigmaYp()
{
  return sigmaYp;
}

// generate primary coordinates

G4double BDSBunch::GetNextX()
{
  return sigmaX * GaussGen->shoot() * CLHEP::m;
} 

G4double BDSBunch::GetNextY()
{
  return sigmaY * GaussGen->shoot() * CLHEP::m;
}

G4double BDSBunch::GetNextZ()
{
  return 0;
}



G4double BDSBunch::GetNextXp()
{
  return sigmaXp * GaussGen->shoot();
}

G4double BDSBunch::GetNextYp()
{
  return sigmaYp * GaussGen->shoot();
}

G4double BDSBunch::GetNextT()
{
  return - sigmaT* (1.-2.*GaussGen->shoot());
}


void BDSBunch::GetNextParticle(G4double& x0,G4double& y0,G4double& z0,
			       G4double& xp,G4double& yp,G4double& zp,
			       G4double& t, G4double& E, G4double &weight)
{

#ifdef DEBUG 
  G4cout<< "BDSBunch::GetNextParticle> Twiss : " << betaX  << " " << betaY  << " " 
	                                         << alphaX << " " << alphaY << " "
	                                         << emitX  << " " << emitY  << G4endl;
#endif
  if(verboseStep) G4cout<< "BDSBunch : " <<"distribution type: "<<distribType<<G4endl;

  double r, phi;
  // Rescale must be at the top of GetNextParticle

  if(BDSGlobalConstants::Instance()->isReference && partId<nptwiss){
    if (betaX==0) {G4cerr << __METHOD_NAME__ << "WARNING betaX equal to 0, xp NaN, division by zero! " << G4endl; exit(1);}
    if (betaY==0) {G4cerr << __METHOD_NAME__ << "WARNING betaY equal to 0, yp NaN, division by zero! " << G4endl; exit(1);}
    
    G4double phiX= CLHEP::twopi * G4UniformRand();
    G4double phiY= CLHEP::twopi * G4UniformRand();
    //    G4double ex=-log(G4UniformRand())*emitX;
    //    G4double ey=-log(G4UniformRand())*emitY;
    G4double ex=std::abs(GaussGen->shoot()*emitX);
    G4double ey=std::abs(GaussGen->shoot()*emitY);
    x0=sqrt(2*ex*betaX)*sin(phiX);
    xp=sqrt(2*ex/betaX)*(cos(phiX)-alphaX*sin(phiX));
    y0=sqrt(2*ey*betaY)*sin(phiY);
    yp=sqrt(2*ey/betaY)*(cos(phiY)-alphaY*sin(phiY)); 
    z0 = Z0 * CLHEP::m + (T0 - sigmaT * (1.-2.*GaussGen->shoot())) * CLHEP::c_light * CLHEP::s;
    if (Zp0<0)
      zp = -sqrt(1.-xp*xp -yp*yp);
    else
      zp = sqrt(1.-xp*xp -yp*yp);
    t = 0; 
    E = BDSGlobalConstants::Instance()->GetBeamKineticEnergy();
    ++partId;
    return;
  }
  
  if(BDSGlobalConstants::Instance()->DoTwiss() && partId<nptwiss)
    {
      if (betaX==0) {G4cerr << __METHOD_NAME__ << "WARNING betaX equal to 0, xp NaN, division by zero! " << G4endl; exit(1);}
      if (betaY==0) {G4cerr << __METHOD_NAME__ << "WARNING betaY equal to 0, yp NaN, division by zero! " << G4endl; exit(1);}

      // temp numbers - to be replaced by parsed parameters
      
      G4double sigx = sqrt(betaX*emitX);
      G4double sigxp= sqrt(emitX / betaX);
      
      G4double sigy = sqrt(betaY*emitY);
      G4double sigyp= sqrt(emitY / betaY);
      
      partId++;
      
      G4double phase_factor = 1 / ( (nptwiss/2.) - 1.0 );
      if(partId<=nptwiss/2) //primary - xx' ellipse
	{
	  x0 = sigx * cos(partId* 2 * CLHEP::pi*phase_factor);
	  xp = -sigxp * ( -1*alphaX * cos(partId * 2 * CLHEP::pi*phase_factor )
			  + sin(partId * 2 * CLHEP::pi*phase_factor ) );
	  y0 = 0;
	  yp = 0;
	}
      else if(partId<=nptwiss) //primary - yy' ellipse
	{
	  x0 = 0;
	  xp = 0;
	  y0 = sigy * cos( (partId-nptwiss/2)*2*CLHEP::pi*phase_factor);
	  yp = -sigyp * ( -1*alphaY * cos( (partId-nptwiss/2) * 2 * CLHEP::pi*phase_factor)
			  + sin( (partId-nptwiss/2) * 2 * CLHEP::pi*phase_factor) );
	}
      //tmp - check units of above equations!!
      x0*=CLHEP::m;
      y0*=CLHEP::m;
      xp*=CLHEP::radian;
      yp*=CLHEP::radian;
      
      E = BDSGlobalConstants::Instance()->GetBeamTotalEnergy() - BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass();
      zp = sqrt(1-xp*xp-yp*yp);
      t=0;
      z0=0;
      
#ifdef DEBUG
	G4cout << "x: " << x0/CLHEP::micrometer << " xp: " << xp/CLHEP::radian << G4endl;
	G4cout << "y: " << y0/CLHEP::micrometer << " yp: " << yp/CLHEP::radian << G4endl;
	G4cout << "z: " << z0/CLHEP::micrometer << " zp: " << zp/CLHEP::radian << G4endl;
#endif
        return;
    } //end doTwiss && partId<nptwiss
  
 
 
  switch(distribType){
  case _REFERENCE: 
    {
      x0 = (X0 + 0.0) * CLHEP::m;
      y0 = (Y0 + 0.0) * CLHEP::m;
      z0 = (Z0 + 0.0) * CLHEP::m;
      xp = (Xp0 + 0.0)* CLHEP::rad;
      yp = (Yp0 + 0.0)* CLHEP::rad;
      if (Zp0<0)
        zp = -sqrt(1.-xp*xp -yp*yp);
      else
        zp = sqrt(1.-xp*xp -yp*yp);      
      t  = 0.0; 
      E = BDSGlobalConstants::Instance()->GetBeamKineticEnergy();
      weight = 1.0;
      break;
    }
  case _GAUSSIAN:
    {
      x0 = X0 * CLHEP::m;
      y0 = Y0 * CLHEP::m;
      z0 = Z0 * CLHEP::m;
      xp = Xp0 * CLHEP::rad;
      yp = Yp0 * CLHEP::rad;
      z0 = Z0 * CLHEP::m + (T0 - sigmaT * (1.-2.*GaussGen->shoot())) * CLHEP::c_light * CLHEP::s;

      if(sigmaX !=0) x0  += sigmaX * GaussGen->shoot() * CLHEP::m;
      if(sigmaY !=0) y0  += sigmaY * GaussGen->shoot() * CLHEP::m;
      if(sigmaXp !=0) xp += sigmaXp * GaussGen->shoot() * CLHEP::rad;
      if(sigmaYp !=0) yp += sigmaYp * GaussGen->shoot() * CLHEP::rad;

      if (Zp0<0)
        zp = -sqrt(1.-xp*xp -yp*yp);
      else
        zp = sqrt(1.-xp*xp -yp*yp);
      t = 0;
      E = BDSGlobalConstants::Instance()->GetBeamKineticEnergy() * (1 + energySpread * GaussGen->shoot());

#ifdef DEBUG 
      G4cout<< "BDSBunch : " <<"GAUSSIAN: "<<G4endl
            <<" X0= "<<X0<<" m"<<G4endl
            <<" Y0= "<<Y0<<" m"<<G4endl
            <<" Z0= "<<Z0<<" m"<<G4endl
            <<" T0= "<<T0<<" s"<<G4endl
            <<" Xp0= "<<Xp0<<G4endl
            <<" Yp0= "<<Yp0<<G4endl
            <<" Zp0= "<<Zp0<<G4endl
            <<" sigmaX= "<<sigmaX<<" m"<<G4endl
            <<" sigmaY= "<<sigmaY<<" m"<<G4endl
            <<" sigmaXp= "<<sigmaXp<<G4endl
            <<" sigmaYp= "<<sigmaYp<<G4endl
            <<" sigmaT= "<<sigmaT<<"s"<<G4endl
            <<" relative energy spread= "<<energySpread<<G4endl
	    <<G4endl;
#endif


      break;
    }
  case _SQUARE:
    {
      x0 = X0 * CLHEP::m;
      y0 = Y0 * CLHEP::m;
      z0 = Z0 * CLHEP::m;
      xp = Xp0 * CLHEP::rad;
      yp = Yp0 * CLHEP::rad;
      z0 = Z0 * CLHEP::m + (T0 - envelopeT * (1.-2.*FlatGen->shoot())) * CLHEP::c_light * CLHEP::s;

      if(envelopeX !=0) x0  += envelopeX * (1-2*FlatGen->shoot()) * CLHEP::m;
      if(envelopeY !=0) y0  += envelopeY * (1-2*FlatGen->shoot()) * CLHEP::m;
      if(envelopeXp !=0) xp += envelopeXp * (1-2*FlatGen->shoot()) * CLHEP::rad;
      if(envelopeYp !=0) yp += envelopeYp * (1-2*FlatGen->shoot()) * CLHEP::rad;

      if (Zp0<0)
        zp = -sqrt(1.-xp*xp -yp*yp);
      else
        zp = sqrt(1.-xp*xp -yp*yp);
      t = 0;
      E = BDSGlobalConstants::Instance()->GetBeamKineticEnergy() * (1 + energySpread * (1-2*FlatGen->shoot()));

#ifdef DEBUG 
      G4cout<< "BDSBunch : " <<"SQUARE: "<<G4endl
            <<" X0= "<<X0<<" m"<<G4endl
            <<" Y0= "<<Y0<<" m"<<G4endl
            <<" Z0= "<<Z0<<" m"<<G4endl
            <<" T0= "<<T0<<" s"<<G4endl
            <<" Xp0= "<<Xp0<<G4endl
            <<" Yp0= "<<Yp0<<G4endl
            <<" Zp0= "<<Zp0<<G4endl
            <<" envelopeX= "<<envelopeX<<" m"<<G4endl
            <<" envelopeY= "<<envelopeY<<" m"<<G4endl
            <<" envelopeXp= "<<envelopeXp<<G4endl
            <<" envelopeYp= "<<envelopeYp<<G4endl
            <<" envelopeT= "<<envelopeT<<"s"<<G4endl
            <<" relative energy spread= "<<energySpread<<G4endl
	    <<G4endl;
#endif


      break;
    }
  case _GAUSSIAN_TWISS:
    {
#ifdef DEBUG 
      G4cout<<"GAUSSIAN_TWISS: "<<G4endl
            // <<" X0= "<<X0<<" m"<<G4endl
            // <<" Y0= "<<Y0<<" m"<<G4endl
            // <<" Z0= "<<Z0<<" m"<<G4endl
            <<" T0= "<<T0<<" s"<<G4endl
            // <<" Xp0= "<<Xp0<<G4endl
            // <<" Yp0= "<<Yp0<<G4endl
            // <<" Zp0= "<<Zp0<<G4endl
            <<" alphaX= "<<alphaX<<" m"<<G4endl
            <<" alphaY= "<<alphaY<<" m"<<G4endl
            <<" betaX= "<<betaX<<G4endl
            <<" betaY= "<<betaY<<G4endl
	    <<" emitX= "<<emitX<<G4endl
            <<" emitY= "<<emitY<<G4endl
            <<" sigmaT= "<<sigmaT<<"s"<<G4endl
            <<" relative energy spread= "<<energySpread<<G4endl;
#endif

      if (betaX==0) {G4cerr << __METHOD_NAME__ << "WARNING betaX equal to 0, xp NaN, division by zero! " << G4endl; exit(1);}
      if (betaY==0) {G4cerr << __METHOD_NAME__ << "WARNING betaY equal to 0, yp NaN, division by zero! " << G4endl; exit(1);}

      G4double phiX= CLHEP::twopi * G4UniformRand();
      G4double phiY= CLHEP::twopi * G4UniformRand();
      //      G4double ex=-log(G4UniformRand())*emitX;
      //      G4double ey=-log(G4UniformRand())*emitY;
      G4double ex=std::abs(GaussGen->shoot()*emitX);
      G4double ey=std::abs(GaussGen->shoot()*emitY);
      x0=sqrt(2*ex*betaX)*sin(phiX)*CLHEP::m;
      xp=sqrt(2*ex/betaX)*(cos(phiX)-alphaX*sin(phiX))*CLHEP::rad;
      y0=sqrt(2*ey*betaY)*sin(phiY)*CLHEP::m;
      yp=sqrt(2*ey/betaY)*(cos(phiY)-alphaY*sin(phiY))*CLHEP::rad;
      z0 = Z0 * CLHEP::m + (T0 - sigmaT * (1.-2.*GaussGen->shoot())) * CLHEP::c_light * CLHEP::s;

      if (Zp0<0)
        zp = -sqrt(1.-xp*xp -yp*yp);
      else
        zp = sqrt(1.-xp*xp -yp*yp);
      t = 0; // (T0 - sigmaT * (1.-2.*GaussGen->shoot())) * s;
      E = BDSGlobalConstants::Instance()->GetBeamKineticEnergy() * (1 + energySpread * GaussGen->shoot());
      break;
    }
  case _GAUSSIAN_MATRIX :
    {
#ifdef DEBUG 
      G4cout<< "BDSBunch::GetNextParticle> V0 : " << X0 << " " << Xp0 << " " << Y0 << " " << Yp0 << " " << T0 << G4endl;
#endif

      CLHEP::HepVector v = GaussMultiGen->fire();
      x0 = v[0]*CLHEP::m;
      xp = v[1]*CLHEP::rad;
      y0 = v[2]*CLHEP::m;
      yp = v[3]*CLHEP::rad;
      t  = v[4];
      z0 = Z0*CLHEP::m + t*CLHEP::c_light;
      E  = BDSGlobalConstants::Instance()->GetBeamKineticEnergy() * v[5];
      
      if (Zp0<0)
        zp = -sqrt(1.-xp*xp -yp*yp);
      else
        zp =  sqrt(1.-xp*xp -yp*yp);

#ifdef DEBUG 
      G4cout<< "BDSBunch::GetNextParticle>" << " GAUSSIAN_MATRIX : "<<G4endl;
#endif
      break;
    }

  case _RING:
    {
#ifdef DEBUG 
      G4cout<< "BDSBunch : " <<"RING: "<<G4endl
            <<" X0= "<<X0<<" m"<<G4endl
            <<" Y0= "<<Y0<<" m"<<G4endl
            <<" Z0= "<<Z0<<" m"<<G4endl
            <<" T0= "<<T0<<" s"<<G4endl
            <<" Xp0= "<<Xp0<<G4endl
            <<" Yp0= "<<Yp0<<G4endl
            <<" Zp0= "<<Zp0<<G4endl
            <<" rMin= "<<rMin<<" m"<<G4endl
            <<" rMax= "<<rMax<<" m"<<G4endl
            <<" relative energy spread= "<<energySpread<<G4endl;
#endif
     
     
     r = ( rMin + (rMax - rMin) *  rand() / RAND_MAX );
     phi = 2 * CLHEP::pi * rand() / RAND_MAX;
     
     x0 = ( X0 + r * sin(phi) ) * CLHEP::m;
     y0 = ( Y0 + r * cos(phi) ) * CLHEP::m;
     z0 = Z0 * CLHEP::m;
     xp = Xp0 * CLHEP::rad;
     yp = Yp0 * CLHEP::rad;
     if (Zp0<0)
       zp = -sqrt(1.-xp*xp -yp*yp);
     else
       zp = sqrt(1.-xp*xp -yp*yp);
     t = T0 * CLHEP::s;
     E = BDSGlobalConstants::Instance()->GetBeamKineticEnergy()
       * (1 + energySpread/2. * (1. -2. * FlatGen->shoot()));
     break;
    }
  case _ESHELL:
    {// generate elliptical shell - first generate on S1 and then transform into ellipse
      
#ifdef DEBUG 
      G4cout<< "BDSBunch : " <<"SHELL: " 
            <<" X0= " <<X0<<" m"<<G4endl
            <<" Y0= " <<Y0<<" m"<<G4endl
	    <<" Z0= " <<Z0<<" m"<<G4endl
            <<" T0= " <<T0<<" s"<<G4endl
            <<" Xp0= " <<Xp0<<G4endl
            <<" Yp0= " <<Yp0<<G4endl
            <<" Zp0= " <<Zp0<<G4endl
            <<" shellX= " <<shellx<<" m"<<G4endl
            <<" shellY= " <<shelly<<" m"<<G4endl
            <<" shellXp= " <<shellxp<<G4endl
            <<" shellYp= " <<shellyp<<G4endl
            <<" relative energy spread= "<<energySpread<<G4endl;
#endif
      
      phi = 2 * CLHEP::pi * FlatGen->shoot();
      
      x0 = (X0 + sin(phi) * shellx) * CLHEP::m;
      xp = Xp0 + cos(phi) * shellxp;
      
      phi = 2 * CLHEP::pi * FlatGen->shoot();
      
      y0 = (Y0 + sin(phi) * shelly) * CLHEP::m;
      yp = Yp0 + cos(phi) * shellyp;
      
      z0 = Z0 * CLHEP::m;
      if (Zp0<0)
        zp = -sqrt(1.-xp*xp -yp*yp);
      else
        zp = sqrt(1.-xp*xp -yp*yp);
      
      t = T0 * CLHEP::s;
      E = BDSGlobalConstants::Instance()->GetBeamKineticEnergy()
        * (1 + energySpread/2. * (1. -2. * FlatGen->shoot()));
      break;
    }
   
  case _GUINEAPIG_BUNCH:
    {
      if(ReadValue(E))
        {
         ReadValue(x0);
         ReadValue(y0);
         ReadValue(z0);
         ReadValue(xp);
         ReadValue(yp);
	 
         E*=CLHEP::GeV;
         x0*= CLHEP::micrometer;
         y0*= CLHEP::micrometer;
         z0*= CLHEP::micrometer;
         xp*=1.e-6*CLHEP::radian;
         yp*=1.e-6*CLHEP::radian;
         zp=sqrt(1.-xp*xp -yp*yp);  
         t=0; 
         // use the Kinetic energy:
         E-=BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass();
        }
      break;
    }
  case _GUINEAPIG_SLAC:
    {
      if(ReadValue(E))
        {
          ReadValue(xp);
          ReadValue(yp);
          ReadValue(z0);
          ReadValue(x0);
          ReadValue(y0);
          
          E*=CLHEP::GeV;
          x0*= CLHEP::nanometer;
          y0*= CLHEP::nanometer;
          z0*= CLHEP::micrometer;
          xp*=CLHEP::radian;
          yp*=CLHEP::radian;
          zp=sqrt(1.-xp*xp -yp*yp);  
          t=0; 
	  weight=1;
          // use the Kinetic energy:
          E-=BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass();
        }
      else{
        InputBunchFile.clear();
        InputBunchFile.seekg(0);
        skip(nlinesIgnore * 6);
        GetNextParticle(x0,y0,z0,xp,yp,zp,t,E,weight);
      }
      break;
    }
  case _GUINEAPIG_PAIRS:
    {
      if(ReadValue(E))
        {
          ReadValue(xp);
          ReadValue(yp);
          ReadValue(zp);
          ReadValue(x0);
          ReadValue(y0);
          ReadValue(z0);
          if(E>0) BDSGlobalConstants::Instance()->SetParticleDefinition(G4ParticleTable::
                                                    GetParticleTable()
                                                    ->FindParticle("e-"));
          if(E<0) BDSGlobalConstants::Instance()->SetParticleDefinition(G4ParticleTable::
                                                    GetParticleTable()
                                                    ->FindParticle("e+"));
          E=fabs(E)*CLHEP::GeV;
          x0*= CLHEP::nanometer;
          y0*= CLHEP::nanometer;
          z0*= CLHEP::nanometer;
          xp*= CLHEP::radian;
          yp*= CLHEP::radian;
          zp*= CLHEP::radian;
	  // Using the sign of the pair file zp
	  // but calculating zp more accurately
          if(zp<0) zp = -sqrt(1-(xp*xp+yp*yp));
          else zp = sqrt(1-(xp*xp+yp*yp));
          t=0; 
          // use the Kinetic energy:
          E-=BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass();
        }
      break;
    }
  case _CAIN:
    {// Note that for the CAIN input the following variables are read in but NOT used by BDSIM:
     //     generation, weight, spin_x, spin_y, spin_z
      
      G4int type;
      G4int gen;
      G4int pos;
      //      G4double weight; // JS: weight shadows output parameter!
      G4double part_mass;
      G4double px,py,pz;
      G4double sx;
      G4double sy;
      G4double sz;
      G4String dbl_var;
      G4String rep = 'E';      
      
      if(ReadValue(type))
        {
          
          ReadValue(gen); // Read in but not currently used
          
          ReadValue(dbl_var);
          if(dbl_var.contains('D'))
            {
              pos = dbl_var.first('D');
	      weight = atof( dbl_var.replace(pos,1,rep.data(),1) );
            }
          else weight = atof(dbl_var);
          // weight read in but not currently used
          
          ReadValue(dbl_var);
          if(dbl_var.contains('D'))
	    {
	      pos = dbl_var.first('D');
	      t = atof( dbl_var.replace(pos,1,rep.data(),1) );
	    }
          else t = atof(dbl_var);
          
          ReadValue(dbl_var);
          if(dbl_var.contains('D'))
	    {
	      pos = dbl_var.first('D');
	      x0 = atof( dbl_var.replace(pos,1,rep.data(),1) );
	    }
          else x0 = atof(dbl_var);
          
          ReadValue(dbl_var);
          if(dbl_var.contains('D'))
            {
	      pos = dbl_var.first('D');
	      y0 = atof( dbl_var.replace(pos,1,rep.data(),1) );
            }
          else y0 = atof(dbl_var);
          
          ReadValue(dbl_var);
          if(dbl_var.contains('D'))
            {
              pos = dbl_var.first('D');
              z0 = atof( dbl_var.replace(pos,1,rep.data(),1) );
            }
          else z0 = atof(dbl_var);
          
          ReadValue(dbl_var);
          if(dbl_var.contains('D'))
            {
	      pos = dbl_var.first('D');
	      E = atof( dbl_var.replace(pos,1,rep.data(),1) );
            }
          else E = atof(dbl_var);
          
          ReadValue(dbl_var);
          if(dbl_var.contains('D'))
	    {
	      pos = dbl_var.first('D');
	      px = atof( dbl_var.replace(pos,1,rep.data(),1) );
	    }
          else px = atof(dbl_var);
          
          ReadValue(dbl_var);
          if(dbl_var.contains('D'))
            {
              pos = dbl_var.first('D');
              py = atof( dbl_var.replace(pos,1,rep.data(),1) );
            }
          else py = atof(dbl_var);
          
          ReadValue(dbl_var);
          if(dbl_var.contains('D'))
            {
              pos = dbl_var.first('D');
              pz = atof( dbl_var.replace(pos,1,rep.data(),1) );
            }
          else pz = atof(dbl_var);
          
          // Spin Components read in - but not currently used
          ReadValue(sx);
          ReadValue(sy);
          ReadValue(sz);
          
          if(type==1) 
            BDSGlobalConstants::Instance()->SetParticleDefinition(G4ParticleTable::
                                              GetParticleTable()
                                              ->FindParticle("gamma"));
          else if(type==2) 
            BDSGlobalConstants::Instance()->SetParticleDefinition(G4ParticleTable::
                                              GetParticleTable()
                                              ->FindParticle("e-"));
          
          else if(type==3) 
            BDSGlobalConstants::Instance()->SetParticleDefinition(G4ParticleTable::
                                              GetParticleTable()
                                              ->FindParticle("e+"));
          
          t*= CLHEP::m/CLHEP::c_light;
          x0*= CLHEP::m;
          y0*= CLHEP::m;
          z0*= CLHEP::m;
          E*=CLHEP::eV;
          px*=CLHEP::eV/CLHEP::c_light;
          py*=CLHEP::eV/CLHEP::c_light;
          pz*=CLHEP::eV/CLHEP::c_light;
          
          part_mass = BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass();
          // use the Kinetic energy:
          E-=part_mass;
          
          // calculate the momentum direction needed for BDSPrimaryGenerator
          xp = px*CLHEP::c_light / sqrt(E*E + 2*E*part_mass);
          yp = py*CLHEP::c_light / sqrt(E*E + 2*E*part_mass);
          zp = pz*CLHEP::c_light / sqrt(E*E + 2*E*part_mass);
          
#ifdef DEBUG 
          G4cout << "Bunch input was: " << G4endl;
          G4cout << type << "\t"
                 << gen << "\t"
                 << weight << "\t"
                 << t/CLHEP::m << "\t"
                 << x0/CLHEP::m << "\t"
                 << y0/CLHEP::m << "\t"
                 << z0/CLHEP::m << "\t"
                 << E/CLHEP::eV << "\t"
                 << px/(CLHEP::eV/CLHEP::c_light) << "\t"
                 << py/(CLHEP::eV/CLHEP::c_light) << "\t"
                 << pz/(CLHEP::eV/CLHEP::c_light) << "\t"
                 << sx << "\t"
                 << sy << "\t"
                 << sz << "\t"
                 << xp << "\t"
                 << yp << "\t"
                 << zp << "\t"
                 << G4endl << G4endl;
#endif
        }
      break;
    }
  case _UDEF:
    {
      E = x0 = y0 = z0 = xp = yp = zp = 0;

      bool zpdef = false; //keeps record whether zp has been read from file
      bool tdef = false; //keeps record whether t has been read from file

     
      G4int type;

      //Skip the a number of lines defined by the user option.
#ifdef DEBUG 
      G4cout<< "BDSBunch : " <<"UDEF_BUNCH: skipping "<<nlinesIgnore<<"  lines"<<G4endl;
#endif
      skip((G4int)(nlinesIgnore * fields.size()));
     
      std::list<struct BDSBunch::Doublet>::iterator it;
      for(it=fields.begin();it!=fields.end();it++)
	{
#ifdef DEBUG 
	  G4cout<< "BDSBunch : " <<it->name<<"  ->  "<<it->unit<<G4endl;
#endif
	  if(it->name=="E") { 
	    ReadValue(E); E *= ( CLHEP::GeV * it->unit ); 
#ifdef DEBUG 
	    G4cout << "******** Particle Mass = " << BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass() << G4endl;
	   G4cout << "******** Particle Total Energy = " << E << G4endl;
#endif
	   E-=BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass();
#ifdef DEBUG 
	   G4cout << "******** Particle Kinetic Energy = " << E << G4endl;
	   G4cout<< "BDSBunch : " << E <<G4endl;
#endif
	  }
	 G4double P=0;
         if(it->name=="P") { 
	   ReadValue(P); P *= ( CLHEP::GeV * it->unit ); //Paticle momentum
	   G4double particleMass = BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass();
	   G4double totalEnergy = sqrt(P*P + particleMass*particleMass);
	   E = totalEnergy - particleMass;
#ifdef DEBUG 
	   G4cout << "******** Particle Mass = " << particleMass << G4endl;
	   G4cout << "******** Particle Total Energy = " << totalEnergy << G4endl;
	   G4cout << "******** Particle Kinetic Energy = " << E << G4endl;
	   G4cout<< "BDSBunch : " << E <<G4endl;
#endif
         }
         if(it->name=="t") { ReadValue(t); t *= ( CLHEP::s * it->unit ); tdef = true; }
         if(it->name=="x") { ReadValue(x0); x0 *= ( CLHEP::m * it->unit ); 
#ifdef DEBUG 
           G4cout<< "BDSBunch : " << x0 <<G4endl;
#endif
         }
         if(it->name=="y") { 
           ReadValue(y0); y0 *= ( CLHEP::m * it->unit ); 
#ifdef DEBUG 
           G4cout<< "BDSBunch : " << y0 <<G4endl;
#endif
         }
         if(it->name=="z") { 
           ReadValue(z0); z0 *= ( CLHEP::m * it->unit ); 
#ifdef DEBUG 
           G4cout<< "BDSBunch : " << z0 <<G4endl;
#endif
         }
         if(it->name=="xp") { ReadValue(xp); xp *= ( CLHEP::radian * it->unit ); }
         if(it->name=="yp") { ReadValue(yp); yp *= ( CLHEP::radian * it->unit ); }
         if(it->name=="zp") { ReadValue(zp); zp *= ( CLHEP::radian * it->unit ); zpdef = true;}
         if(it->name=="pt") {
           ReadValue(type);
           if(InputBunchFile.good()){
             BDSGlobalConstants::Instance()->SetParticleName(G4ParticleTable::GetParticleTable()->FindParticle(type)->GetParticleName());
             BDSGlobalConstants::Instance()->SetParticleDefinition(G4ParticleTable::
                                               GetParticleTable()
                                               ->FindParticle(type));
             if(!BDSGlobalConstants::Instance()->GetParticleDefinition()) 
               {
                 G4Exception("Particle not found, quitting!", "-1", FatalErrorInArgument, "");
                 exit(1);
               }
           }
         }
	 if(it->name=="weight") ReadValue(weight);

	 // compute zp from xp and yp if it hasn't been read from file
	 if (!zpdef) zp=sqrt(1.-xp*xp -yp*yp);
	 // compute t from z0 if it hasn't been read from file
	 if (!tdef) t=0; 
	 // use the Kinetic energy:
	 //          if(BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGEncoding() != 22){
	 //}
	}
     //Add the global offset Z
     z0=z0+Z0*CLHEP::m;
     break;
    }
  default:
    {
      G4Exception("BDSBunch: Unknown distribution file type!", "-1", FatalErrorInArgument, "");
    }
  }

#ifdef DEBUG
  // particle properties
  G4cout << __METHOD_NAME__ <<G4endl
	 <<" x0 = "<<x0<<" m"<<G4endl
	 <<" y0 = "<<y0<<" m"<<G4endl
	 <<" z0 = "<<z0<<" m"<<G4endl
	 <<" t = "<<t<<" s"<<G4endl
	 <<" xp = "<<xp<<G4endl
	 <<" yp = "<<yp<<G4endl
	 <<" zp = "<<zp<<G4endl
	 <<" E = "<<E<< " MeV"<<G4endl
	 <<" weight= "<<weight<<G4endl;
#endif

}


G4double BDSBunch::GetEmitX()
{
  return emitX;
}

G4double BDSBunch::GetEmitY()
{
  return emitY;
}

G4double BDSBunch::GetAlphaX()
{
  return alphaX;
}

G4double BDSBunch::GetAlphaY()
{
  return alphaY;
}

G4double BDSBunch::GetBetaX()
{
  return betaX;
}

G4double BDSBunch::GetBetaY()
{
  return betaY;
}



// set initial bunch distribution parameters in square/circle case 
void BDSBunch::SetEnvelopeT(double val) 
{
  envelopeT= val;
} 

void BDSBunch::SetEnvelopeX(double val)
{ 
  envelopeX = val;
}
 
void BDSBunch::SetEnvelopeY(double val)
{
  envelopeY = val;
} 

void BDSBunch::SetEnvelopeXp(double val)
{
  envelopeXp = val;
}

void BDSBunch::SetEnvelopeYp(double val)
{
  envelopeYp = val;
}

// set initial bunch distribution parameters in Gaussian case 
void BDSBunch::SetSigmaT(double val) 
{
  sigmaT= val;
} 

void BDSBunch::SetSigmaX(double val)
{ 
  sigmaX = val;
}
 
void BDSBunch::SetSigmaY(double val)
{
  sigmaY = val;
} 

void BDSBunch::SetSigmaXp(double val)
{
  sigmaXp = val;
}

void BDSBunch::SetSigmaYp(double val)
{
  sigmaYp = val;
}

void BDSBunch::SetX0(double val)
{
  X0 = val;
} 

void BDSBunch::SetY0(double val)
{
  Y0 = val;
}

void BDSBunch::SetXp0(double val)
{
  Xp0 = val;
}

void BDSBunch::SetYp0(double val)
{
  Yp0 = val;
}

void BDSBunch::SetEmitX(double val)
{
  emitX = val;
}

void BDSBunch::SetEmitY(double val)
{
  emitY = val;
}

void BDSBunch::SetAlphaX(double val)
{
  alphaX = val;
}

void BDSBunch::SetAlphaY(double val)
{
  alphaY = val;
}

void BDSBunch::SetBetaX(double val)
{
  betaX = val;
}

void BDSBunch::SetBetaY(double val)
{
  betaY = val;
}

void BDSBunch::SetEnergySpread(double val)
{
  energySpread = val;
}

template <typename Type> G4bool  BDSBunch::ReadValue(Type &value){
  InputBunchFile>>value; 
  if (InputBunchFile.eof()){ //If the end of the file is reached go back to the beginning of the file.
#ifdef DEBUG
    G4cout << "BDSBunch.cc> End of file reached. Returning to beginning of file." << G4endl;
#endif
    CloseBunchFile();
    OpenBunchFile();
    InputBunchFile>>value; 
  } 

  return !InputBunchFile.eof();
}

void BDSBunch::OpenBunchFile(){
  InputBunchFile.open(inputfile);
  if(!InputBunchFile.good()){ 
    G4cerr<<"Cannot open bunch file "<<inputfile<<G4endl; 
    exit(1); 
  } 
}

void BDSBunch::CloseBunchFile(){
  InputBunchFile.close();
}

//Stores input beam as a "sampler hit" to be be later saved to the output file.
