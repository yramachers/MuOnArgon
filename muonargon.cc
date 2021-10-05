// ********************************************************************
// muonargon project

// standard
#include <algorithm>
#include <string>

// Geant4
#include "G4Types.hh"

#ifdef G4MULTITHREADED
#  include "G4MTRunManager.hh"
#else
#  include "G4RunManager.hh"
#endif

#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"

#include "G4NeutronTrackingCut.hh"
#include "G4Threading.hh"
#include "G4UImanager.hh"
#include "Shielding.hh"

// us
#include "CLI11.hpp"  // c++17 safe; https://github.com/CLIUtils/CLI11
#include "MAActionInitialization.hh"
#include "MADetectorConstruction.hh"

int main(int argc, char** argv)
{
  // command line interface
  CLI::App    app{ "Muon on Argon Simulation" };
  int         nthreads = 4;
  std::string outputFileName("ma.root");
  std::string macroName;

  app.add_option("-m,--macro", macroName, "<Geant4 macro filename> Default: None");
  app.add_option("-o,--outputFile", outputFileName,
                 "<FULL PATH ROOT FILENAME> Default: ma.root");
  app.add_option("-t, --nthreads", nthreads, "<number of threads to use> Default: 4");

  CLI11_PARSE(app, argc, argv);

  // GEANT4 code
  // If we're in interactive mode (no macroName), create a UIexecutive
  // Constructing it here ensures all G4cout/cerr is delivered to it, so in GUI
  // modes we don't get a mix of terminal/GUI info
  G4UIExecutive* ui = nullptr;
  if(macroName.empty())
  {
    ui = new G4UIExecutive(argc, argv);
  }

  // -- Construct the run manager : MT or sequential one
#ifdef G4MULTITHREADED
  nthreads =
    std::min(nthreads, G4Threading::G4GetNumberOfCores());  // limit thread number to
                                                            // max on machine

  auto runManager = std::make_unique<G4MTRunManager>();
  G4cout << "      ********* Run Manager constructed in MT mode: " << nthreads
         << " threads ***** " << G4endl;
  runManager->SetNumberOfThreads(nthreads);

#else

  auto runManager = std::make_unique<G4RunManager>();
  G4cout << "      ********** Run Manager constructed in sequential mode ************ "
         << G4endl;

#endif

  // -- Set mandatory initialization classes
  auto* detector = new MADetectorConstruction;
  runManager->SetUserInitialization(detector);

  // -- set user physics list
  auto* physicsList = new Shielding;
  // allow for thermal neutrons to find Ge
  auto* neutronCut  = new G4NeutronTrackingCut(1);
  neutronCut->SetTimeLimit(2.0 * CLHEP::ms);  // 2 milli sec limit
  physicsList->RegisterPhysics(neutronCut);   // like in Gerda paper

  // finish physics list
  runManager->SetUserInitialization(physicsList);

  // -- Set user action initialization class, forward random seed
  auto* actions = new MAActionInitialization(detector, outputFileName);
  runManager->SetUserInitialization(actions);

  // Initialize G4 kernel
  //
  // runManager->Initialize();

  // Visualization manager
  //
  // auto visManager = std::make_unique<G4VisExecutive>();
  // visManager->Initialize();

  // Get the pointer to the User Interface manager
  //
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  if(ui != nullptr)  // Define UI session for interactive mode
  {
    auto visManager = std::make_unique<G4VisExecutive>();
    visManager->Initialize();

    UImanager->ApplyCommand("/control/execute vis.mac");
    ui->SessionStart();
    // UI must be deleted *before* the vis manager
    delete ui;
  }
  else  // Batch mode
  {
    G4String command = "/control/execute ";
    UImanager->ApplyCommand(command + macroName);
  }

  return 0;
}
