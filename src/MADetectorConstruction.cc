#include "MADetectorConstruction.hh"

#include <cmath>

#include "G4Box.hh"
#include "G4Cons.hh"
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
#include "MACrystalSD.hh"

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
    G4String crystalSDname  = "CrystalSD";
    MACrystalSD* aCrystalSD = new MACrystalSD(crystalSDname,
                                                  "CrystalHitsCollection");
    fSD.Put(aCrystalSD);

    // Also only add it once to the SD manager!
    G4SDManager::GetSDMpointer()->AddNewDetector(fSD.Get());

    SetSensitiveDetector("TPC_log", fSD.Get());
    SetSensitiveDetector("TIB_log", fSD.Get());
    SetSensitiveDetector("TOB_log", fSD.Get());

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
  G4double stone     = 100.0;  // Hall wall thickness 1 m
  G4double hallhside = 850.0;  // Hall cube side 17 m
  // cryostat
  G4double tankhside  = 650;   // cryostat cube side 13 m
  G4double outerwall  = 1.2;   // outer SS wall thickness
  G4double insulation = 80.0;  // polyurethane foam
  G4double innerwall  = 0.12;  // inner SS membrane
  // copper tubes with Germanium ROI
  G4double copper    = 0.35;    // tube thickness 3.5 mm
  G4double curad     = 40.0;    // copper tube diam 80 cm
  G4double cuhheight = 334.34;  // copper tube height 7 m inside cryostat
  G4double cushift   = 234.34;  // shift cu tube inside cryostat to top
  G4double ringrad   = 100.0;   // cu tube placement ring radius
  // Ge cylinder for 250 kg at 5.32 g/cm3
  G4double roiradius     = 30.0;   // string radius curad - Ge radius - gap

  G4double gerad          = 4.0;                      // Ge radius
  G4double gehheight      = 5.0;                      // full height 10 cm
  G4double gegap          = 3.0;                      // gap between Ge 3cm
  G4double layerthickness = gegap + 2 * gehheight;    // 13 cm total
  G4int    nofLayers      = 8;   // 8 Ge + 7 gaps = 1010 mm string height
  G4int    nofStrings     = 12;  // 12 strings  of 8 Ge each

  // total
  G4double offset =
    hallhside - tankhside;  // shift cavern floor to keep detector centre at origin
  G4double worldside = hallhside + stone + offset + 0.1;  // larger than rest
  G4double larside =
    tankhside - outerwall - insulation - innerwall;  // cube side of LAr volume

  fvertexZ = (worldside - stone - 0.1) * cm;  // max vertex height
  fmaxrad  = hallhside * cm;                   // max vertex circle radius

  // Volumes for this geometry

  //
  // World
  //
  auto* worldSolid     = new G4Box("World", worldside * cm, worldside * cm, worldside * cm);
  auto* fWorldLogical  = new G4LogicalVolume(worldSolid, worldMaterial, "World_log");
  auto* fWorldPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), fWorldLogical,
                                           "World_phys", nullptr, false, 0);

  //
  // Cavern
  //
  auto* cavernSolid    = new G4Box("Cavern", (hallhside + stone) * cm,
                                (hallhside + stone) * cm, (hallhside + stone) * cm);
  auto* fCavernLogical = new G4LogicalVolume(cavernSolid, stdRock, "Cavern_log");
  auto* fCavernPhysical =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., offset * cm), fCavernLogical,
                      "Cavern_phys", fWorldLogical, false, 0);

  //
  // Hall
  //
  auto* hallSolid = new G4Box("Cavern", hallhside * cm, hallhside * cm, hallhside * cm);
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
  // LAr
  //
  auto* larSolid     = new G4Box("LAr", larside * cm, larside * cm, larside * cm);
  auto* fLarLogical  = new G4LogicalVolume(larSolid, larMat, "Lar_log");
  auto* fLarPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), fLarLogical,
                                         "Lar_phys", fMembraneLogical, false, 0, true);

  //
  // copper tubes, hollow cylinder shell
  //
  auto* copperSolid = new G4Tubs("Copper", (curad - copper) * cm, curad * cm,
                                 cuhheight * cm, 0.0, CLHEP::twopi);

  //
  // ULAr bath, solid cylinder
  //
  auto* ularSolid = new G4Tubs("ULar", 0.0 * cm, (curad - copper) * cm, cuhheight * cm,
                               0.0, CLHEP::twopi);

  // tower; logical volumes
  auto* fCopperLogical = new G4LogicalVolume(copperSolid, copperMat, "Copper_log");
  auto* fUlarLogical   = new G4LogicalVolume(ularSolid, larMat, "ULar_log");

  //
  // Germanium, solid cylinder
  //
  // layers in tower
  auto* layerSolid = new G4Tubs("LayerSolid", 0.0 * cm, gerad * cm,
                                (gehheight + gegap / 2.0) * cm, 0.0, CLHEP::twopi);
  
  auto* fLayerLogical = new G4LogicalVolume(layerSolid, larMat, "Layer_log");
                      
  // fill one layer
  auto* geSolid =
    new G4Tubs("ROI", 0.0 * cm, gerad * cm, gehheight * cm, 0.0, CLHEP::twopi);
    
  auto* fGeLogical = new G4LogicalVolume(geSolid, roiMat, "Ge_log");
  new G4PVPlacement(nullptr, G4ThreeVector(0.0, 0.0, -gegap / 2.0 * cm), fGeLogical,   
                    "Ge_phys", fLayerLogical, false, 0, true);
  
  auto* gapSolid =
    new G4Tubs("Gap", 0.0 * cm, gerad * cm, gegap / 2.0 * cm, 0.0, CLHEP::twopi);
  
  auto* fGapLogical = new G4LogicalVolume(gapSolid, larMat, "Gap_log");
  new G4PVPlacement(nullptr, G4ThreeVector(0.0, 0.0, gehheight * cm), fGapLogical,
                    "Gap_phys", fLayerLogical, false, 0, true);
  
  // place layers as mother volume with unique copy number
  G4double step = (gehheight + gegap / 2) * cm;
  G4double xpos;
  G4double ypos;
  G4double angle = CLHEP::twopi / nofStrings;

  // layer logical into ULarlogical
  for(G4int j = 0; j < nofStrings; j++)
  {
    xpos = roiradius * cm * std::cos(j * angle);
    ypos = roiradius * cm * std::sin(j * angle);
    for(G4int i = 0; i < nofLayers; i++)
    {
      new G4PVPlacement(
        nullptr,
        G4ThreeVector(xpos, ypos,  
                      - step + (nofLayers / 2 * layerthickness - i * layerthickness) * cm),
        fLayerLogical, "Layer_phys", fUlarLogical, false, i + j * nofLayers, true);
    }
  }


  // placements
  new G4PVPlacement(nullptr, G4ThreeVector(ringrad * cm, 0., cushift * cm),
                    fCopperLogical, "Copper_phys", fLarLogical, false, 0, true);

  new G4PVPlacement(nullptr, G4ThreeVector(ringrad * cm, 0., cushift * cm), fUlarLogical,
                    "ULar_phys", fLarLogical, false, 0, true);


  // tower 2
  new G4PVPlacement(nullptr, G4ThreeVector(0., ringrad * cm, cushift * cm),
                    fCopperLogical, "Copper_phys2", fLarLogical, false, 1, true);

  new G4PVPlacement(nullptr, G4ThreeVector(0., ringrad * cm, cushift * cm), fUlarLogical,
                    "ULar_phys2", fLarLogical, false, 1, true);

  // tower 3
  new G4PVPlacement(nullptr, G4ThreeVector(-ringrad * cm, 0., cushift * cm),
                    fCopperLogical, "Copper_phys3", fLarLogical, false, 2, true);

  new G4PVPlacement(nullptr, G4ThreeVector(-ringrad * cm, 0., cushift * cm), fUlarLogical,
                    "ULar_phys3", fLarLogical, false, 2, true);

  // tower 4
  new G4PVPlacement(nullptr, G4ThreeVector(0., -ringrad * cm, cushift * cm),
                    fCopperLogical, "Copper_phys4", fLarLogical, false, 3, true);

  new G4PVPlacement(nullptr, G4ThreeVector(0., -ringrad * cm, cushift * cm), fUlarLogical,
                    "ULar_phys4", fLarLogical, false, 3, true);

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
  fCopperLogical->SetVisAttributes(greenVisAtt);
  fUlarLogical->SetVisAttributes(greyVisAtt);
  fGeLogical->SetVisAttributes(redVisAtt);

  return fWorldPhysical;
}

