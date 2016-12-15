#pragma once

class BaseSimulation;

namespace Viewer {
    class UserInteractor;
}

class Launcher
{
public:
    Launcher();

    void Simulate(const int factoryId);
    void ChangeSimulation(const int factoryId);

public:
    void Init(Viewer::UserInteractor *userInteractor, BaseSimulation *simulation);
    void Loop();
    void Finish();

    void SetMustStop();

private:
    bool IsMustStop();

private:
    Viewer::UserInteractor *userInteractor_;
    BaseSimulation *simulation_;

    bool isMustStop_;
};

