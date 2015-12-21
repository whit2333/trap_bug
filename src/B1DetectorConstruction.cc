#include "B1DetectorConstruction.hh"
#include "B1DetectorMessenger.hh"

#include "G4Material.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Tubs.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4UserLimits.hh"
#include "FakeSD.hh"
#include "G4GenericTrap.hh"
#include "G4TwoVector.hh"
#include "G4IntersectionSolid.hh"
#include "G4RotationMatrix.hh"

//___________________________________________________________________


B1DetectorConstruction::B1DetectorConstruction() : 
   G4VUserDetectorConstruction(), 
   world_x                      ( 10.0*m                  ),
   world_y                      ( 10.0*m                  ),
   world_z                      ( 10.0*m                  ),
   radiator_thickness           ( 5.0*cm                 ),
   collimator_target_center_gap ( 4.0*cm                 ),
   collimator_upstream_ID       ( 1.0*cm                 ),
   collimator_downstream_ID     ( 1.0*cm                 ),
   collimator_OD                ( 2.54*cm                ),
   outer_collimator_ID          ( 2.54*cm                ),
   outer_collimator_OD          ( 4.0*2.54*cm            ),
   collimator_diameter          ( 8.0*cm            ),
   collimator_z_end             ( 0.0*cm            ),
   collimator_tooth_slope       ( 0.0*cm),
   radiator_collimator_gap      ( 1.0*mm                 ),
   collimator_length            ( 4.0*cm                 ),
   beampipe_length              ( 20.0*cm ),
   beampipe_diameter            ( 8.0*cm  ),
   radiator_diameter            ( 8.0*cm  ),
   scoring_diameter             ( 20.0*cm ),
   scoring_length               ( 0.01*mm    ),
   window_diameter              ( 1.0*cm  ),
   window_thickness             ( 8.0*mm  ),
   scoring2_diameter            ( 20.0*cm ),
   scoring2_length              ( 0.01*mm    ),
   fScoringVolume               ( 0),
   fHasBeenBuilt(false)
{
   fMessenger = new B1DetectorMessenger(this);
   fCollimatorMatName = "G4_Cu";
   scoring_det      = 0;
   scoring2_det     = 0;

   world_mat        = 0;
   world_solid      = 0;
   world_log        = 0;
   world_phys       = 0;

   beampipe_mat     = 0;
   beampipe_solid   = 0;
   beampipe_log     = 0;
   beampipe_phys    = 0;
   radiator_mat     = 0;
   radiator_solid   = 0;
   radiator_log     = 0;
   radiator_phys    = 0;
   collimator_mat   = 0;
   collimator_solid = 0;
   collimator_log   = 0;
   collimator_phys  = 0;
   collimator2_mat   = 0;
   collimator2_solid = 0;
   collimator2_log   = 0;
   collimator2_phys  = 0;
   outer_collimator_mat   = 0;
   outer_collimator_solid = 0;
   outer_collimator_log   = 0;
   outer_collimator_phys  = 0;
   scoring_mat      = 0;
   scoring_solid    = 0;
   scoring_log      = 0;
   scoring_phys     = 0;
   window_mat       = 0;
   window_solid     = 0;
   window_log       = 0;
   window_phys      = 0;
   scoring2_mat     = 0;
   scoring2_solid   = 0;
   scoring2_log     = 0;
   scoring2_phys    = 0;
}
//___________________________________________________________________

B1DetectorConstruction::~B1DetectorConstruction()
{
   delete fMessenger;
}
//______________________________________________________________________________

void B1DetectorConstruction::Rebuild()
{
   B1DetectorConstruction::Construct();
   G4RunManager::GetRunManager()->GeometryHasBeenModified();
}
//______________________________________________________________________________

void B1DetectorConstruction::CalculatePositions()
{
   beampipe_pos     = { 0, 0, -beampipe_length/2.0 - radiator_thickness/2.0 };
   radiator_pos     = { 0, 0, 0.0 };
   collimator_pos   = { 0, 0, collimator_length/4.0 + radiator_thickness/2.0 + radiator_collimator_gap };
   collimator2_pos   = { 0, 0, 3.0*collimator_length/4.0 + radiator_thickness/2.0 + radiator_collimator_gap };
   outer_collimator_pos   = { 0, 0, collimator_length/2.0 + radiator_thickness/2.0 + radiator_collimator_gap };
   collimator_z_end = collimator_length + radiator_thickness/2.0 + radiator_collimator_gap;
   scoring_pos      = { 0, 0, radiator_thickness/2.0 + radiator_collimator_gap/2.0 };
   window_pos       = { 0, 0, collimator_z_end - window_thickness/2.0 };
   scoring2_pos     = { 0, 0, collimator_z_end + collimator_target_center_gap };
}
//______________________________________________________________________________
G4VPhysicalVolume* B1DetectorConstruction::Construct()
{  
   //std::cout << "============================================================" << std::endl;
   //std::cout << "B1DetectorConstruction::Construct()" << std::endl;

   // Get nist material manager

   G4NistManager* nist = G4NistManager::Instance();

   CalculatePositions();

   bool    checkOverlaps    = false;
   int     natoms           = 0;
   int     ncomponents      = 0;
   double  A                = 0.0;
   double  Z                = 0.0;
   double  thickness        = 0.0;
   double  surface_z        = 0.0;
   double  red              = 0.0;
   double  green            = 0.0;
   double  blue             = 0.0;
   double  alpha            = 0.0;
   double  density          = 0.0;
   double  pressure         = 0.0;
   double  temperature      = 0.0;
   double  a                = 0.0;


   // ------------------------------------------------------------------------
   // World
   // ------------------------------------------------------------------------
   density     = universe_mean_density;
   pressure    = 1.e-7*bar;
   temperature = 0.1*kelvin;
   red         = 0.0/256.0;
   green       = 200.0/256.0;
   blue        = 0.0/256.0;
   alpha       = 0.4;

   world_mat   = nist->FindOrBuildMaterial("G4_AIR");
   if(!world_solid) world_solid = new G4Box( "World", 0.5*world_x, 0.5 * world_y, 0.5 * world_z );
   if(!world_log)   world_log = new G4LogicalVolume( world_solid, world_mat, "world_log" );
   if(!world_phys)  world_phys  = new G4PVPlacement( 0, G4ThreeVector(), world_log, "world_phys", 0, false, 0, checkOverlaps );

   G4Colour            world_color {red, green, blue, alpha }; 
   G4VisAttributes   * world_vis   = new G4VisAttributes(world_color);
   //(*world_vis) = G4VisAttributes::GetInvisible();
   world_vis->SetForceWireframe(true);
   world_log->SetVisAttributes(world_vis);

   // ------------------------------------------------------------------------
   // beam vacuum  
   // ------------------------------------------------------------------------
   density     = universe_mean_density;
   pressure    = 1.e-7*bar;
   temperature = 0.1*kelvin;
   red       = 0.0/256.0;
   green     = 0.0/256.0;
   blue      = 192.0/256.0;
   alpha     = 0.4;


   if(!beampipe_mat)   beampipe_mat   = new G4Material("beampipe_mat", /*z=*/1.0, /*a=*/1.01*g/mole, density, kStateGas,temperature,pressure);
   if(!beampipe_solid) beampipe_solid  = new G4Tubs("beampipe_solid", 0.0, beampipe_diameter/2.0, beampipe_length/2.0, 0.0, 360.*deg );
   if(!beampipe_log  ) beampipe_log   = new G4LogicalVolume(beampipe_solid, beampipe_mat,"beampipe_log");
   if(!beampipe_phys ) beampipe_phys  = new G4PVPlacement(0,beampipe_pos, beampipe_log, "beampipe_phys",world_log,false,0,checkOverlaps);                                  

   G4Colour            beampipe_color {red, green, blue, alpha };   // Gray 
   G4VisAttributes   * beampipe_vis   = new G4VisAttributes(beampipe_color);
   beampipe_log->SetVisAttributes(beampipe_vis);


   // ------------------------------------------------------------------------
   // radiator target centered at origin
   // ------------------------------------------------------------------------
   red       = 256.0/256.0;
   green     = 1.0/256.0;
   blue      = 1.0/256.0;
   alpha     = 0.4;

   if(radiator_phys) delete radiator_phys;
   if(radiator_log) delete radiator_log;
   if(radiator_solid) delete radiator_solid;

   //radiator_mat   = nist->FindOrBuildMaterial("G4_Cu");
   // define Elements
   a = 1.01*g/mole;
   G4Element* elH  = new G4Element("Hydrogen", "H",  1.0, a);
   a = 12.01*g/mole;
   density   = 1.032*g/cm3;
   G4Element* elC  = new G4Element("Carbon"  , "C",  6.0, a);
   radiator_mat = new G4Material("Scintillator", density, 2);
   radiator_mat->AddElement(elC, 9);
   radiator_mat->AddElement(elH, 10);

   const G4int NUMENTRIES = 9;
   G4double Scnt_PP[NUMENTRIES] = { 1.6*eV, 6.7*eV, 6.8*eV, 6.9*eV,
      7.0*eV, 7.1*eV, 7.2*eV, 7.3*eV, 7.4*eV };

   G4double Scnt_FAST[NUMENTRIES] = { 0.000134, 0.004432, 0.053991, 0.241971, 
      0.398942, 0.000134, 0.004432, 0.053991,
      0.241971 };
   G4double Scnt_SLOW[NUMENTRIES] = { 0.000010, 0.000020, 0.000030, 0.004000,
      0.008000, 0.005000, 0.020000, 0.001000,
      0.000010 };
   G4double rindex[NUMENTRIES]     = { 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58 };

   G4MaterialPropertiesTable* Scnt_MPT = new G4MaterialPropertiesTable();

   Scnt_MPT->AddProperty("FASTCOMPONENT", Scnt_PP, Scnt_FAST, NUMENTRIES);
   Scnt_MPT->AddProperty("SLOWCOMPONENT", Scnt_PP, Scnt_SLOW, NUMENTRIES);
   Scnt_MPT->AddProperty(     "RINDEX"  , Scnt_PP, rindex,    NUMENTRIES);
   Scnt_MPT->AddConstProperty("SCINTILLATIONYIELD", 1./MeV);
   Scnt_MPT->AddConstProperty("RESOLUTIONSCALE", 2.0);
   Scnt_MPT->AddConstProperty("FASTTIMECONSTANT",  1.*ns);
   Scnt_MPT->AddConstProperty("SLOWTIMECONSTANT", 10.*ns);
   Scnt_MPT->AddConstProperty("YIELDRATIO", 0.8);

   //Scnt->SetMaterialPropertiesTable(Scnt_MPT);
   //const G4int NUMENTRIES = 2; //32;
   //G4double ppckov[NUMENTRIES]     = { 1.9074494*eV  , 6.5254848*eV  };
   //G4double rindex[NUMENTRIES]     = { 1.58          , 1.58          };
   //G4double absorption[NUMENTRIES] = { 1000.0000000*m, 802.8323273*m };
   //G4MaterialPropertiesTable *scint_MPT = new G4MaterialPropertiesTable();
   //scint_MPT->AddConstProperty("SCINTILLATIONYIELD", 1000./MeV);
   //scint_MPT->AddProperty(     "RINDEX"            , ppckov,  rindex,     NUMENTRIES);
   //scint_MPT->AddProperty(     "ABSLENGTH"         , ppckov,  absorption, NUMENTRIES);

   radiator_mat->SetMaterialPropertiesTable(Scnt_MPT);

   radiator_solid = new G4Tubs("radiator_solid", 0.0, radiator_diameter/2.0, radiator_thickness/2.0, 0.0, 360.*deg );
   radiator_log   = new G4LogicalVolume(radiator_solid, radiator_mat,"radiator_log");
   radiator_phys  = new G4PVPlacement(0,radiator_pos, radiator_log, "radiator_phys",world_log,false,0,checkOverlaps);                                  
   G4Colour            radiator_color {red, green, blue, alpha };   // Gray 
   G4VisAttributes   * radiator_vis   = new G4VisAttributes(radiator_color);
   radiator_log->SetVisAttributes(radiator_vis);

   //G4UserLimits * scoring_limits = new G4UserLimits(0.004*um);
   //scoring_log->SetUserLimits(scoring_limits);

   // ------------------------------------------------------------------------
   // Inner Collimator 
   // ------------------------------------------------------------------------
   // Part I : Upstream inner cone 
   red       = 250.0/256.0;
   green     = 0.0/256.0;
   blue      = 1.0/256.0;
   alpha     = 0.4;

   G4VSolid * temp_box    = new G4Box("DC_placement_box_solid",5*m,5*m,5*m);
   G4VSolid * temp_region = 0;
   //temp_region    = new G4Box("R1trap_solid_BOX", 4.0*m, 4.0*m, 1.0*m); 

   double trap_width = 195.259*mm;
   std::vector< G4TwoVector> trap_points = {
      {-103.734*mm, 179.672 *mm},
      {-1625.95*mm, 3172.94 *mm},
      {1625.95 *mm, 3172.94 *mm},
      {103.734 *mm, 179.672 *mm},
      {-103.734*mm, -2.42984*mm},
      {-1683.74*mm, 3103.61 *mm},
      {1683.74 *mm, 3103.61 *mm},
      {103.734 *mm, -2.42984*mm}
   };
   temp_region    = new G4GenericTrap("R1trap_solid", trap_width/2.0, trap_points);
   G4VSolid * fRegion1_solid = new G4IntersectionSolid("RI_solid", temp_box, temp_region, 0,  G4ThreeVector(0.0,0.0,trap_width/2.0) );

   if(collimator_phys)  delete collimator_phys;
   if(collimator_log)   delete collimator_log;
   if(collimator_solid) delete collimator_solid;

   G4NistManager * matman = G4NistManager::Instance();
   G4Element     * Ar     = new G4Element("Argon", "Ar", /*z    = */18, /*a            = */ 39.95*g/mole);
   G4Material    * fGasMaterial  = new G4Material("DC_gas", /* density = */ 1.8*mg/cm3, /*nel = */ 3);
   fGasMaterial->AddElement(Ar, 90*perCent);
   fGasMaterial->AddMaterial(matman->FindOrBuildMaterial("G4_O"),  6.6*perCent);
   fGasMaterial->AddMaterial(matman->FindOrBuildMaterial("G4_C"),  3.4*perCent);

   collimator_mat   = fGasMaterial;//nist->FindOrBuildMaterial(fCollimatorMatName);
   collimator_solid = fRegion1_solid;
   collimator_log   = new G4LogicalVolume(collimator_solid, collimator_mat,"collimator_log");
   collimator_phys  = new G4PVPlacement(0,collimator_pos, collimator_log, "collimator_phys",world_log,false,0,checkOverlaps);                                  

   G4Colour            collimator_color {red, green, blue, alpha };   // Gray 
   G4VisAttributes   * collimator_vis   = new G4VisAttributes(collimator_color);
   collimator_vis->SetForceWireframe(true);
   collimator_log->SetVisAttributes(collimator_vis);

   // ------------------------------------------------------------------------
   // Part II  
   red       = 0.0/256.0;
   green     = 200.0/256.0;
   blue      = 30.0/256.0;
   alpha     = 0.4;
   G4Colour            collimator2_color {red, green, blue, alpha };   // Gray 
   G4VisAttributes   * collimator2_vis   = new G4VisAttributes(collimator2_color);
   collimator2_vis->SetForceWireframe(true);
   G4RotationMatrix * wire_rot = new G4RotationMatrix();
   wire_rot->rotateY(90.0*degree);

   double wire_spacing = 1.0*cm;
   double sqrtof3 = sin(80.0*degree);//sqrt(3.0);

   for(int iWire = 0; iWire < 300; iWire ++ ) {
      double deltaL     = iWire*wire_spacing/sqrtof3;
      double hex_length = 21.5*cm + deltaL ;
      double y_position = 10.0*cm + wire_spacing/2.0 + iWire*wire_spacing;

      collimator2_pos = {0.0, y_position, trap_width/2.0};

      G4VSolid * hex_polyhedra = new G4Box("wire_box",wire_spacing/3.0,wire_spacing/3.0, hex_length/2.0 );

      collimator2_mat   = fGasMaterial;//nist->FindOrBuildMaterial(fCollimatorMatName);
      collimator2_solid = hex_polyhedra;
      collimator2_log   = new G4LogicalVolume(collimator2_solid, collimator2_mat,"collimator2_log");
      collimator2_phys  = new G4PVPlacement(wire_rot,collimator2_pos, collimator2_log, "collimator2_phys",collimator_log,false,0,false);

      collimator2_log->SetVisAttributes(collimator2_vis);
   }


   // ------------------------------------------------------------------------
   // Outer Collimator 
   // ------------------------------------------------------------------------
   //red       = 0.0/256.0;
   //green     = 256.0/256.0;
   //blue      = 1.0/256.0;
   //alpha     = 0.4;

   //if(outer_collimator_phys)  delete outer_collimator_phys;
   //if(outer_collimator_log)   delete outer_collimator_log;
   //if(outer_collimator_solid) delete outer_collimator_solid;

   //outer_collimator_mat   = nist->FindOrBuildMaterial("G4_Cu");
   //outer_collimator_solid = new G4Tubs("outer_collimator_solid", outer_collimator_ID/2.0, outer_collimator_OD/2.0, collimator_length/2.0, 0.0, 360.*deg );
   //outer_collimator_log   = new G4LogicalVolume(outer_collimator_solid, outer_collimator_mat,"outer_collimator_log");
   ////outer_collimator_phys  = new G4PVPlacement(0,outer_collimator_pos, outer_collimator_log, "outer_collimator_phys",world_log,false,0,checkOverlaps);                                  
   //G4Colour            outer_collimator_color {red, green, blue, alpha };   // Gray 
   //G4VisAttributes   * outer_collimator_vis   = new G4VisAttributes(outer_collimator_color);
   //outer_collimator_vis->SetForceWireframe(true);
   //outer_collimator_log->SetVisAttributes(outer_collimator_vis);

   //// ------------------------------------------------------------------------
   //// Scoring volume 
   //// ------------------------------------------------------------------------
   //red       = 177.0/256.0;
   //green     = 104.0/256.0;
   //blue      = 177.0/256.0;
   //alpha     = 0.4;

   //if(scoring_phys) delete scoring_phys;
   //if(scoring_log) delete scoring_log;
   //if(scoring_solid) delete scoring_solid;
   ////if(scoring_det) delete scoring_det;

   //scoring_mat   = nist->FindOrBuildMaterial("G4_AIR");
   //scoring_solid = new G4Tubs("scoring_solid", 0.0, scoring_diameter/2.0, scoring_length/2.0, 0.0, 360.*deg );
   //scoring_log   = new G4LogicalVolume(scoring_solid, scoring_mat,"scoring_log");
   ////scoring_phys  = new G4PVPlacement(0,scoring_pos, scoring_log, "scoring_phys",world_log,false,0,checkOverlaps);                                  

   //G4Colour            scoring_color {red, green, blue, alpha };   // Gray 
   //G4VisAttributes   * scoring_vis   = new G4VisAttributes(scoring_color);
   //scoring_log->SetVisAttributes(scoring_vis);

   ////G4UserLimits * scoring_limits = new G4UserLimits(0.004*um);
   ////scoring_log->SetUserLimits(scoring_limits);

   //if(!scoring_det) scoring_det = new FakeSD("/FakeSD1");
   //SetSensitiveDetector("scoring_log",scoring_det);

   //// ------------------------------------------------------------------------
   //// window 
   //// ------------------------------------------------------------------------
   //red       = 5.0/256.0;
   //green     = 255.0/256.0;
   //blue      = 1.0/256.0;
   //alpha     = 0.4;

   //if(window_phys) delete window_phys;
   //if(window_log) delete window_log;
   //if(window_solid) delete window_solid;

   //window_diameter = collimator_downstream_ID;
   //double cone_slope = collimator_tooth_slope;//(collimator_ID/4.0)/(collimator_length/2.0);
   //window_mat   = nist->FindOrBuildMaterial("G4_Cu");
   //window_solid = new G4Cons("collimator2_solid", 
   //      0.0, window_diameter/2.0 - cone_slope*window_thickness, 
   //      0.0, window_diameter/2.0,
   //      window_thickness/2.0, 0.0, 360.*deg );
   //window_log   = new G4LogicalVolume(window_solid, window_mat,"window_log");
   ////window_phys  = new G4PVPlacement(0,window_pos, window_log, "window_phys",world_log,false,0,checkOverlaps);                                  

   //G4Colour            window_color {red, green, blue, alpha };   // Gray 
   //G4VisAttributes   * window_vis   = new G4VisAttributes(window_color);
   //window_log->SetVisAttributes(window_vis);

   //// ------------------------------------------------------------------------
   //// Scoring volume 
   //// ------------------------------------------------------------------------
   //red       = 66.0/256.0;
   //green     = 174.0/256.0;
   //blue      = 208.0/256.0;
   //alpha     = 0.4;

   //if(scoring2_phys) delete scoring2_phys;
   //if(scoring2_log) delete scoring2_log;
   //if(scoring2_solid) delete scoring2_solid;
   ////if(scoring2_det) delete scoring2_det;

   //scoring2_mat   = nist->FindOrBuildMaterial("G4_AIR");
   //scoring2_solid  = new G4Tubs("scoring2_solid", 0.0, scoring2_diameter/2.0, scoring2_length/2.0, 0.0, 360.*deg );
   //scoring2_log   = new G4LogicalVolume(scoring2_solid, scoring2_mat,"scoring2_log");
   ////scoring2_phys  = new G4PVPlacement(0,scoring2_pos, scoring2_log, "scoring2_phys",world_log,false,0,checkOverlaps);                                  

   //G4Colour            scoring2_color {red, green, blue, alpha };   // Gray 
   //G4VisAttributes   * scoring2_vis   = new G4VisAttributes(scoring2_color);
   //scoring2_log->SetVisAttributes(scoring2_vis);

   ////G4UserLimits * scoring_limits = new G4UserLimits(0.004*um);
   ////scoring_log->SetUserLimits(scoring_limits);

   //if(!scoring2_det) scoring2_det = new FakeSD("/FakeSD2");
   //SetSensitiveDetector("scoring2_log",scoring2_det);


   // --------------------------------------------------------

   //fScoringVolume = scoring2_log;
   fHasBeenBuilt = true;

   return world_phys;
}
//___________________________________________________________________

void B1DetectorConstruction::SetCollimatorMaterial(G4String materialName)
{
   fCollimatorMatName = materialName;
   if(fHasBeenBuilt) Rebuild();
}
//______________________________________________________________________________

void     B1DetectorConstruction::SetRadiatorCollimatorGap(G4double l)
{   
   radiator_collimator_gap = l; 
   if(fHasBeenBuilt) Rebuild();
}
//______________________________________________________________________________

void     B1DetectorConstruction::SetInnerCollimatorOD(G4double l)
{   
   collimator_OD       = l;
   outer_collimator_ID = l;
   if(fHasBeenBuilt) Rebuild();
}
//______________________________________________________________________________

void     B1DetectorConstruction::SetInnerCollimatorUpstreamID(G4double l)
{   
   collimator_upstream_ID       = l;
   if(fHasBeenBuilt) Rebuild();
}
//______________________________________________________________________________

void     B1DetectorConstruction::SetInnerCollimatorDownstreamID(G4double l)
{   
   collimator_downstream_ID       = l;
   if(fHasBeenBuilt) Rebuild();
}
//______________________________________________________________________________
void     B1DetectorConstruction::SetCollimatorLength(G4double l)
{   
   collimator_length = l;
   if(fHasBeenBuilt) Rebuild();
}
//______________________________________________________________________________

void     B1DetectorConstruction::SetCollimatorToothSlope(G4double l)
{   
   collimator_tooth_slope = l;
   if(fHasBeenBuilt) Rebuild();
}
//______________________________________________________________________________

void     B1DetectorConstruction::PrintConfigInfo() const
{   

   if(fHasBeenBuilt) {
      std::cout << "------------------------------------------------------------------------\n";
      std::cout 
         << " Collimator parameters :  \n"
         << "                   weight : " << collimator_log->GetMass(true)/kg << " kg\n"
         << "                 material : " << fCollimatorMatName               << "\n"
         << "                   length : " << collimator_length/cm             << " cm\n"
         << "  collimator upstream_ID  : " << collimator_upstream_ID/cm        << " cm\n"
         << "collimator downstream_ID  : " << collimator_downstream_ID/cm      << " cm\n"
         << "      outer collimator ID : " << outer_collimator_ID/cm      << " cm\n"
         << "      outer collimator OD : " << outer_collimator_OD/cm      << " cm\n"
         << "  collimator tooth slope  : " << collimator_tooth_slope      << " \n"
         << "  collimator tooth angle  : " << atan(collimator_tooth_slope)/degree  << " \n"
         << "           collimator OD  : " << collimator_OD/cm                 << " cm\n"
         << " radiator collimator gap  : " << radiator_collimator_gap/cm       << " cm\n"
         << "  collimator target dist  : " << collimator_target_center_gap/cm  << " cm\n"
         << "      radiator thickness  : " << radiator_thickness/cm            << " cm\n";
   } else {
      std::cout << " detector not built yet" << std::endl;
   }

}

