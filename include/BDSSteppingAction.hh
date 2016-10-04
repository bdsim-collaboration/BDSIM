#ifndef BDSSTEPPINGACTION_H
#define BDSSTEPPINGACTION_H

#include "G4UserSteppingAction.hh"
#include "G4Types.hh"


class BDSSteppingAction : public G4UserSteppingAction
{
public:
  BDSSteppingAction();
  virtual ~BDSSteppingAction();

  virtual void UserSteppingAction(const G4Step*);

private:
  void VerboseSteppingAction();
  const G4Step* _step;
};

#endif
