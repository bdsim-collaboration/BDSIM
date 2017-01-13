#ifndef BDSPROGRESSBAR_H
#define BDSPROGRESSBAR_H

#include "BDSGlobalConstants.hh"

#include <iostream>
#include <iomanip>
#include <string>

/**
 * @brief Display a progress bar in ascii.
 *
 * @author Lawrence Deacon
 */

class BDSProgressBar
{
public:
  BDSProgressBar(double maxCountIn);

  void Reset();
  inline double Count()    const {return count;}
  inline double MaxCount() const {return maxCount;}

  inline void Increment(double val)
  {
    count += val;
    if(count > nextTicMarkCount)
      {Display();}
  }

private:
  /// Private default constructor to force use of supplied one.
  BDSProgressBar() = delete;
  
  double count, maxCount, nextTicMarkCount;
  double ticMark;
  double nTicMarksInBar;
  double countsPerTicMark;

  inline void Display()
  {
    ticMark = 0;
    double ticMarksNeeded = nTicMarksInBar * count/maxCount;
    nextTicMarkCount = (ticMarksNeeded + 1) * countsPerTicMark;
    std::cout << "]\n\033[F\033[J" << "0%" << "[";
    
    do {std::cout << '-' ;}
    while ( ++ticMark < ticMarksNeeded );
    if(ticMark < nTicMarksInBar)
      {
	std::cout << '>';      
	while (++ticMark < nTicMarksInBar)
	  {std::cout << ' ';} 
      }
    std::cout << "]100%" << std::flush;
  }
};

#endif
