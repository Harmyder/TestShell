// **************************************************************************************
// Desc: Stores mesh data
// **************************************************************************************
namespace Pipeline
{
    class InputMesh
    {
    public:
        InputMesh();

        void SetName(const char *name);
        const char *GetName() const;

        void SetPositions(std::vector<XMFLOAT4> &positions);
        void SetTrianglesPositions(std::vector<uint16> &triangles);
        void SetNormals(std::vector<XMFLOAT4> &positions);
        void SetTexCoords(std::vector<XMFLOAT2> &texCoords);
        void SetTrianglesTexCoords(std::vector<uint16> &triangles);

        uint_t GetVerticesCount() const;
        const std::vector<XMFLOAT4>& GetPositions() const;
        const std::vector<uint16>&   GetTrianglesPositions() const;
        const std::vector<XMFLOAT4>& GetNormals() const;
        const std::vector<XMFLOAT2>& GetTexCoords() const;
        const std::vector<uint16>&   GetTrianglesTexCoords() const;

        void SetTransform(const XMFLOAT4X4& transform);
        const XMFLOAT4X4& GetTransform() const;

        struct Vertex
        {
            uint16 PositionIndex;
            uint16 NormalIndex;
            uint16 TexCoordIndex;

            bool operator==(const Vertex& other);
        };

        const std::vector<Vertex>& GetVertices() const;
        const std::vector<uint16>& GetTrianglesVertices() const;

    public:
        void ComputeVertices();

    private:
        char name_[128];
        std::vector<XMFLOAT4> positions_;
        std::vector<uint16>   trianglesPositions_;
        std::vector<XMFLOAT4> normals_;
        std::vector<XMFLOAT2> texCoords_;
        std::vector<uint16>   trianglesTexCoords_;

        XMFLOAT4X4 transform_;

        std::vector<Vertex> uniqueVertices_;
        std::vector<uint16> trianglesVertices_;
    };
}
