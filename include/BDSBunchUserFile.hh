#ifndef BDSBunchUserFile_h
#define BDSBunchUserFile_h 

#include "BDSBunchInterface.hh"
#include <fstream>
#include <list>

class BDSBunchUserFile : public BDSBunchInterface { 
private:
  void ParseFileFormat();
  void OpenBunchFile();
  void CloseBunchFile();
  void skip(G4int nvalues);
  std::ifstream InputBunchFile;
  template <typename Type> G4bool ReadValue(Type &value);
  struct Doublet {
    G4String name;
    G4double unit; // relative to SI units, i.e. mm=0.001 etc.
  };
  std::list<struct Doublet> fields;
  void SetDistribFile(G4String val) {distribFile=val;}
  void SetBunchFormat(G4String val) {bunchFormat=val;}
  void SetNLinesIgnore(G4int val)   {nlinesIgnore=val;}
  G4double ParseEnergyUnit(G4String &fmt);
  G4double ParseLengthUnit(G4String &fmt);
  G4double ParseAngleUnit(G4String &fmt);
  G4double ParseTimeUnit(G4String &fmt);

protected : 
  G4String distribFile;
  G4String bunchFormat;
  G4int nlinesIgnore;
  
public: 
  BDSBunchUserFile();
  ~BDSBunchUserFile(); 
  virtual void SetOptions(struct Options& opt);
  virtual void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
			       G4double& xp, G4double& yp, G4double& zp,
			       G4double& t , G4double&  E, G4double& weight);  
  
  G4String GetDistribFile()  {return distribFile;}
  G4String GetBunchFormat()  {return bunchFormat;}
  G4int    GetNLinesIgnore() {return nlinesIgnore;}
};

#endif
