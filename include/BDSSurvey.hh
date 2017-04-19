#ifndef BDSSURVEY_H
#define BDSSURVEY_H

#include "globals.hh" // geant4 types / globals

#include <fstream>
#include <vector>

class BDSBeamline;
class BDSBeamlineElement;
class BDSMagnetStrength;

/**
 * @brief A class of functions to output Geant4/Mokka/BDSIM parameters for the beamline.
 *
 * This writes all details about elements in a beamline including geometrical placement
 * information, fields, etc, much like MadX Tfs file.
 */

class BDSSurvey
{
public:
  explicit BDSSurvey(G4String filename);  
  ~BDSSurvey();

  /// write line
  void Write(BDSBeamlineElement* beamlineElement);

  /// Write a whole beamline, summary of it and close the file.
  void Write(BDSBeamline* beamline);
  
private:
  /// Write header
  void WriteHeader();
  
  /// Output file stream
  std::ofstream survey;

  /// The default zero strength instance of BDSMagnetStrength that
  /// can be queried in place of one not being available - ie when
  /// the element isn't a magnet.
  BDSMagnetStrength const* nullStrength;

  /// Cache of all the possible magnet strength parameters.
  std::vector<G4String> magnetKeys;
};

#endif
