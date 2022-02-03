#ifndef MARunAction_h
#define MARunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

/// Run action class
///

class MARunAction : public G4UserRunAction
{
public:
  MARunAction(G4String name);
  virtual ~MARunAction();

  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

private:
  G4String         fout;          // output file name
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
