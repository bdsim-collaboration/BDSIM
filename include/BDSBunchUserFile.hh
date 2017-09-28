#ifndef BDSBUNCHUSERFILE_H
#define BDSBUNCHUSERFILE_H 

#include "BDSBunch.hh"
#include <fstream>
#include <list>

#ifdef USE_GZSTREAM
#include "gzstream.h"
#endif

/**
 * @brief A bunch distribution that reads a user specified column file.
 * 
 * @author Lawrence Deacon
 */

template <class T>
class BDSBunchUserFile: public BDSBunch
{
public: 
  BDSBunchUserFile();
  virtual ~BDSBunchUserFile();
  virtual void SetOptions(const GMAD::Beam& beam,
			  G4Transform3D beamlineTransformIn = G4Transform3D::Identity);
  virtual void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
			       G4double& xp, G4double& yp, G4double& zp,
			       G4double& t , G4double&  E, G4double& weight);
protected: 
  G4String distrFile; ///< bunch file
  G4String bunchFormat; ///< the format of the file
  G4int nlinesIgnore; ///< number of lines that will be ignored after each particle (default 0)
  
private:
  void ParseFileFormat();
  void OpenBunchFile();
  void SkipLines();
  void CloseBunchFile();
  void skip(G4int nvalues);
  T InputBunchFile;
	template <typename Type> G4bool ReadValue(Type &value);
  struct Doublet {
    G4String name;
    G4double unit; ///< relative to SI units, i.e. mm=0.001 etc.
  };
  std::list<Doublet> fields;
  void SetDistrFile(G4String filename);
  void SetBunchFormat(G4String val) {bunchFormat=val;}
  void SetNLinesIgnore(G4int val)   {nlinesIgnore=val;}
  G4double ParseEnergyUnit(G4String &fmt);
  G4double ParseLengthUnit(G4String &fmt);
  G4double ParseAngleUnit(G4String &fmt);
  G4double ParseTimeUnit(G4String &fmt);
};

#endif
