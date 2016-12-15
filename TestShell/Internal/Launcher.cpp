#include "stdafx.h"

#include "Internal/Launcher.h"

#include "Internal/BaseSimulation.h"
#include "Viewer/UserInteractor.h"

using namespace Viewer;

Launcher::Launcher() :
    userInteractor_(NULL),
    simulation_(NULL)
{
}

void Launcher::Init(UserInteractor *userInteractor, BaseSimulation *simulation) {
    userInteractor_ = userInteractor;
    simulation_ = simulation;
    isMustStop_ = false;

    simulation_->Init();
}

void Launcher::Loop() {
    const float dT = userInteractor_->GetDeltaTime();
    for (int i = 0 ; !IsMustStop(); ++i)
    {
        simulation_->BeforeStep();
        userInteractor_->BeforeStep();

        simulation_->Step(dT);

        userInteractor_->AfterStep();
        simulation_->AfterStep();

        userInteractor_->BeforeRender();
        userInteractor_->Render();
        userInteractor_->AfterRender();
    }
}

void Launcher::Finish() {
    simulation_->Quit();
}

void Launcher::SetMustStop() {
    isMustStop_ = true;
}

bool Launcher::IsMustStop() {
    return isMustStop_;
}
