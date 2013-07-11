#ifndef __BDSTRACKINGFIFO_H_
#define __BDSTRACKINGFIFO_H_

#include <string>
#include "globals.hh"

//A class for file input/output
class BDSTrackingFIFO {
public:
  BDSTrackingFIFO();
  ~BDSTrackingFIFO();
  void doFifo();
private:
  void writeToFifo();
  void readFromFifo();
  void initWrite();
  void initRead();
  void finishWrite();
  void finishRead();
  void openForWriting();
  void openForReading();
  void open(const char* mode);
  void close();
  std::string _filename;
  FILE* _fifo;
  G4double _tmpT;
};

#endif
