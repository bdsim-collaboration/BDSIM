#ifndef BDSPROCESSMAP_H
#define BDSPROCESSMAP_H

#include "globals.hh" // geant4 types / globals

#include <unordered_map>

/**
 * @brief Map of all process names to IDs.
 *
 * @author L. Nevay / S. Boogert.
 */

class BDSProcessMap
{
public:
  /// Singleton accessor.
  static BDSProcessMap* Instance();

  ~BDSProcessMap();

  /// Main access method for getting the name of processes.
  G4String operator()(const G4int& type, const G4int& subType = -1) const;

  /// Despatched function to operator() for getting the name of processes.
  G4String GetProcessName(const G4int& type, const G4int& subType = -1) const
  {return operator()(type, subType);}

private:
  /// Private default constructor as singelton pattern.
  BDSProcessMap();

  /// Singleton instance
  static BDSProcessMap* instance;

  /// Typedef to make syntax more readable.
  typedef std::unordered_map<G4int, G4String> processMap;

  /// Map of main process categories
  processMap processes;

  /// Map of subprocesses mapped (again) to main processes categories. Safer
  /// than using a vector as there's not guarantee there in order by geant or
  /// contiguous.
  std::unordered_map<G4int, processMap> subProcesses;
};

#endif
