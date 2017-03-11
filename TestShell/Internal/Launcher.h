#pragma once

class BaseSimulation;

namespace Viewer {
    class UserInteractor;
}

class Launcher
{
public:
    Launcher();

    void Init(Viewer::UserInteractor *userInteractor, BaseSimulation *simulation);
    void Loop();
    void Finish();

    bool IsMustStop() { return isMustStop_; }

private:
    Viewer::UserInteractor *userInteractor_;
    BaseSimulation *simulation_;

    bool isMustStop_ = false;
};

