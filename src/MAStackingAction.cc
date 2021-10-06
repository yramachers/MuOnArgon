#include "MAStackingAction.hh"

G4ClassificationOfNewTrack MAStackingAction ::ClassifyNewTrack(const G4Track* aTrack)
{
  G4ClassificationOfNewTrack classification = fUrgent;

  return classification;
}

void MAStackingAction::NewStage() { ; }

void MAStackingAction::PrepareNewEvent() { ; }
