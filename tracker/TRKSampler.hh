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
  TRKSampler(std::string nameIn);
  ~TRKSampler();

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKSampler& element);
  
private:
  TRKSampler(); ///< not implemented
};

#endif
