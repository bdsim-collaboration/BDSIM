#include "beamBase.h"

#include <iostream>

using namespace GMAD;

BeamBase::BeamBase()
{
  particleName           = "";
  beamEnergy             = 0.0;
  distrType              = "reference";
  xDistrType             = "";
  yDistrType             = "";
  zDistrType             = "";
  distrFile              = "";
  distrFileFormat        = "";
  matchDistribFileLength = true;  
  nlinesIgnore           = 0;
  
  X0  = 0.0;
  Y0  = 0.0;
  Z0  = 0.0;
  S0  = 0.0;
  Xp0 = 0.0;
  Yp0 = 0.0;
  Zp0 = 0.0;
  T0  = 0.0;
  E0  = 0.0;
  sigmaT = 0.0;
  sigmaE = 0.0;

  betx    = 0.0;
  bety    = 0.0;
  alfx    = 0.0;
  alfy    = 0.0;
  emitx   = 0.0;
  emity   = 0.0;
  dispx   = 0.0;
  dispy   = 0.0;
  dispxp  = 0.0;
  dispyp  = 0.0;
  sigmaX  = 0.0;
  sigmaXp = 0.0;
  sigmaY  = 0.0;
  sigmaYp = 0.0;
  
  envelopeX  = 0.0;
  envelopeXp = 0.0;
  envelopeY  = 0.0;
  envelopeYp = 0.0;
  envelopeT  = 0.0;
  envelopeE  = 0.0;
  envelopeR  = 0.0;
  envelopeRp = 0.0;
  
  sigma11 = 0.0,sigma12 = 0.0,sigma13 = 0.0,sigma14 = 0.0,sigma15 = 0.0,sigma16 = 0.0;
  sigma22 = 0.0,sigma23 = 0.0,sigma24 = 0.0,sigma25 = 0.0,sigma26 = 0.0;
  sigma33 = 0.0,sigma34 = 0.0,sigma35 = 0.0,sigma36 = 0.0;
  sigma44 = 0.0,sigma45 = 0.0,sigma46 = 0.0;
  sigma55 = 0.0,sigma56 = 0.0;
  sigma66 = 0.0;
  
  shellX  = 0.0;
  shellXp = 0.0;
  shellY  = 0.0;
  shellYp = 0.0;
  shellXWidth  = 0.0;
  shellXpWidth = 0.0;
  shellYWidth  = 0.0;
  shellYpWidth = 0.0;

  Rmin = 0.0;
  Rmax = 0.0;

  haloEmitX =  0;
  haloEmitY =  0;

  // TODO : check if we really need default arguments here as
  // yields incorrect behaviour when collimators are not used
#if 0
  haloEnvelopeEmitX     =  1e9;
  haloEnvelopeEmitY     =  1e9;
  haloEnvelopeCollMinX  =  1e9;
  haloEnvelopeCollMaxX  = -1e9;
  haloEnvelopeCollMinXp =  1e9;
  haloEnvelopeCollMaxXp = -1e9;
  haloEnvelopeCollMinY  =  1e9;
  haloEnvelopeCollMaxY  = -1e9;
  haloEnvelopeCollMinYp =  1e9;
  haloEnvelopeCollMaxYp = -1e9;
#endif
  haloEnvelopeEmitX     =  0;
  haloEnvelopeEmitY     =  0;
  haloEnvelopeCollMinX  =  0;
  haloEnvelopeCollMaxX  =  0;
  haloEnvelopeCollMinXp =  0;
  haloEnvelopeCollMaxXp =  0;
  haloEnvelopeCollMinY  =  0;
  haloEnvelopeCollMaxY  =  0;
  haloEnvelopeCollMinYp =  0;
  haloEnvelopeCollMaxYp =  0;

  haloPSWeightParameter = 1.0;
  haloPSWeightFunction  = "";
}
