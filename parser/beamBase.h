#ifndef BEAMBASE_H
#define BEAMBASE_H

#include <string>

namespace GMAD
{
  /**
   * @brief Options for a beam distribution.
   *
   * This is the basic structure that can be serialised in an output
   * file and only contains built in C++ types. No Geant4 types are
   * to be used. The Beam class inherits this one.
   */
  class BeamBase
    {
    public:
      BeamBase();

      ///@{ beam parameters
      std::string particleName;
      double      beamEnergy;
      std::string distrType;
      std::string xDistrType;
      std::string yDistrType;
      std::string zDistrType;
      std::string distrFile;
      std::string distrFileFormat;
      bool        matchDistrFileLength;
      ///@}
      int         nlinesIgnore; ///< ignore first lines in the input bunch file

      ///@{ initial beam centroid
      double X0, Y0, Z0, S0;
      double Xp0, Yp0, Zp0;
      double T0;
      double E0;
      ///@}

      /// bunch length
      double sigmaT;

      /// for the gaussian, elliptic shell, ring distributions
      double sigmaE;
      
      ///@{ initial twiss parameters
      double betx, bety, alfx, alfy, emitx, emity, dispx, dispy, dispxp, dispyp;
      ///@}
      
      ///@{ for the gaussian beam distribution
      double sigmaX, sigmaXp, sigmaY, sigmaYp;
      ///@}
      
      ///@{ for the circle/square beam distribution
      double envelopeX, envelopeXp, envelopeY, envelopeYp, envelopeT, envelopeE;
      double envelopeR, envelopeRp;
      ///@}
      
      ///@{ for the gaussian sigma matrix distribution
      double sigma11, sigma12, sigma13, sigma14, sigma15, sigma16;
      double          sigma22, sigma23, sigma24, sigma25, sigma26;
      double                   sigma33, sigma34, sigma35, sigma36;
      double                            sigma44, sigma45, sigma46;
      double                                     sigma55, sigma56;
      double                                              sigma66;
      ///@}
      
      ///@{ for the elliptic shell distribution
      double shellX, shellXp, shellY, shellYp;
      double shellXWidth, shellXpWidth, shellYWidth, shellYpWidth;
      ///@}
      
      ///@{ for the ring beam distribution
      double Rmin, Rmax;
      ///@}
      
      ///@{ for the halo distribution
      double haloNSigmaXInner;
      double haloNSigmaXOuter;
      double haloNSigmaYInner;
      double haloNSigmaYOuter;
      double haloXCutInner;
      double haloYCutInner;
      double haloPSWeightParameter;
      std::string haloPSWeightFunction;
      ///@}
    };
}

#endif
