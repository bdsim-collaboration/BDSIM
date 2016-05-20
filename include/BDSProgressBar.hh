#ifndef BDS_PROGRESS_BAR_Hh
#define BDS_PROGRESS_BAR_Hh

#include <iostream>           // for ostream, cout, etc
#include <iomanip>
#include <string>             // for string
#include "BDSGlobalConstants.hh"

class BDSProgressBar
{
public:
  BDSProgressBar( double maxCount );
  void reset();
  inline double count() const { return _count; }
  inline double maxCount() const { return _maxCount; }

  inline void increment(double val){
    _count += val;
    if(_count > _nextTicmarkCount){
      display();
    }
  }


  private:
  double _count, _maxCount, _nextTicmarkCount;
  double _ticmark;
  double _nTicmarksInBar;
  double _countsPerTicmark;

  inline void display()  {
    _ticmark=0;
    double ticmarksNeeded = _nTicmarksInBar*_count/_maxCount;
    _nextTicmarkCount = (ticmarksNeeded + 1)*_countsPerTicmark;
    std::cout << "]\n\033[F\033[J" << "0%" << "[";
    do { std::cout << '-' ;} while ( ++_ticmark < ticmarksNeeded );
    if(_ticmark < _nTicmarksInBar){
      std::cout << '>';      
      while ( ++_ticmark < _nTicmarksInBar ){ std::cout << ' '; } 
    } 
    std::cout << "]100%" << std::flush;
  } // display
};

#endif  // BDS_PROGRESS_BAR_Hh
