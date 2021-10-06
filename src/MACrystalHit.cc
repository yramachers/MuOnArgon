#include "MACrystalHit.hh"

G4ThreadLocal G4Allocator<MACrystalHit>* MACrystalHitAllocator=0;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool MACrystalHit::operator==(const MACrystalHit& right) const
{
  return ( this == &right ) ? true : false;
}

void MACrystalHit::Draw()
{ ; }

void MACrystalHit::Print()
{ ; }
