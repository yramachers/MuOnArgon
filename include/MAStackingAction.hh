#ifndef MAStackingAction_H
#define MAStackingAction_H 1

#include "G4Track.hh"
#include "G4UserStackingAction.hh"
#include "globals.hh"

class MAStackingAction : public G4UserStackingAction
{
public:
  MAStackingAction()          = default;
  virtual ~MAStackingAction() = default;

public:
  virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
  virtual void                       NewStage();
  virtual void                       PrepareNewEvent();

private:
};

#endif
