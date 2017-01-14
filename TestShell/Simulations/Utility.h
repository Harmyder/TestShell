#pragma once
#include "Viewer\Viewport.h"

namespace Pipeline
{
    class UserMesh;
    class UserCollider;
    class UserScene;
}

Viewer::RenderItemVerticesDesc BuildDescFromMesh(const Pipeline::UserMesh& mesh);

Viewer::RenderItemTypeDesc BuildDescFromCollider(const Pipeline::UserCollider& collider);

Viewer::RenderItemsDescriptions BuildDescsFromScene(const Pipeline::UserScene& scene);
