#pragma once

    #ifdef DLLEXPORT
    #define DLL_FUNC _declspec(dllexport)
    #else // DLLEXPORT
    #define DLL_FUNC _declspec(dllimport)
    #endif // !DLLEXPORT

namespace FBX 
{

    struct Element
    {
        enum Type { NODE, MESH, COLLIDER };

        static const Type TYPE = NODE;
        Type m_Type;
    };

    struct Vector2
    {
        float p[2];
    };
    struct Vector4
    {
        float p[4];
    };
    struct Matrix44
    {
        float p[4][4];
    };

    struct Triangle
    {
        unsigned int v[3];
        unsigned int n[3];
        unsigned int uv[3];
    };

    struct Mesh : Element
    {
        static const Type TYPE = MESH;

        int       verticesCount;
        Vector4  *vertices;

        int       normalsCount;
        Vector4  *normals;

        int       uvsCount;
        Vector2  *uvs;

        int       trianglesCount;
        Triangle *triangles;
    };

    struct Collider : Element
    {
        static const Type TYPE = COLLIDER;
    };

    struct Box : Collider
    {
        float length;
        float width;
        float height;
    };

    struct Sphere : Collider
    {
        float radius;
    };

    struct Capsule : Collider
    {
        float radius;
        float height;
    };

    struct Node
    {
        Node* parent;

        unsigned int childrenCount;
        Node **childrenList;

        char *name;

        Matrix44 transform;

        Element *element;
    };

    struct Scene
    {
        enum Axis
        {
            AXIS_X, AXIS_Y, AXIS_Z
        };

        Node *rootNode;

        int upVectorSign;
        Axis upVector;
        float scaleFactor;
    };

}

typedef DLL_FUNC const FBX::Scene * (pfnLoadFbxFile)(const char *fbxFilename);
typedef DLL_FUNC void (pfnFreeScene)(const FBX::Scene *scene);
extern "C" DLL_FUNC const FBX::Scene *LoadFbxFile(const char *fbxFilename);
extern "C" DLL_FUNC void FreeScene(const FBX::Scene *scene);

