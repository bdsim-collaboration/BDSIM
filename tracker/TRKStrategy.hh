#ifndef TRKStrategy_h
#define TRKStrategy_h 1

#include "TRKElement.hh"

class TRKBunch;
class TRKDrift;
class TRKBend;
class TRKDipole;
class TRKQuadrupole;
class TRKSextupole;
class TRKOctupole;
class TRKSampler;

/**
 * @brief virtual base class for a track strategy
 */
class TRKStrategy {
public : 

  TRKStrategy(int trackingSteps);
		     
  virtual ~TRKStrategy(); 

  /// returns tracking type
  //  TRKType trackingType()const {return type;}

  /**
   * Tracks a bunch through an element
   * @param[in]  el    The TRKElement.
   * @param[in]  bunch The bunch.
   */
  ///@{
  void         Track(TRKElement*    el, TRKBunch* bunch);
  virtual void Track(TRKDrift*      el, TRKBunch* bunch)=0;
  virtual void Track(TRKBend*       el, TRKBunch* bunch)=0;
  virtual void Track(TRKDipole*     el, TRKBunch* bunch)=0;
  virtual void Track(TRKQuadrupole* el, TRKBunch* bunch)=0;
  virtual void Track(TRKSextupole*  el, TRKBunch* bunch)=0;
  virtual void Track(TRKOctupole*   el, TRKBunch* bunch)=0;
  void         Track(TRKSampler*    el, TRKBunch* bunch);
  ///@}

protected:
  /// number of tracking steps
  int trackingSteps;

private:
  TRKStrategy(); ///< not implemented
};


#endif
