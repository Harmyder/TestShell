#pragma once

#include <memory>

#include "Pile/Pattern/Singleton.h"

namespace FBX
{
    struct Scene;
    struct Node;
};

namespace DirectX {
    struct XMFLOAT4X4;
}

namespace Pipeline
{
    class InputScene;

    class SceneManager
    {
        DECLARE_CLASS_SINGLETON(SceneManager);
        ~SceneManager();

    public:
        void Init();
        void Close();

    public:
        void Load(const char * filename);

        InputScene *GetScene();

    private:
        void ConvertScene(const ::FBX::Scene *fbxScene, float scaleFactor);
        void FillSceneFromNode(const ::FBX::Node *fbxNode, const DirectX::XMFLOAT4X4 &transform, float scaleFactor);

    private:
        std::unique_ptr<InputScene> scene_;
    };
}
