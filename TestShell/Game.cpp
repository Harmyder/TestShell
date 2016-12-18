#include "stdafx.h"

#include "Game.h"
#include "Viewer/UserInteractor.h"
#include "Internal/Launcher.h"
#include "Internal/BaseSimulation.h"
#include "Internal/SimulationManager.h"
#include "Internal/SimulationFactory.h"
#include "Pipeline/Importer/FBXImporter.h"
#include "Pipeline/SceneManager/SceneManager.h"

#include "Viewer/Menu/Menu.h"
#include "Viewer/Menu/MenuWalker.h"
#include "GameCommand.h"

using namespace std;
using namespace Pipeline;
using namespace Viewer;

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

        Menu menu;
        FillSimulationsMenu(menu, simulationManager);
        MenuWalker menuWalker(&menu);
        userInteractor_->SetMenuWalker(&menuWalker);

        launcher_ = make_unique<Launcher>();
        nextSimulation_ = 0; // Start with first simulation
        while (nextSimulation_ != NO_SIMULATION)
        {
            BaseFactory *factory = simulationManager.GetFactory(nextSimulation_);
            auto simulation = factory->Create();

            nextSimulation_ = NO_SIMULATION;

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

void Game::SetNextSimulation(const uint_t nextSimulation)
{
    nextSimulation_ = nextSimulation;
    launcher_->SetMustStop();
}

void Game::FillSimulationsMenu(Menu &menu, const SimulationManager &simulationManager)
{
    const uint_t factoriesCount = simulationManager.GetFactoriesCount();
    if (factoriesCount > 0)
    {
        const uint_t simsNodeId = menu.AddNode(Menu::ROOT_ID, "Simulations");
        for (uint_t i = 0; i < factoriesCount; i++)
        {
            GameCommandSetNextSimulation *command = new GameCommandSetNextSimulation((uint32)i);
            command->SetReceiver(this);

            const char *title = simulationManager.GetFactory(i)->GetName();
            menu.AddLeaf(simsNodeId, command, title);
        }
    }
}

void Game::Step(float dT)
{
    UNREFERENCED_PARAMETER(dT);
}
