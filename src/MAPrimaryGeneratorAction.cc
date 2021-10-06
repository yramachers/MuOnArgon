// us
#include "MAPrimaryGeneratorAction.hh"
#include "MADetectorConstruction.hh"

// geant
#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

MAPrimaryGeneratorAction::MAPrimaryGeneratorAction(MADetectorConstruction* det)
: G4VUserPrimaryGeneratorAction()
, fDetector(det)
, fParticleGun(nullptr)
, fMessenger(nullptr)
, fDepth(0.0)
{
  generator.seed(rd());  // set a random seed

  G4int nofParticles = 1;
  fParticleGun       = new G4ParticleGun(nofParticles);

  auto particleTable = G4ParticleTable::GetParticleTable();

  // default particle kinematics
  fParticleGun->SetParticleDefinition(particleTable->FindParticle("mu-"));

  // define commands for this class
  DefineCommands();
}

MAPrimaryGeneratorAction::~MAPrimaryGeneratorAction()
{
  delete fParticleGun;
  delete fMessenger;
}

void MAPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  using pld_type = std::piecewise_linear_distribution<double>;

  int    nw             = 100;     // number of bins
  double lower_bound    = 1.0;     // energy interval lower bound [GeV]
  double upper_bound    = 3000.0;  // upper bound [GeV]
  double nearhorizontal = 1.0e-5;
  double fullcosangle   = 1.0;

  // custom probability distributions
  pld_type ed(nw, lower_bound, upper_bound, MuEnergy(fDepth));
  pld_type cosd(nw, nearhorizontal, fullcosangle, MuAngle(fDepth));

  // momentum vector
  G4double costheta = cosd(generator);  // get a random number
  G4double sintheta = std::sqrt(1. - costheta * costheta);

  std::uniform_real_distribution<> rndm(0.0, 1.0);   // azimuth angle
  G4double phi    = CLHEP::twopi * rndm(generator);  // random uniform number
  G4double sinphi = std::sin(phi);
  G4double cosphi = std::cos(phi);

  G4double      px = -sintheta * cosphi;
  G4double      py = -sintheta * sinphi;
  G4double      pz = -costheta;  // default downwards: pz = -1.0
  G4ThreeVector momentumDir(px, py, pz);
  fParticleGun->SetParticleMomentumDirection(momentumDir);
  // G4cout << "Momentum direction Primary: " << momentumDir << G4endl;

  G4double ekin = ed(generator);  // get random number
  ekin *= GeV;
  fParticleGun->SetParticleEnergy(ekin);

  // position, top of world, sample circle uniformly
  G4double zvertex = fDetector->GetWorldSizeZ();  // inline on MADetectorConstruction
  G4double radius  = fDetector->GetWorldExtent() * rndm(generator);  // fraction of max
  phi              = CLHEP::twopi * rndm(generator);  // another random angle
  G4double vx      = radius * std::cos(phi);
  G4double vy      = radius * std::sin(phi);

  fParticleGun->SetParticlePosition(G4ThreeVector(vx, vy, zvertex - 1.0 * cm));

  fParticleGun->GeneratePrimaryVertex(event);
}

void MAPrimaryGeneratorAction::DefineCommands()
{
  // Define /MA/generator command directory using generic messenger class
  fMessenger =
    new G4GenericMessenger(this, "/MA/generator/", "Primary generator control");

  // depth command
  auto& depthCmd = fMessenger->DeclareProperty("depth", fDepth,
                                               "Underground laboratory depth [km.w.e.].");
  depthCmd.SetParameterName("d", true);
  depthCmd.SetRange("d>=0.");
  depthCmd.SetDefaultValue("0.");
}
