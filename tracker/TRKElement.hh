#ifndef TRKElement_h
#define TRKElement_h

#include <string>
#include <iostream>

#include "TRKAperture.hh"
#include "TRKPlacement.hh"

class TRKBunch;
class TRKStrategy;

/**
 * @brief a basic element
 */
class TRKElement { 
public :
  TRKElement(std::string name, double length, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKElement();

  /// track method, visitor pattern
  virtual void Track(TRKBunch* bunch, TRKStrategy* strategy);

  std::string GetName()const {return name;}
  double GetLength()const {return length;}
  // should made const
  TRKAperture* GetAperture()const {return aperture;}
  
  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKElement &element);

protected : 
  std::string  name;               ///< name of element -- do we need this? JS
  double       length;             ///< length of component [m]
  TRKAperture  *aperture;          ///< aperture of element
  TRKPlacement *placement;         ///< location of element

private :
  TRKElement(); ///< not implemented
  
  /// global coordinates of local point
  double* LocalToGlobal(double /*vOut*/[]){return NULL;}
};

// declare drift
typedef TRKElement TRKDrift;

#endif
