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

struct RenderItemsDescriptions
{
    Viewer::DescsInstanced Instanced;
    Viewer::DescsVertices Vertices;
    Viewer::DescsTypes Types;
    Viewer::DescsParticles Particles;
};

RenderItemsDescriptions BuildDescsFromScene(const Pipeline::UserScene& scene, Viewer::Material matRigid, Viewer::Material matCollider);
