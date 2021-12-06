#ifndef MALiquidHit_h
#define MALiquidHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

/// Liquid hit class
///
/// It defines data members to store the energy deposit,
/// and position in a selected volume:

class MALiquidHit : public G4VHit
{
  public:
    G4bool operator==(const MALiquidHit&) const;

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    // methods from base class
    virtual void Draw();
    virtual void Print();

    // Set methods
    void SetTID      (G4int   tid)      { fTid    = tid; };
    void SetIonZ     (G4int   tz)       { fZ      = tz; };
    void SetIonA     (G4int   ta)       { fA      = ta; };
    void SetTime     (G4double ti)      { fTime   = ti; };
    void SetEdep     (G4double de)      { fEdep   = de; };
    void SetPos      (G4ThreeVector xyz){ fPos    = xyz; };

    // Get methods
    G4int    GetTID()  const     { return fTid; };
    G4int    GetIonZ()  const    { return fZ; };
    G4int    GetIonA()  const    { return fA; };
    G4double GetTime() const     { return fTime; };
    G4double GetEdep() const     { return fEdep; };
    G4ThreeVector GetPos() const { return fPos; };

  private:
      G4int         fTid = 0;
      G4int         fZ   = 0;
      G4int         fA   = 0;
      G4double      fTime = 0.0 ;
      G4double      fEdep = 0.0;
      G4ThreeVector fPos = G4ThreeVector{};
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<MALiquidHit> MALiquidHitsCollection;

extern G4ThreadLocal G4Allocator<MALiquidHit>* MALiquidHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* MALiquidHit::operator new(size_t)
{
  if(!MALiquidHitAllocator)
      MALiquidHitAllocator = new G4Allocator<MALiquidHit>;
  return (void *) MALiquidHitAllocator->MallocSingle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void MALiquidHit::operator delete(void *hit)
{
  MALiquidHitAllocator->FreeSingle((MALiquidHit*) hit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
