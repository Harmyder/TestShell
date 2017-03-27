#pragma once

class ISimulation;

namespace Viewer {
    class UserInteractor;
}

class Launcher
{
public:
    Launcher();

    void Init(Viewer::UserInteractor *userInteractor, ISimulation *simulation);
    void Loop();

    bool IsMustStop() { return isMustStop_; }

private:
    Viewer::UserInteractor *userInteractor_;
    ISimulation *simulation_;

    bool isMustStop_ = false;
};

