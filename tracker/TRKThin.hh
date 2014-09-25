#ifndef TRKThin_h
#define TRKThin_h

#include "TRKStrategy.hh"
#include "TRKElement.hh"

class TRKBunch;
class TRKDipole;
class TRKQuadrupole;
class TRKSextupole;
class TRKOctupole;

/**
 * @brief Thin tracking
 */
class TRKThin : public TRKStrategy {
public:

  TRKThin(int trackingSteps);
		     
  virtual ~TRKThin(); 

  /// returns tracking type
  //  TRKType trackingType()const {return type;}

  /**
   * Tracks a bunch through an element
   * @param[in]  el    The TRKElement.
   * @param[in]  bunch The bunch.
   */
  ///@{
  virtual void Track(TRKDrift* el, TRKBunch* bunch);
  virtual void Track(TRKDipole* el, TRKBunch* bunch);
  virtual void Track(TRKQuadrupole* el, TRKBunch* bunch);
  virtual void Track(TRKSextupole* el, TRKBunch* bunch);
  virtual void Track(TRKOctupole* el, TRKBunch* bunch);
  ///@}

protected:
  /// number of tracking steps
  int trackingSteps;

private:
  TRKThin(); ///< not implemented
};


#endif
