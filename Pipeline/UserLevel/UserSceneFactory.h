#pragma once

#include "Common/Attribute/NonCopyable.h"

namespace Pipeline
{
    class InputScene;
    class UserScene;

    class UserSceneFactory : public Common::NonCopyable
    {
    public:
        static void BuildScene(UserScene &userScene, const InputScene &inputScene);

    private:
        static void BuildMeshes(UserScene &userScene, const InputScene &inputScene);
        static void BuildColliders(UserScene &userScene, const InputScene &inputScene);
    };
}
