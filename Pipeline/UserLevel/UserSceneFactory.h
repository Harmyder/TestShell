#pragma once

#include "Common/Attribute/NonCopyable.h"

namespace Pipeline
{
    class InputScene;
    class UserScene;

    class UserSceneFactory : public Common::NonCopyable
    {
    public:
        void BuildScene(UserScene &userScene, const InputScene &inputScene);
    };
}
