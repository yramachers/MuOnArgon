#ifndef MAPrimaryGeneratorAction_h
#define MAPrimaryGeneratorAction_h 1

// std c++ includes
#include <cmath>
#include <random>

#include "G4GenericMessenger.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;
class G4ParticleDefinition;
class MADetectorConstruction;

/// Primary generator
///
/// A single particle is generated.
// The G4GenericMessenger is used for simple UI
/// User can select
/// - the underground laboratory depth in [km.w.e.]

// muon distribution functors, energy and
// angle relative to z-axis, i.e. third component of G4ThreeVector
class MuEnergy
{
  // data members
private:
  double bpar;     // fixed parameter; Mei, Hime, Preprint astro-ph/0512125, Eq.8
  double gammaMu;  // "
  double epsMu;    // "
  double depth;    // laboratory depth [km.w.e.] to be set

public:
  MuEnergy(double d)
  : bpar(0.4)
  , gammaMu(3.77)
  , epsMu(693.0)
  , depth(d)
  {}  // default constructor, fix parameter values
  ~MuEnergy() {}

  double operator()(double x)
  {  // energy distribution function
    double dummy  = (x + epsMu * (1.0 - std::exp(-bpar * depth)));
    double result = std::exp(-bpar * depth * (gammaMu - 1.0)) * std::pow(dummy, -gammaMu);
    return result;
  }
};

class MuAngle
{
  // data members
private:
  double i1, i2, L1,
    L2;          // fixed parameter; Mei, Hime, Preprint astro-ph/0512125, Eq.3/4
  double depth;  // laboratory depth [km.w.e.] to be set

public:
  MuAngle(double d)
  : i1(8.6e-6)
  , i2(0.44e-6)
  , L1(0.45)
  , L2(0.87)
  , depth(d)
  {}  // default constructor, fix parameter values
  ~MuAngle() {}

  double operator()(double x)
  {  // cos(theta) distribution function
    double costheta = x;
    double sec      = 1.0e5;  // inverse smallest cos theta
    if(costheta > 1.0e-5)
      sec = 1.0 / costheta;  // exclude horizontal costheta = 0
    double dummy  = depth * sec / L1;
    double dummy2 = depth * sec / L2;
    double result = (i1 * std::exp(-dummy) + i2 * std::exp(-dummy2)) * sec;
    return result;
  }
};

class MAPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  MAPrimaryGeneratorAction(MADetectorConstruction* det);
  virtual ~MAPrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event*);

  void     SetDepth(G4double val) { fDepth = val; }
  G4double GetDepth() const { return fDepth; }

private:
  void DefineCommands();

  MADetectorConstruction* fDetector;

  G4ParticleGun*      fParticleGun;
  G4GenericMessenger* fMessenger;

  std::random_device rd;
  std::ranlux24      generator;
  G4double           fDepth;
};

#endif
