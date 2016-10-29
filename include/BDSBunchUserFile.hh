#ifndef BDSBUNCHUSERFILE_H
#define BDSBUNCHUSERFILE_H 

#include "BDSBunchInterface.hh"
#include <fstream>
#include <list>

/**
 * @brief A bunch distribution that reads a user specified column file.
 * 
 */
template <class T>
class BDSBunchUserFile: public BDSBunchInterface
{ 
private:
  void ParseFileFormat();
  void OpenBunchFile();
  void CloseBunchFile();
  void skip(G4int nvalues);
  T InputBunchFile;
	template <typename Type> G4bool ReadValue(Type &value);
  struct Doublet {
    G4String name;
    G4double unit; ///< relative to SI units, i.e. mm=0.001 etc.
  };
  std::list<struct Doublet> fields;
  void SetDistribFile(G4String filename);
  void SetBunchFormat(G4String val) {bunchFormat=val;}
  void SetNLinesIgnore(G4int val)   {nlinesIgnore=val;}
  G4double ParseEnergyUnit(G4String &fmt);
  G4double ParseLengthUnit(G4String &fmt);
  G4double ParseAngleUnit(G4String &fmt);
  G4double ParseTimeUnit(G4String &fmt);

protected : 
  G4String distribFile; ///< bunch file
  G4String bunchFormat; ///< the format of the file
  G4int nlinesIgnore; ///< number of lines that will be ignored after each particle (default 0)
  
public: 
  BDSBunchUserFile();
  virtual ~BDSBunchUserFile();
  virtual void SetOptions(const GMAD::Options& opt);
  virtual void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
			       G4double& xp, G4double& yp, G4double& zp,
			       G4double& t , G4double&  E, G4double& weight);  
};

#endif
