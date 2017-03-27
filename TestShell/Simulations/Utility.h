#pragma once
#include "Viewer\Viewport.h"

namespace Pipeline
{
    class UserMesh;
    class UserCollider;
    class UserScene;
}
namespace Viewer
{
    struct Material;
}

Viewer::RenderItemsDescriptions BuildDescsFromScene(const Pipeline::UserScene& scene, Viewer::Material matRigid, Viewer::Material matCollider);
