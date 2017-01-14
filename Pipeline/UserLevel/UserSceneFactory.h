#pragma once

#include "Pile/Attribute/NonCopyable.h"

namespace Pipeline
{
    class InputScene;
    class UserScene;

    class UserSceneFactory : public Pile::NonCopyable
    {
    public:
        void BuildScene(UserScene &userScene, const InputScene &inputScene);
    };
}
