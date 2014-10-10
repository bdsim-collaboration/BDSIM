#ifndef TRKSampler_h
#define TRKSampler_h

#include "TRKElement.hh"
#include "TRKStrategy.hh"
#include "TRKBunch.hh"

/**
 * @brief sampler class - output from tracker
 */
class TRKSampler : public TRKElement {
public:
  TRKSampler(std::string name);
  ~TRKSampler();

  friend std::ostream& operator<< (std::ostream &out, const TRKSampler& element);

  void Track(TRKBunch* bunch, TRKStrategy* strategy);
  void CheckAperture(TRKBunch* bunch);

private:
  TRKSampler(); ///< not implemented
};

#endif
