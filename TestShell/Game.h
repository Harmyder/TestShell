#ifndef __GAME_H__
#define __GAME_H__

#include <Windows.h>

namespace Viewer {
    class UserInteractor;
    class Menu;
}
class Launcher;
class SimulationManager;

class Game
{
public:
    Game();
    ~Game();

    int Run(HINSTANCE hInstance);

    void SetNextSimulation(const uint_t nextSimulation);
    void FillSimulationsMenu(Viewer::Menu &menu, const SimulationManager &simulationManager);

private:
    void Step(float dT);

private:
    std::unique_ptr<Viewer::UserInteractor> userInteractor_;
    std::unique_ptr<Launcher> launcher_;

    enum { NO_SIMULATION = -1 };
    int_t nextSimulation_;
};

#endif // __GAME_H__
