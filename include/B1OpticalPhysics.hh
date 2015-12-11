#ifndef B1OpticalPhysics_h
#define B1OpticalPhysics_h 1

#include "globals.hh"

//#include "G4OpB1.hh"
#include "G4Cerenkov.hh"
#include "G4Scintillation.hh"

#include "G4OpMieHG.hh"
#include "G4OpRayleigh.hh"
#include "G4OpAbsorption.hh"
#include "G4OpBoundaryProcess.hh"

#include "G4VPhysicsConstructor.hh"

class B1OpticalPhysics : public G4VPhysicsConstructor
{
  public:

    B1OpticalPhysics(G4bool toggle=true);
    virtual ~B1OpticalPhysics();

    virtual void ConstructParticle();
    virtual void ConstructProcess();

    //G4OpB1* GetB1Process() {return fB1Process;}
    G4Cerenkov* GetCerenkovProcess() {return fCerenkovProcess;}
    G4Scintillation* GetScintillationProcess() {return fScintProcess;}
    G4OpAbsorption* GetAbsorptionProcess() {return fAbsorptionProcess;}
    G4OpRayleigh* GetRayleighScatteringProcess() {return fRayleighScattering;}
    G4OpMieHG* GetMieHGScatteringProcess() {return fMieHGScatteringProcess;}
    G4OpBoundaryProcess* GetBoundaryProcess() { return fBoundaryProcess;}

    void SetNbOfPhotonsCerenkov(G4int);

private:

    //G4OpB1*             fB1Process;
    G4Cerenkov*          fCerenkovProcess;
    G4Scintillation*     fScintProcess;
    G4OpAbsorption*      fAbsorptionProcess;
    G4OpRayleigh*        fRayleighScattering;
    G4OpMieHG*           fMieHGScatteringProcess;
    G4OpBoundaryProcess* fBoundaryProcess;
 
    G4bool fAbsorptionOn;

};
#endif
