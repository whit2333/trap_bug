#include "G4LossTableManager.hh"
#include "G4EmSaturation.hh"

#include "B1OpticalPhysics.hh"

B1OpticalPhysics::B1OpticalPhysics(G4bool toggle)
    : G4VPhysicsConstructor("Optical")
{
  //fB1Process                 = nullptr;
  fScintProcess              = nullptr;
  fCerenkovProcess           = nullptr;
  fBoundaryProcess           = nullptr;
  fAbsorptionProcess         = nullptr;
  fRayleighScattering        = nullptr;
  fMieHGScatteringProcess    = nullptr;

  fAbsorptionOn              = toggle;
}

B1OpticalPhysics::~B1OpticalPhysics() { }

#include "G4OpticalPhoton.hh"

void B1OpticalPhysics::ConstructParticle()
{
  G4OpticalPhoton::OpticalPhotonDefinition();
}

#include "G4ProcessManager.hh"

void B1OpticalPhysics::ConstructProcess()
{
    G4cout << "B1OpticalPhysics:: Add Optical Physics Processes"
           << G4endl;

  //fB1Process = new G4OpB1();
  fScintProcess = new G4Scintillation();
  fScintProcess->SetScintillationYieldFactor(1.);
  fScintProcess->SetTrackSecondariesFirst(true);

  //fCerenkovProcess = new G4Cerenkov();
  //fCerenkovProcess->SetMaxNumPhotonsPerStep(300);
  //fCerenkovProcess->SetTrackSecondariesFirst(true);

  fAbsorptionProcess      = new G4OpAbsorption();
  fRayleighScattering     = new G4OpRayleigh();
  fMieHGScatteringProcess = new G4OpMieHG();
  fBoundaryProcess        = new G4OpBoundaryProcess();

  G4ProcessManager* pManager =
                G4OpticalPhoton::OpticalPhoton()->GetProcessManager();

  if (!pManager) {
     std::ostringstream o;
     o << "Optical Photon without a Process Manager";
     G4Exception("B1OpticalPhysics::ConstructProcess()","",
                  FatalException,o.str().c_str());
  }

  if (fAbsorptionOn) pManager->AddDiscreteProcess(fAbsorptionProcess);

  //pManager->AddDiscreteProcess(fRayleighScattering);
  //pManager->AddDiscreteProcess(fMieHGScatteringProcess);

  pManager->AddDiscreteProcess(fBoundaryProcess);

  //fB1Process->UseTimeProfile("delta");
  //fB1Process->UseTimeProfile("exponential");

  //pManager->AddDiscreteProcess(fB1Process);

  fScintProcess->SetScintillationYieldFactor(1.);
  fScintProcess->SetScintillationExcitationRatio(0.0);
  fScintProcess->SetTrackSecondariesFirst(true);

  // Use Birks Correction in the Scintillation process

  G4EmSaturation* emSaturation = G4LossTableManager::Instance()->EmSaturation();
  fScintProcess->AddSaturation(emSaturation);

  aParticleIterator->reset();
  while ( (*aParticleIterator)() ){

    G4ParticleDefinition* particle = aParticleIterator->value();
    G4String particleName = particle->GetParticleName();

    pManager = particle->GetProcessManager();
    if (!pManager) {
       std::ostringstream o;
       o << "Particle " << particleName << "without a Process Manager";
       G4Exception("B1OpticalPhysics::ConstructProcess()","",
                    FatalException,o.str().c_str());
    }

    //if(fCerenkovProcess->IsApplicable(*particle)){
    //  pManager->AddProcess(fCerenkovProcess);
    //  pManager->SetProcessOrdering(fCerenkovProcess,idxPostStep);
    //}
    if(fScintProcess->IsApplicable(*particle)){
      pManager->AddProcess(fScintProcess);
      pManager->SetProcessOrderingToLast(fScintProcess,idxAtRest);
      pManager->SetProcessOrderingToLast(fScintProcess,idxPostStep);
    }

  }
}

void B1OpticalPhysics::SetNbOfPhotonsCerenkov(G4int maxNumber)
{
  fCerenkovProcess->SetMaxNumPhotonsPerStep(maxNumber);
}
