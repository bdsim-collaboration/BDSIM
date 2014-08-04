#ifndef BDSBunchUserFile_h
#define BDSBunchUserFile_h 

#include "BDSBunchInterface.hh"

class BDSBunchUserFile : public BDSBunchInterface { 
private:
  void ParseFileFormat();
  void OpenBunchFile();
  void CloseBunchFile();
  std::ifstream InputBunchFile;
  template <typename Type> G4bool ReadValue(Type &value);

protected : 
  G4String distribFile;
  G4String bunchFormat;
  
public: 
  BDSBunchUserFile();
  ~BDSBunchUserFile(); 
  void SetOptions(struct Options& opt);
  void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		       G4double& xp, G4double& yp, G4double& zp,
		       G4double& t , G4double&  E, G4double& weight);  
  
  G4String GetDistibFile() {return distribFile;}
  G4String GetBunchFormat() {return bunchFormat;}
};

#endif
