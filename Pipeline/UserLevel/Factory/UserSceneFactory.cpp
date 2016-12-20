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

namespace Pipeline
{
    void UserSceneFactory::Build(UserScene &userScene, const InputScene &inputScene)
    {
        const uint_t meshesCount = inputScene.GetMeshesCount();
        auto& viewport = userScene.GetViewport();
        for (uint_t i = 0; i < meshesCount; i++)
        {
            const InputMesh &inputMesh = inputScene.GetMesh(i);
            UserMesh *mesh = new UserMesh(inputMesh);
            UserMeshFromInputMesh restorer(*mesh);
            restorer.Restore(viewport);
            userScene.AddMesh(mesh);
        }
    }
}
