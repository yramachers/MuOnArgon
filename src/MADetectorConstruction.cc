#include "MADetectorConstruction.hh"

#include <cmath>

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Polyhedra.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"

#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include "G4SDManager.hh"
#include "MALiquidSD.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

MADetectorConstruction::MADetectorConstruction()
{
  DefineMaterials();
}

MADetectorConstruction::~MADetectorConstruction()
{
}

auto MADetectorConstruction::Construct() -> G4VPhysicalVolume*
{
  return SetupCryostat();
}

void MADetectorConstruction::DefineMaterials()
{
  G4NistManager* nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_Galactic");
  nistManager->FindOrBuildMaterial("G4_lAr");
  nistManager->FindOrBuildMaterial("G4_AIR");
  nistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL");
  nistManager->FindOrBuildMaterial("G4_Cu");
  nistManager->FindOrBuildMaterial("G4_Gd");

  auto* C  = new G4Element("Carbon", "C", 6., 12.011 * g / mole);
  auto* O  = new G4Element("Oxygen", "O", 8., 16.00 * g / mole);
  auto* Ca = new G4Element("Calcium", "Ca", 20., 40.08 * g / mole);
  auto* Mg = new G4Element("Magnesium", "Mg", 12., 24.31 * g / mole);

  // Standard Rock definition, similar to Gran Sasso rock
  // with density from PDG report
  auto* stdRock = new G4Material("StdRock", 2.65 * g / cm3, 4);
  stdRock->AddElement(O, 52.0 * perCent);
  stdRock->AddElement(Ca, 27.0 * perCent);
  stdRock->AddElement(C, 12.0 * perCent);
  stdRock->AddElement(Mg, 9.0 * perCent);

  auto* H     = new G4Element("Hydrogen", "H", 1., 1.00794 * g / mole);
  auto* N     = new G4Element("Nitrogen", "N", 7., 14.00 * g / mole);
  auto* puMat = new G4Material("polyurethane", 0.3 * g / cm3, 4);  // high density foam
  puMat->AddElement(H, 16);
  puMat->AddElement(O, 2);
  puMat->AddElement(C, 8);
  puMat->AddElement(N, 2);

  // PMMA and PMMA with Gd
  G4double density = 1.18 * g / cm3;
  auto*    pmma    = new G4Material("PMMA", density, 3);
  pmma->AddElement(H, 8);
  pmma->AddElement(C, 5);
  pmma->AddElement(O, 2);

  auto*    pmmagd  = new G4Material("PMMAGd", density, 3);
  pmmagd->AddElement(H, 8);
  pmmagd->AddElement(C, 5);
  pmmagd->AddElement(O, 2);
  auto* gadMat     = G4Material::GetMaterial("G4_Gd");
  pmmagd->AddMaterial(gadMat, 1.0 * perCent);

}

void MADetectorConstruction::ConstructSDandField()
{
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  // Only need to construct the (per-thread) SD once
  if(!fSD.Get())
  {
    G4String liquidSDname  = "LiquidSD";
    MALiquidSD* aliquidSD = new MALiquidSD(liquidSDname,
                                                 "LiquidHitsCollection");
    fSD.Put(aliquidSD);

    // Also only add it once to the SD manager!
    G4SDManager::GetSDMpointer()->AddNewDetector(fSD.Get());

    SetSensitiveDetector("TPC_log", fSD.Get());
    SetSensitiveDetector("IB_log",  fSD.Get());
    SetSensitiveDetector("OB_log",  fSD.Get());

  }
  else
  {
    G4cout << " >>> fSD has entry. Repeated call." << G4endl;
  }
}

auto MADetectorConstruction::SetupCryostat() -> G4VPhysicalVolume*
{
  // Get materials
  auto* worldMaterial = G4Material::GetMaterial("G4_Galactic");
  auto* larMat        = G4Material::GetMaterial("G4_lAr");
  auto* airMat        = G4Material::GetMaterial("G4_AIR");
  auto* steelMat      = G4Material::GetMaterial("G4_STAINLESS-STEEL");
  auto* copperMat     = G4Material::GetMaterial("G4_Cu");
  auto* stdRock       = G4Material::GetMaterial("StdRock");
  auto* puMat         = G4Material::GetMaterial("polyurethane");
  auto* pmmaMat       = G4Material::GetMaterial("pmma");
  auto* pmmagdMat     = G4Material::GetMaterial("pmmagd");

  // size parameter, unit [cm]
  // cavern
  G4double stone       = 100.0;  // Hall wall thickness 1 m
  G4double hallrad     = 800.0;  // Hall diameter 16 m
  G4double hallhheight = 650.0;  // Hall height 13 m
  // cryostat
  G4double tankhside  = 570.5; // cryostat cube side 11.41 m
  G4double outerwall  = 1.2;   // outer SS wall thickness
  G4double insulation = 62.0;  // polyurethane foam
  G4double innerwall  = 0.12;  // inner SS membrane
  // octagons, 2 planes in z
  const G4double rInner[]  = {0.0*cm, 0.0*cm}; // full volume
  const G4double rOutTPC[] = {177.5*cm, 177.5*cm};
  const G4double rOutAc[]  = {182.5*cm, 182.5*cm}; // Acrylic 5cm
  const G4double rOutIB[]  = {222.5*cm, 222.5*cm}; // Inner Buffer 40cm
  const G4double rOutAc2[] = {232.5*cm, 232.5*cm}; // Acrylic+Gd 10cm
  const G4double rOutOB[]  = {272.5*cm, 272.5*cm}; // Outer Buffer 40cm
  const G4double rOutCu[]  = {272.6*cm, 272.6*cm}; // Copper 1mm

  const G4double zTPC[] = {-175.0*cm, 175.0*cm};
  const G4double zAc[]  = {-180.0*cm, 180.0*cm};
  const G4double zIB[]  = {-220.0*cm, 220.0*cm};
  const G4double zAc2[] = {-230.0*cm, 230.0*cm};
  const G4double zOB[]  = {-270.0*cm, 270.0*cm};
  const G4double zCu[]  = {-270.1*cm, 270.1*cm};

  // total
  G4double offset =
    hallhheight - tankhside;  // shift cavern floor to keep detector centre at origin
  G4double worldside = hallrad + stone + offset + 0.1;  // larger than rest
  G4double larside =
    tankhside - outerwall - insulation - innerwall;  // cube side of LAr volume

  fvertexZ = (worldside - stone - 0.1) * cm;  // max vertex height
  fmaxrad  = (hallrad + stone) * cm;          // max vertex circle radius

  // Volumes for this geometry

  //
  // World
  //
  auto* worldSolid     = new G4Tubs("World", 0.0 * cm, worldside * cm, 
                                (hallhheight + stone + offset + 0.1) * cm, 0.0, CLHEP::twopi);
  auto* fWorldLogical  = new G4LogicalVolume(worldSolid, worldMaterial, "World_log");
  auto* fWorldPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), fWorldLogical,
                                           "World_phys", nullptr, false, 0);

  //
  // Cavern
  //
  auto* cavernSolid    = new G4Tubs("Cavern", 0.0 * cm, (hallrad + stone) * cm, 
                                (hallhheight + stone) * cm, 0.0, CLHEP::twopi);
  auto* fCavernLogical = new G4LogicalVolume(cavernSolid, stdRock, "Cavern_log");
  auto* fCavernPhysical =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., offset * cm), fCavernLogical,
                      "Cavern_phys", fWorldLogical, false, 0);

  //
  // Hall
  //
  auto* hallSolid = new G4Tubs("Hall", 0.0 * cm, hallrad * cm, hallhheight * cm, 0.0, CLHEP::twopi);
  auto* fHallLogical = new G4LogicalVolume(hallSolid, airMat, "Hall_log");
  auto* fHallPhysical =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., -stone * cm), fHallLogical,
                      "Hall_phys", fCavernLogical, false, 0, true);

  //
  // Tank
  //
  auto* tankSolid    = new G4Box("Tank", tankhside * cm, tankhside * cm, tankhside * cm);
  auto* fTankLogical = new G4LogicalVolume(tankSolid, steelMat, "Tank_log");
  auto* fTankPhysical =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., -stone * cm), fTankLogical,
                      "Tank_phys", fHallLogical, false, 0, true);

  //
  // Insulator
  //
  auto* puSolid     = new G4Box("Insulator", (tankhside - outerwall) * cm,
                            (tankhside - outerwall) * cm, (tankhside - outerwall) * cm);
  auto* fPuLogical  = new G4LogicalVolume(puSolid, puMat, "Pu_log");
  auto* fPuPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), fPuLogical, "Pu_phys",
                                        fTankLogical, false, 0, true);

  //
  // Membrane
  //
  auto* membraneSolid = new G4Box("Membrane", (tankhside - outerwall - insulation) * cm,
                                  (tankhside - outerwall - insulation) * cm,
                                  (tankhside - outerwall - insulation) * cm);
  auto* fMembraneLogical = new G4LogicalVolume(membraneSolid, steelMat, "Membrane_log");
  auto* fMembranePhysical =
    new G4PVPlacement(nullptr, G4ThreeVector(), fMembraneLogical, "Membrane_phys",
                      fPuLogical, false, 0, true);

  //
  // LAr filling box
  //
  auto* larSolid     = new G4Box("LAr", larside * cm, larside * cm, larside * cm);
  auto* fLarLogical  = new G4LogicalVolume(larSolid, larMat, "Lar_log");
  auto* fLarPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), fLarLogical,
                                         "Lar_phys", fMembraneLogical, false, 0, true);

  //
  // copper Faraday cage
  //
  G4int nSides  = 8; // regular Octagon
  G4int nPlanes = 2;
  auto* copperSolid = new G4Polyhedra("Copper", 0.0, CLHEP::twopi, nSides, nPlanes,
                                      zCu, rInner, rOutCu);
  auto* fCuLogical  = new G4LogicalVolume(copperSolid, copperMat, "Cu_log");
  auto* fCuPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), fCuLogical,
                                         "Cu_phys", fLarLogical, false, 0, true);

  //
  // LAr Outer buffer
  //
  auto* obSolid = new G4Polyhedra("OuterB", 0.0, CLHEP::twopi, nSides, nPlanes,  
                                      zOB, rinner, rOutOB);
  auto* fOBLogical  = new G4LogicalVolume(obSolid, larMat, "OB_log");  
  auto* fOBPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), fOBLogical,   
                                         "OB_phys", fCuLogical, false, 0, true);

  //
  // Acrylic + Gd
  //
  auto* ac2Solid = new G4Polyhedra("PMMAGd", 0.0, CLHEP::twopi, nSides, nPlanes,
                                      zAc2, rinner, rOutAc2);
  auto* fAc2Logical  = new G4LogicalVolume(ac2Solid, pmmagdMat, "Ac2_log");
  auto* fAc2Physical = new G4PVPlacement(nullptr, G4ThreeVector(), fAc2Logical,
                                         "Ac2_phys", fOBLogical, false, 0, true);

  //
  // LAr Inner buffer
  //
  auto* ibSolid = new G4Polyhedra("InnerB", 0.0, CLHEP::twopi, nSides, nPlanes,
                                      zIB, rinner, rOutIB);
  auto* fIBLogical  = new G4LogicalVolume(obSolid, larMat, "IB_log");
  auto* fIBPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), fIBLogical,
                                         "IB_phys", fAc2Logical, false, 0, true);

  //
  // Acrylic shell
  //                                     
  auto* acSolid = new G4Polyhedra("PMMA", 0.0, CLHEP::twopi, nSides, nPlanes,
                                      zAc, rinner, rOutAc);
  auto* fAcLogical  = new G4LogicalVolume(acSolid, pmmaMat, "Ac_log");
  auto* fAcPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), fAcLogical,
                                         "Ac_phys", fIBLogical, false, 0, true);

  //
  // TPC
  //
  auto* tpcSolid = new G4Polyhedra("TPC", 0.0, CLHEP::twopi, nSides, nPlanes,
                                      zTPC, rinner, rOutTPC);
  auto* fTPCLogical  = new G4LogicalVolume(tpcSolid, larMat, "TPC_log");
  auto* fTPCPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), fTPCLogical,
                                         "TPC_phys", fAcLogical, false, 0, true);
                                         

  //
  // Visualization attributes
  //
  fWorldLogical->SetVisAttributes(G4VisAttributes::GetInvisible());

  auto* redVisAtt = new G4VisAttributes(G4Colour::Red());
  redVisAtt->SetVisibility(true);
  auto* greyVisAtt = new G4VisAttributes(G4Colour::Grey());
  greyVisAtt->SetVisibility(true);
  auto* greenVisAtt = new G4VisAttributes(G4Colour::Green());
  greenVisAtt->SetVisibility(true);
  auto* blueVisAtt = new G4VisAttributes(G4Colour::Blue());
  blueVisAtt->SetVisibility(true);

  fCavernLogical->SetVisAttributes(redVisAtt);
  fHallLogical->SetVisAttributes(greyVisAtt);
  fTankLogical->SetVisAttributes(blueVisAtt);
  fPuLogical->SetVisAttributes(greyVisAtt);
  fMembraneLogical->SetVisAttributes(blueVisAtt);
  fLarLogical->SetVisAttributes(greyVisAtt);
  fCuLogical->SetVisAttributes(greenVisAtt);

  return fWorldPhysical;
}
