#ifndef BDSOutputNone_h
#define BDSOutputNone_h 

#include "BDSOutput.hh"

/**
 * @brief No output class
 * 
 * Write no output
 *
 * @author Jochem Snuverink
 */

class BDSOutputNone: public BDSOutput
{
public: 
  BDSOutputNone(): BDSOutput("", -1){;} ///< Default constructor.
  virtual ~BDSOutputNone(){;}

  /// @{ No action.
  virtual void NewFile(){;}
  virtual void CloseFile(){;}
private:
  virtual void WriteOptions(){;}
  virtual void WriteModel(){;}
  virtual void WriteFileEventLevel(){;}
  virtual void WriteFileRunLevel(){;}
  /// @}
};

#endif
