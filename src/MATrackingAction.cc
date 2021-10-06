#include "MATrackingAction.hh"
#include "MATrajectory.hh"

#include "G4Track.hh"
#include "G4TrackingManager.hh"

MATrackingAction::MATrackingAction() = default;

void MATrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
  // Create trajectory for track if requested
  if(fpTrackingManager->GetStoreTrajectory() > 0)
  {
    fpTrackingManager->SetTrajectory(new MATrajectory(aTrack));
  }
}

void MATrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
}
