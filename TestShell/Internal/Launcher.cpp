#include "stdafx.h"

#include "Internal/Launcher.h"

#include "Internal/ISimulation.h"
#include "Viewer/UserInteractor.h"

using namespace Viewer;

Launcher::Launcher() :
    userInteractor_(NULL),
    simulation_(NULL)
{
}

void Launcher::Init(UserInteractor *userInteractor, ISimulation *simulation) {
    userInteractor_ = userInteractor;
    simulation_ = simulation;
    isMustStop_ = false;
}

void Launcher::Loop() {
    const float dT = userInteractor_->GetDeltaTime();
    while(!IsMustStop() && simulation_->IsOngoing())
    {
        simulation_->BeforeStep();
        userInteractor_->BeforeStep();

        simulation_->Step(dT);

        isMustStop_ = userInteractor_->AfterStep();
        simulation_->AfterStep();

        userInteractor_->BeforeRender();
        userInteractor_->Render();
        userInteractor_->AfterRender();
    }
}
