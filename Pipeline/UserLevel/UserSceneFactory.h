#pragma once

#include "Pile/Attribute/NonCopyable.h"

namespace Viewer
{
    struct RenderItemsDescriptions;
}

namespace Pipeline
{
    class InputScene;
    class UserScene;

    class UserSceneFactory : public Pile::NonCopyable
    {
    public:
        Viewer::RenderItemsDescriptions Build(UserScene &userScene, const InputScene &inputScene);
    };
}
