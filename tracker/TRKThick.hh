#ifndef TRKThick_h
#define TRKThick_h

#include "TRKStrategy.hh"
#include "TRKElement.hh"

/**
 * @brief Thick tracking
 */
class TRKThick : public TRKStrategy {
public:

  TRKThick(int trackingSteps);
		     
  virtual ~TRKThick(); 

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
  ///@}

private:
  TRKThick(); ///< not implemented
};


#endif
