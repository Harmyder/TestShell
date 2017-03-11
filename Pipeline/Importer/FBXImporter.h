#pragma once

#include "Common/Pattern/Singleton.h"

#include <map>

namespace FBX
{
    struct Scene;
};

namespace Pipeline
{
    class FbxImporter
    {
    public:
        DECLARE_CLASS_SINGLETON(FbxImporter);

    public:
        ~FbxImporter();

        void Init();
        void Close();

        const FBX::Scene* GetScene(const char *filename);

    private:
        void DeleteScenes();

    private:
        HINSTANCE m_Library;

        std::map<const char*, const FBX::Scene*> *scene_s;
    };
}
