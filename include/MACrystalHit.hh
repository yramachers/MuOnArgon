#ifndef MACrystalHit_h
#define MACrystalHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

/// Crystal hit class
///
/// It defines data members to store the energy deposit,
/// and position in a selected volume:

class MACrystalHit : public G4VHit
{
  public:
    G4bool operator==(const MACrystalHit&) const;

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    // methods from base class
    virtual void Draw();
    virtual void Print();

    // Set methods
    void SetTID      (G4int   tid)      { fTid    = tid; };
    void SetTime     (G4double ti)      { fTime   = ti; };
    void SetWeight   (G4double we)      { fWeight = we; };
    void SetEdep     (G4double de)      { fEdep   = de; };
    void SetPos      (G4ThreeVector xyz){ fPos    = xyz; };

    // Get methods
    G4int    GetTID()  const     { return fTid; };
    G4double GetTime() const     { return fTime; };
    G4double GetWeight() const   { return fWeight; };
    G4double GetEdep() const     { return fEdep; };
    G4ThreeVector GetPos() const { return fPos; };

  private:
      G4int         fTid = 0;
      G4double      fTime = 0.0 ;
      G4double      fWeight = 0.0;
      G4double      fEdep = 0.0;
      G4ThreeVector fPos = G4ThreeVector{};
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<MACrystalHit> MACrystalHitsCollection;

extern G4ThreadLocal G4Allocator<MACrystalHit>* MACrystalHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* MACrystalHit::operator new(size_t)
{
  if(!MACrystalHitAllocator)
      MACrystalHitAllocator = new G4Allocator<MACrystalHit>;
  return (void *) MACrystalHitAllocator->MallocSingle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void MACrystalHit::operator delete(void *hit)
{
  MACrystalHitAllocator->FreeSingle((MACrystalHit*) hit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
