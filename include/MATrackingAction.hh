#ifndef MATrackingAction_h
#define MATrackingAction_h 1

#include "G4UserTrackingAction.hh"

class MATrackingAction : public G4UserTrackingAction
{
public:
  MATrackingAction();
  virtual ~MATrackingAction(){};

  virtual void PreUserTrackingAction(const G4Track*);
  virtual void PostUserTrackingAction(const G4Track*);
};

#endif
