// **************************************************************************************
// File: UserSceneFactory.cpp
// **************************************************************************************

#include "stdafx.h"

#include "UserLevel/Factory/UserSceneFactory.h"

#include "InputLevel/Mesh/InputMesh.h"
#include "InputLevel/Scene/InputScene.h"
#include "UserLevel/Mesh/UserMesh.h"
#include "UserLevel/Scene/UserScene.h"
#include "UserLevel/Mesh/Algorithm/UserMeshFromInputMesh.h"
#include "Viewer\Viewport.h"

using namespace std;

namespace Pipeline
{
    void UserSceneFactory::Build(UserScene &userScene, const InputScene &inputScene)
    {
        const uint_t meshesCount = inputScene.GetMeshesCount();
        auto& viewport = userScene.GetViewport();
        for (uint_t i = 0; i < meshesCount; i++) {
            const InputMesh &inputMesh = inputScene.GetMesh(i);
            UserMesh *mesh = new UserMesh(inputMesh);
            UserMeshFromInputMesh restorer(*mesh);
            restorer.Restore();
            userScene.AddMesh(mesh);
        }

        vector<Viewer::Viewport::RenderItemDesc> descs;
        descs.reserve(meshesCount);
        for (uint_t i = 0; i < meshesCount; i++) {
            const auto& mesh = userScene.GetMesh(i);
            const auto& mg = mesh.GetGeometry();

            descs.emplace_back(mesh.GetName(), mg.Vertices, mesh.GetTransform());
        }
        viewport.CreateRenderItem(descs);
    }
}
