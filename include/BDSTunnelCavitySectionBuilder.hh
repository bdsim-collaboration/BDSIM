#ifndef __BDSTUNNELCAVITYSECTIONBUILDER_H
#define __BDSTUNNELCAVITYSECTIONBUILDER_H

#include "BDSTunnelCavitySection.hh"

class BDSTunnelCavitySectionBuilder{
public:
  virtual ~BDSTunnelCavitySectionBuilder();
  BDSTunnelCavitySectionBuilder();
  
  void createNewTunnelCavitySection(BDSAcceleratorComponent* var);
  BDSTunnelCavitySection* tunnelCavitySection();

  virtual void build() = 0;
  
protected:
  BDSTunnelCavitySection* _tunnelCavitySection;
};


#endif
