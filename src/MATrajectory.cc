#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4ParticleTypes.hh"
#include "G4Polyline.hh"
#include "G4UIcommand.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4VisAttributes.hh"

#include "MATrajectory.hh"

G4ThreadLocal G4Allocator<MATrajectory>* myTrajectoryAllocator = nullptr;

MATrajectory::MATrajectory(const G4Track* aTrack)
: G4VTrajectory()
, fPositionRecord{ new MATrajectoryPointContainer() }
, fTrackID{ aTrack->GetTrackID() }
, fParentID{ aTrack->GetParentID() }
, fParticleDefinition{ aTrack->GetDefinition() }
, fParticleName{ fParticleDefinition->GetParticleName() }
, fPDGEncoding{ fParticleDefinition->GetPDGEncoding() }
, fVertexName{ aTrack->GetLogicalVolumeAtVertex()->GetName() }
, fVertexPosition{ aTrack->GetVertexPosition() }
{
  fPositionRecord->push_back(new G4TrajectoryPoint(aTrack->GetPosition()));
}

MATrajectory::~MATrajectory()
{
  size_t i;
  for(i = 0; i < fPositionRecord->size(); i++)
  {
    delete(*fPositionRecord)[i];
  }
  fPositionRecord->clear();

  delete fPositionRecord;
}

void MATrajectory::ShowTrajectory(std::ostream& os) const
{
  os << G4endl << "TrackID =" << fTrackID << " : ParentID=" << fParentID << G4endl;

  os << "Particle name : " << fParticleName << "  PDG code : " << fPDGEncoding << G4endl;

  os << "Vertex : " << G4BestUnit(fVertexPosition, "Length") << "  in volume "
     << fVertexName << G4endl;

  os << "  Current trajectory has " << fPositionRecord->size() << " points." << G4endl;

  for(size_t i = 0; i < fPositionRecord->size(); i++)
  {
    G4TrajectoryPoint* aTrajectoryPoint = (G4TrajectoryPoint*) ((*fPositionRecord)[i]);
    os << "Point[" << i << "]"
       << " Position= " << aTrajectoryPoint->GetPosition() << G4endl;
  }
}

void MATrajectory::DrawTrajectory() const
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  G4ThreeVector  pos;

  G4Polyline pPolyline;
  for(size_t i = 0; i < fPositionRecord->size(); i++)
  {
    G4TrajectoryPoint* aTrajectoryPoint = (G4TrajectoryPoint*) ((*fPositionRecord)[i]);
    pos                                 = aTrajectoryPoint->GetPosition();
    pPolyline.push_back(pos);
  }

  G4Colour colour(0.2, 0.2, 0.2);
  if(fParticleDefinition == G4Gamma::GammaDefinition())
    colour = G4Colour(0., 0., 1.);
  else if(fParticleDefinition == G4Electron::ElectronDefinition() ||
          fParticleDefinition == G4Positron::PositronDefinition())
    colour = G4Colour(1., 1., 0.);
  else if(fParticleDefinition == G4MuonMinus::MuonMinusDefinition() ||
          fParticleDefinition == G4MuonPlus::MuonPlusDefinition())
    colour = G4Colour(0., 1., 0.);
  else if(fParticleDefinition->GetParticleType() == "meson")
  {
    colour = G4Colour(1., 0., 0.);
  }
  else if(fParticleDefinition->GetParticleType() == "baryon")
  {
    colour = G4Colour(0., 1., 1.);
  }

  G4VisAttributes attribs(colour);
  pPolyline.SetVisAttributes(attribs);
  if(pVVisManager)
    pVVisManager->Draw(pPolyline);
}

void MATrajectory::AppendStep(const G4Step* aStep)
{
  fPositionRecord->push_back(
    new G4TrajectoryPoint(aStep->GetPostStepPoint()->GetPosition()));
}

void MATrajectory::MergeTrajectory(G4VTrajectory* secondTrajectory)
{
  if(!secondTrajectory)
    return;

  MATrajectory*   seco = (MATrajectory*) secondTrajectory;
  G4int           ent  = seco->GetPointEntries();
  //
  // initial point of the second trajectory should not be merged
  for(int i = 1; i < ent; i++)
  {
    fPositionRecord->push_back((*(seco->fPositionRecord))[i]);
  }
  delete(*seco->fPositionRecord)[0];
  seco->fPositionRecord->clear();
}
