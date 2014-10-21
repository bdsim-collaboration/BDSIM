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

  virtual void Track(TRKBunch* bunch, TRKStrategy* strategy);

protected:
  /// output stream
  virtual void Print(std::ostream& out) const;
  
private:
  TRKSampler(); ///< not implemented
};

#endif
