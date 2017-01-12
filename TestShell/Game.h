#ifndef __GAME_H__
#define __GAME_H__

#include <Windows.h>

namespace Viewer {
    class UserInteractor;
}
class Launcher;
class SimulationManager;

class Game
{
public:
    Game();
    ~Game();

    int Run(HINSTANCE hInstance);

private:
    void Step(float dT);

private:
    std::unique_ptr<Viewer::UserInteractor> userInteractor_;
    std::unique_ptr<Launcher> launcher_;
};

#endif // __GAME_H__
