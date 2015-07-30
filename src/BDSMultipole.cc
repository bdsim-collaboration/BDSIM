#include "BDSBeamPipeInfo.hh"
#include "BDSMagnet.hh"
#include "BDSMultipoleMagField.hh"
#include "BDSMultipole.hh"

#include "G4FieldManager.hh"
#include "G4HelixImplicitEuler.hh"
#include "G4SimpleRunge.hh"
#include "G4LogicalVolume.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4VPhysicalVolume.hh"

BDSMultipole::BDSMultipole(G4String            name,
			   G4double            length,
			   std::list<G4double> akn, // list of normal multipole strengths
			   // (NOT multiplied by multipole length)
			   std::list<G4double> aks, // list of skew multipole strengths
			   // (NOT multiplied by multipole length)
			   BDSBeamPipeInfo*    beamPipeInfo,
			   BDSMagnetOuterInfo  magnetOuterInfo):
  BDSMagnet(BDSMagnetType::multipole, name, length,
	    beamPipeInfo, magnetOuterInfo)
{
  CommonConstructor(akn,aks);
}

void BDSMultipole::CommonConstructor(std::list<G4double> akn, std::list<G4double> aks)
{
#ifdef BDSDEBUG
  if (akn.size()>0){
    G4cout<<"Building multipole of order "<<akn.size()<<G4endl;
  } else {
    G4cout<<"Building multipole of order "<<aks.size()<<G4endl;
  }
#endif

  //If the multipole has both skew and normal coefficients then they must have the same number of coefficients 
  if((akn.size() != aks.size()) && ((akn.size() >0) && (aks.size() > 0))) { G4cerr<<"ERROR : multipole skew and normal coefficients present, but to different order"<<G4endl;}
  if(akn.size() == 0 &&  aks.size() == 0) { G4cerr<<"ERROR : skew or normal multipole order must be greater than 0"<<G4endl; }

  //Check if any components are non-zero
  /*  
  G4bool fieldNonZero=false;
  for(std::list<double>::iterator it = akn.begin(); it != akn.end() && fieldNonZero == false; it++){
    if(*it != 0) fieldNonZero = true;
  }
  for(std::list<double>::iterator it = aks.begin(); it != aks.end() && fieldNonZero == false; it++){
    if(*it != 0) fieldNonZero = true;
  }
  */

  //If list of normal or skew multipole coefficients not present, create a default list containing zeroes.
  if (aks.size()==0){
    for(unsigned int i=0; i<akn.size(); i++){
      aks.push_back(0.0);
    }
  }
  if (akn.size()==0){
    for(unsigned int i=0; i<aks.size(); i++){
      akn.push_back(0.0);
    }
  }

  kn = akn;
  ks = aks;

#ifdef BDSDEBUG 
  int order = 0;
  G4cout<<"M: kn={ ";
  std::list<double>::iterator kit;
  for(kit=kn.begin();kit!=kn.end();kit++)
    {
      G4cout<<(*kit)<<"m^-"<<++order<<" ";
    }
  G4cout<<"}"<<G4endl;
  order = 0;
  G4cout<<"M: ks={ ";
  for(kit=ks.begin();kit!=ks.end();kit++)
    {
      G4cout<<(*kit)<<"m^-"<<++order<<" ";
    }
  G4cout<<"}"<<G4endl;
#endif

  itsOrder = kn.size();
}

void BDSMultipole::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper
  itsMagField = new BDSMultipoleMagField(kn,ks);
  itsEqRhs    = new G4Mag_UsualEqRhs(itsMagField);
  itsStepper  = new G4SimpleRunge(itsEqRhs);
}
