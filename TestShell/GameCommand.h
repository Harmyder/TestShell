#pragma once

#include "Pile/Pattern/Command.h"
#include "Game.h"

class GameCommand : public Pile::Command
{
protected:
    Game *GetReceiver() 
    {
        return static_cast<Game*>(m_Receiver);
    } 
};

class GameCommandSetNextSimulation : public GameCommand
{
public:
    GameCommandSetNextSimulation(const uint32 factoryId) : m_FactoryId(factoryId) {}

    virtual void Execute() 
    {
        GetReceiver()->SetNextSimulation(m_FactoryId);
    }

private:
    const uint32 m_FactoryId;
};
