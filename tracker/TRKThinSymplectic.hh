#ifndef TRKThinSymplectic_h
#define TRKThinSymplectic_h

#include "TRKThin.hh"

/**
 * @brief Symplectic Thin tracking
 */
class TRKThinSymplectic : public TRKThin {
public:

  TRKThinSymplectic(int trackingSteps);
		     
  virtual ~TRKThinSymplectic(); 

  /// returns tracking type
  //  TRKType trackingType()const {return type;}

  /**
   * Tracks a bunch through an element
   * @param[in]  el    The TRKElement.
   * @param[in]  bunch The bunch.
   */
  ///@{
  virtual void Track(TRKDrift* el, TRKBunch* bunch);
  //  void Track(TRKDipole* el, TRKBunch* bunch);
  //  void Track(TRKQuadrupole* el, TRKBunch* bunch);
  //  void Track(TRKSextupole* el, TRKBunch* bunch);
  //  void Track(TRKOctupole* el, TRKBunch* bunch);
  ///@}

private:
  TRKThinSymplectic(); ///< not implemented
};


#endif
