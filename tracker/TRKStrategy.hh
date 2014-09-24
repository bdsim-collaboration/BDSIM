#ifndef TRKStrategy_h
#define TRKStrategy_h

#include "TRK.hh"
#include "TRKElement.hh"

class TRKBunch;
class TRKDipole;
class TRKQuadrupole;
class TRKSextupole;
class TRKOctupole;

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
  virtual void Track(TRKDrift* el, TRKBunch* bunch)=0;
  virtual void Track(TRKDipole* el, TRKBunch* bunch)=0;
  virtual void Track(TRKQuadrupole* el, TRKBunch* bunch)=0;
  virtual void Track(TRKSextupole* el, TRKBunch* bunch)=0;
  virtual void Track(TRKOctupole* el, TRKBunch* bunch)=0;
  ///@}

protected:
  /// number of tracking steps
  int trackingSteps;

private:
  TRKStrategy(); ///< not implemented
};


#endif
