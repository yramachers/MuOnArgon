#ifndef MATrajectory_h
#define MATrajectory_h 1

#include "G4Allocator.hh"
#include "G4ParticleDefinition.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4TrajectoryPoint.hh"
#include "G4VTrajectory.hh"
#include "G4ios.hh"
#include "globals.hh"
#include <stdlib.h>
#include <vector>

class G4Polyline;

class MATrajectory : public G4VTrajectory
{
public:
  using MATrajectoryPointContainer = std::vector<G4VTrajectoryPoint*>;

  MATrajectory(const G4Track* aTrack);
  virtual ~MATrajectory();

  virtual void ShowTrajectory(std::ostream& os = G4cout) const;
  virtual void DrawTrajectory() const;
  virtual void AppendStep(const G4Step* aStep);
  virtual void MergeTrajectory(G4VTrajectory* secondTrajectory);

  inline void* operator new(size_t);
  inline void  operator delete(void*);
  inline int   operator==(const MATrajectory& right) const { return (this == &right); }

  // required from base class
  virtual G4double      GetCharge() const { return 0.0; };
  virtual G4ThreeVector GetInitialMomentum() const { return G4ThreeVector(); }

  // required for ntuple storage
  virtual G4int               GetTrackID() const { return fTrackID; }
  virtual G4int               GetParentID() const { return fParentID; }
  virtual G4String            GetParticleName() const { return fParticleName; }
  virtual G4String            GetVertexName() const { return fVertexName; }
  virtual G4int               GetPDGEncoding() const { return fPDGEncoding; }
  virtual G4ThreeVector       GetVertex() const { return fVertexPosition; }
  virtual int                 GetPointEntries() const { return fPositionRecord->size(); }
  virtual G4VTrajectoryPoint* GetPoint(G4int i) const { return (*fPositionRecord)[i]; }

  G4ParticleDefinition* GetParticleDefinition() const { return fParticleDefinition; }

private:
  MATrajectoryPointContainer*   fPositionRecord;
  G4int                         fTrackID;
  G4int                         fParentID;
  G4ParticleDefinition*         fParticleDefinition;
  G4String                      fParticleName;
  G4String                      fVertexName;
  G4int                         fPDGEncoding;
  G4ThreeVector                 fVertexPosition;
};

extern G4ThreadLocal G4Allocator<MATrajectory>* myTrajectoryAllocator;

inline void* MATrajectory::operator new(size_t)
{
  if(myTrajectoryAllocator == nullptr)
  {
    myTrajectoryAllocator = new G4Allocator<MATrajectory>;
  }
  return (void*) myTrajectoryAllocator->MallocSingle();
}

inline void MATrajectory::operator delete(void* aTrajectory)
{
  myTrajectoryAllocator->FreeSingle((MATrajectory*) aTrajectory);
}

#endif
