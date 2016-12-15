#include "stdafx.h"

#include "Importer/FBXImporter.h"

#include "3rdParty/YolaFbxImporter/YolaFbxImporter/YolaFbxImporter.h"

#ifdef _DEBUG
#if _WIN64 
#define IMPORTER_NAME "YolaFBXImporter_x64-Debug.dll"
#else
#define IMPORTER_NAME "YolaFBXImporter_Win32-Debug.dll"
#endif
#else
#if _WIN64 
#define IMPORTER_NAME "YolaFBXImporter_x64-Release.dll"
#else
#define IMPORTER_NAME "YolaFBXImporter_Win32-Release.dll"
#endif
#endif

namespace Pipeline
{
    DEFINE_SINGLETON(FbxImporter);

    void FbxImporter::Init()
    {
        scene_s = new std::map<const char*, const FBX::Scene*>;
        m_Library = ::LoadLibrary(IMPORTER_NAME);
    }

    void FbxImporter::Close()
    {
        DeleteScenes();

        if (m_Library)
        {
            ::FreeLibrary(m_Library);
            m_Library = NULL;
        }
    }

    FbxImporter::~FbxImporter()
    {
        assert(scene_s == NULL);
    }

    const FBX::Scene* FbxImporter::GetScene(const char *filename)
    {
        pfnLoadFbxFile *LoadFbxFile = (pfnLoadFbxFile*)::GetProcAddress(m_Library, "LoadFbxFile");

        if (scene_s->find(filename) == scene_s->end())
        {
            const FBX::Scene *scene = LoadFbxFile(filename);
            (*scene_s)[filename] = scene;
        }

        return (*scene_s)[filename];
    }

    void FbxImporter::DeleteScenes()
    {
        pfnFreeScene *FreeScene = (pfnFreeScene*)::GetProcAddress(m_Library, "FreeScene");

        for (auto i = scene_s->begin(); i != scene_s->end(); ++i)
        {
            FreeScene(i->second);
        }
        delete scene_s;
        scene_s = NULL;
    }
}
