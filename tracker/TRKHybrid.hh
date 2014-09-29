#ifndef TRKHybrid_h
#define TRKHybrid_h

#include "TRKStrategy.hh"
#include "TRKElement.hh"

class TRKBunch;
class TRKDipole;
class TRKQuadrupole;
class TRKSextupole;
class TRKOctupole;

/**
 * @brief hybrid geant4 type tracking
 */
class TRKHybrid : public TRKStrategy {
public:

  TRKHybrid(int trackingSteps);
		     
  virtual ~TRKHybrid(); 

  /// returns tracking type
  //  TRKType trackingType()const {return type;}

  /**
   * Tracks a bunch through an element
   * @param[in]  el    The TRKElement.
   * @param[in]  bunch The bunch.
   */
  ///@{
  void Track(TRKDrift* el, TRKBunch* bunch);
  void Track(TRKDipole* el, TRKBunch* bunch);
  void Track(TRKQuadrupole* el, TRKBunch* bunch);
  void Track(TRKSextupole* el, TRKBunch* bunch);
  void Track(TRKOctupole* el, TRKBunch* bunch);
  ///@}

private:
  TRKHybrid(); ///< not implemented
};


#endif
