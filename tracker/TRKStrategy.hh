#ifndef TRKStrategy_h
#define TRKStrategy_h 1

#include "TRKElement.hh"

class TRKBunch;
//class TRKDrift;
class TRKSBend;
class TRKRBend;
class TRKDipole;
class TRKQuadrupole;
class TRKSextupole;
class TRKOctupole;
class TRKSolenoid;
class TRKSampler;
class TRKTiltOffset;

/**
 * @brief virtual base class for a track strategy
 */
class TRKStrategy
{
public: 
  TRKStrategy(int trackingSteps);

  void SetMomentumAndEnergy(double nominalMomentumIn,
			    double nominalEnergyIn);
  
  virtual ~TRKStrategy(); 

  /// returns tracking type
  //  TRKType trackingType()const {return type;}

  /**
   * Tracks a bunch through an element
   * @param[in]  el    The TRKElement.
   * @param[in]  bunch The bunch.
   */
  ///@{
  //void         Track(TRKElement*    el, TRKBunch* bunch);
  virtual void Track(TRKDrift*      el, TRKBunch* bunch)=0;
  virtual void Track(TRKSBend*      el, TRKBunch* bunch)=0;
  virtual void Track(TRKRBend*      el, TRKBunch* bunch)=0;
  virtual void Track(TRKDipole*     el, TRKBunch* bunch)=0;
  virtual void Track(TRKQuadrupole* el, TRKBunch* bunch)=0;
  virtual void Track(TRKSextupole*  el, TRKBunch* bunch)=0;
  virtual void Track(TRKOctupole*   el, TRKBunch* bunch)=0;
  virtual void Track(TRKSolenoid*   el, TRKBunch* bunch)=0;
  void         Track(TRKTiltOffset* el, TRKBunch* bunch);
  ///@}

protected:
  /// number of tracking steps
  int trackingSteps;

  double nominalMomentum;
  double nominalEnergy;

private:
  TRKStrategy() = delete; ///< not implemented
};


#endif
