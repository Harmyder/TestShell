#pragma once

#include <memory>

#include "Common/Pattern/Singleton.h"

namespace FBX
{
    struct Scene;
    struct Node;
};

namespace DirectX {
    struct XMFLOAT4X4;
    struct XMFLOAT3;
}

namespace Common {
    class Matrix3;
    class OrthogonalTransform;
}

namespace Pipeline
{
    class InputScene;
    struct ColliderFbx;

    class SceneManager
    {
        DECLARE_CLASS_SINGLETON(SceneManager);
        ~SceneManager();

    public:
        void Init();
        void Close();

    public:
        void Load(const std::string& path, const std::string& filetitle);

        InputScene *GetScene();

    private:
        using CollidersFbx = std::vector<ColliderFbx>;
        CollidersFbx LoadColliders(const std::string& filename, const Common::Matrix3& upAxisRotation);
        void ConvertScene(const ::FBX::Scene *fbxScene, float scaleFactor);
        void FillSceneFromNode(const ::FBX::Node *fbxNode, const Common::OrthogonalTransform &transform, float scaleFactor);
        const ColliderFbx* GetColliderFbx(const char* name) const;

    private:
        std::unique_ptr<InputScene> scene_;
        CollidersFbx colliders_;
    };
}
