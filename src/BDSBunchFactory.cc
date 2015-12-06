#include "BDSBunchFactory.hh"

#include "BDSBunchCircle.hh"
#include "BDSBunchComposite.hh"
#include "BDSBunchEShell.hh"
#include "BDSBunchGaussian.hh"
#include "BDSBunchInterface.hh"
#include "BDSBunchRing.hh"
#include "BDSBunchSquare.hh"
#include "BDSBunchUserFile.hh"
#include "BDSBunchTwiss.hh"
#include "BDSBunchPtc.hh"
#include "BDSBunchSixTrack.hh"
#include "BDSBunchHalo.hh"

BDSBunchInterface* BDSBunchFactory::createBunch(G4String distribType)
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  BDSBunchInterface* bdsBunch;
  if (distribType == "reference") 
    bdsBunch = new BDSBunchInterface();
  else if(distribType == "gauss" || distribType == "gaussmatrix") 
    bdsBunch = new BDSBunchGaussian(); 
  else if(distribType == "square") 
    bdsBunch = new BDSBunchSquare();
  else if(distribType == "circle") 
    bdsBunch = new BDSBunchCircle();
  else if(distribType == "ring") 
    bdsBunch = new BDSBunchRing();
  else if(distribType == "eshell") 
    bdsBunch = new BDSBunchEShell();
  else if(distribType == "sixtrack") 
    bdsBunch = new BDSBunchSixTrack();
  else if(distribType == "gausstwiss") 
    bdsBunch = new BDSBunchTwiss();
  else if(distribType == "halo") 
    bdsBunch = new BDSBunchHalo();
  else if(distribType == "userfile")
    bdsBunch = new BDSBunchUserFile();
  else if(distribType == "composite")
    bdsBunch = new BDSBunchComposite();
  else if(distribType == "ptc") 
    bdsBunch = new BDSBunchPtc(); 
  else {
    G4cerr << "distribType \"" << distribType << "\" not found" << G4endl;
    exit(1);
  }
  return bdsBunch;
}
