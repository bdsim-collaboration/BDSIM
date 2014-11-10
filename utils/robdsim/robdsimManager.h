#ifndef __ROBDSIMMANAGER_H
#define __ROBDSIMMANAGER_H

/* Add all histograms and chain sampler, eloss and ploss trees together */

class robdsimManager {
  robdsimManager(char *directory);
  ~robdsimManager();
  void fileAnalysis(); 
};


#endif
