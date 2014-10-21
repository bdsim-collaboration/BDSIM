#ifndef TRKHybrid_h
#define TRKHybrid_h

#include "TRKStrategy.hh"

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
  virtual void Track(TRKDrift*      el, TRKBunch* bunch);
  virtual void Track(TRKDipole*     el, TRKBunch* bunch);
  virtual void Track(TRKBend*       el, TRKBunch* bunch);
  virtual void Track(TRKQuadrupole* el, TRKBunch* bunch);
  virtual void Track(TRKSextupole*  el, TRKBunch* bunch);
  virtual void Track(TRKOctupole*   el, TRKBunch* bunch);
  virtual void Track(TRKSolenoid*   el, TRKBunch* bunch);
  ///@}

private:
  TRKHybrid(); ///< not implemented
};


#endif
