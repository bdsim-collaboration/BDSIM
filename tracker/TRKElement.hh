#ifndef TRKElement_h
#define TRKElement_h

#include <string>
#include <iostream>

#include "TRKAperture.hh"
#include "TRKPlacement.hh"

class TRKBunch;
class TRKStrategy;
class TRKTiltOffset;

/**
 * @brief Base class element, also used for Drift
 */
class TRKElement { 
public :
  TRKElement(std::string name, double length, TRKAperture *aperture, TRKPlacement *placement);
  ~TRKElement();

  /// track method, visitor pattern
  virtual void Track(TRKBunch* bunch, TRKStrategy* strategy);
  void CheckAperture(TRKBunch* bunch);

  std::string  GetName()const {return name;}
  double       GetLength()const {return length;}
  TRKAperture* GetAperture()const {return aperture;}
  
  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKElement &element);

protected : 
  std::string    name;              ///< name of element -- do we need this? JS
  double         length;            ///< length of component [m]
  TRKAperture*   aperture;          ///< aperture of element
  TRKPlacement*  placement;         ///< location of element
  TRKTiltOffset* offsetIn;          ///< tilt and offset entrance of element
  TRKTiltOffset* offsetOut;         ///< tilt and offset exit of element

  /// virtual print method for overloading operator<<. Virtual Friend Function Idiom
  virtual void Print(std::ostream& out) const;

private :
  TRKElement(); ///< not implemented
  
  /// global coordinates of local point - perhaps not needed
  double* LocalToGlobal(double /*vOut*/[]){return NULL;}
};

// declare drift
typedef TRKElement TRKDrift;

inline std::ostream& operator<< (std::ostream &out, const TRKElement &element)
{
  element.Print(out);
  return out;
}

#endif
