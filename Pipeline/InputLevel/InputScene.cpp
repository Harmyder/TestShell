#include "stdafx.h"

#include "InputLevel/InputScene.h"
#include "InputLevel/InputMesh.h"
#include "InputLevel/InputCollider.h"

using namespace std;

namespace Pipeline
{
    InputScene::InputScene() {}
    InputScene::~InputScene() {}

    // **************************************************************************************
    void InputScene::AddMesh(unique_ptr<InputMesh> mesh)
    {
        meshes_.push_back(move(mesh));
    }

    // **************************************************************************************
    void InputScene::AddCollider(std::unique_ptr<InputCollider> collider) {
        colliders_.push_back(move(collider));
    }
}
