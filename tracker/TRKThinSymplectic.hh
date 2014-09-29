#ifndef TRKThinSymplectic_h
#define TRKThinSymplectic_h

#include "TRKThin.hh"

/**
 * @brief Symplectic Thin tracking based on Sixtrack methods
 */
class TRKThinSymplectic : public TRKThin {
public:

  // todo discuss do we need trackingsteps for symplectic?
  TRKThinSymplectic(int trackingSteps);
		     
  virtual ~TRKThinSymplectic(); 

  /// returns tracking type
  //  TRKType trackingType()const {return type;}

  /// import all Track declarations from TRKThin, otherwise these are overwritten by specialised methods
  using TRKThin::Track;

  /**
   * Tracks a bunch through an element, replaces TRKThin method
   * @param[in]  el    The TRKElement.
   * @param[in]  bunch The bunch.
   */
  ///@{
  virtual void Track(TRKDrift* el, TRKBunch* bunch);
  virtual void Track(TRKDipole* el, TRKBunch* bunch);
  ///@}

  //  same as in TRKThin
  //  void Track(TRKQuadrupole* el, TRKBunch* bunch);
  //  void Track(TRKSextupole* el, TRKBunch* bunch);
  //  void Track(TRKOctupole* el, TRKBunch* bunch);

private:
  TRKThinSymplectic(); ///< not implemented
};

#endif
