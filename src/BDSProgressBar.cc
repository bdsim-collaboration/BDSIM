#include "BDSProgressBar.hh"

BDSProgressBar::BDSProgressBar( double maxCount ) {
  _maxCount = maxCount;
  _nTicmarksInBar=50;
  _countsPerTicmark=_maxCount/_nTicmarksInBar;
  reset();
}

void BDSProgressBar::reset() {
  _ticmark = 0;
  _nextTicmarkCount = 0;  
  _count = 0;

  std::cout << std::endl;
  if ( !_maxCount ) _maxCount = 1;  
}

