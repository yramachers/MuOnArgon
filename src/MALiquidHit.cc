#include "MALiquidHit.hh"

G4ThreadLocal G4Allocator<MALiquidHit>* MALiquidHitAllocator=0;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool MALiquidHit::operator==(const MALiquidHit& right) const
{
  return ( this == &right ) ? true : false;
}

void MALiquidHit::Draw()
{ ; }

void MALiquidHit::Print()
{ ; }
