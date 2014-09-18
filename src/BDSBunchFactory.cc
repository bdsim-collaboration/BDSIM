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

BDSBunchInterface* BDSBunchFactory::createBunch(G4String distribType)
{
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
  else if(distribType == "gausstwiss") 
    bdsBunch = new BDSBunchTwiss();
  else if(distribType == "userfile")
    bdsBunch = new BDSBunchUserFile();
  else if(distribType == "composite")
    bdsBunch = new BDSBunchComposite();
  else {
    G4cerr << "distribType not found " << distribType << G4endl;
    exit(1);
  }
  return bdsBunch;
}
