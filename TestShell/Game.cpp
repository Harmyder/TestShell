#include "stdafx.h"

#include "Game.h"
#include "Viewer/UserInteractor.h"
#include "Internal/Launcher.h"
#include "Internal/BaseSimulation.h"
#include "Internal/SimulationManager.h"
#include "Internal/SimulationFactory.h"
#include "Pipeline/Importer/FBXImporter.h"
#include "Pipeline/SceneManager/SceneManager.h"

using namespace std;
using namespace Pipeline;
using namespace Viewer;

Game::Game() {}
Game::~Game() {}

int Game::Run(HINSTANCE hInstance)
{
    userInteractor_ = make_unique<UserInteractor>(hInstance);
    const float dT = 1 / 60.f;
    userInteractor_->SetDeltaTime(dT);

    FbxImporter::GetInstance().Init();
    SceneManager::GetInstance().Init();

    {
        SimulationManager simulationManager;
        simulationManager.Init();

        launcher_ = make_unique<Launcher>();
        int nextSimulation_ = -1; // Start with first simulation
        while (++nextSimulation_ < simulationManager.GetFactoriesCount() && !launcher_->IsMustStop())
        {
            BaseFactory *factory = simulationManager.GetFactory(nextSimulation_);
            auto simulation = factory->Create(userInteractor_->GetViewport(), userInteractor_->GetGameInput());
            simulation->Init();

            launcher_->Init(userInteractor_.get(), simulation.get());
            launcher_->Loop();
            launcher_->Finish();
        }
        simulationManager.Close();
    }

    SceneManager::GetInstance().Close();
    FbxImporter::GetInstance().Close();

    return 0;
}

void Game::Step(float dT)
{
    UNREFERENCED_PARAMETER(dT);
}
