#pragma once

#include <openGL/BufferObjects/VertexArrayObject.h>
#include <openGL/BufferObjects/ElementBufferObject.h>
#include <openGL/BufferObjects/VertexBufferObject.h>
#include <array>
#include <unordered_map>
#include <tuple>

class GeometryCache {
    std::vector<VertexArrayObject> vertexArrays;
    std::vector<VertexBufferObject> vertexBuffers;
    std::vector<ElementBufferObject> elementBuffers;
public:
    enum class Query {
        WIDGET,
        IMAGE_2D,
        CUBE_SOLID, CUBE_WIREFRAME,
        AABB,
        PLANE,
        SPHERE_SMOOTH_10,
        TRIANGLE
    };

    enum Index {
        VERTEX_BUFFER = 0,
        VERTEX_ARRAY = 1,
        ELEMENT_BUFFER = 2
    };
private:
    /**
     * 0: VBO
     * 1: VAO
     * 2: EBO
     */
    std::unordered_map<Query, std::array<size_t, 3>> bufferMapper;

    void create2DWidgetBuffers();
    void createPlaneBuffers();
    void createCubeBuffers();
    void createCircleBuffers();
    void createAABBBuffers();
    void createImage2DBuffers();
    void createTriangleBuffers();
    void createSphere_10Buffers();

    GeometryCache();

    struct PackedBuffers {
        VertexArrayObject& VAO;
        ElementBufferObject& EBO;
        VertexBufferObject& VBO;

        PackedBuffers(VertexArrayObject& VAO, ElementBufferObject& EBO, VertexBufferObject& VBO) : VAO(VAO), EBO(EBO), VBO(VBO) {}
    };

    PackedBuffers qcreateNext(Query q);
public:
    constexpr std::array<float, 32> getPlaneVertices();
    constexpr std::array<unsigned, 6> getPlaneIndices();

    constexpr std::array<float, 192> getCubeVertices();
    constexpr std::array<unsigned, 24> getCubeWireframeIndices();
    constexpr std::array<unsigned, 36> getCubeIndices();

    constexpr std::array<float, 24> getAABBVertices();
    constexpr std::array<unsigned, 24> getAABBIndices();

    constexpr std::array<float, 8> get2DWidgetVertices();

    constexpr std::array<float, 16> getImage2DVertices();
    constexpr std::array<unsigned, 6> getImage2DIndices();

    constexpr std::array<float, 40>  getTriangleVertices();
    constexpr std::array<unsigned, 18> getTriangleIndices();
    /**
     * 0: VBO
     * 1: VAO
     * 2: EBO
     */
    auto query(Query query) const -> std::array<BufferMetadata, 3>;
    auto query(Query query, Index index) const -> BufferMetadata;

    static GeometryCache& inst() {
        static GeometryCache cache;

        return cache;
    }
};

using GCache = GeometryCache;
using Q = GCache::Query;
using I = GCache::Index;

constexpr std::array<float, 32> GeometryCache::getPlaneVertices()
{
    return {
        // Bottom Left
        -0.5f,  0.0f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
    
        // Bottom Right
        0.5f,  0.0f, -0.5f,   0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
    
        // Top Right
        0.5f,  0.0f,  0.5f,   0.0f, 1.0f, 0.0f,  1.0f, 1.0f, 
    
        // Top Left
        -0.5f,  0.0f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f   
    };
}

constexpr std::array<unsigned, 6> GeometryCache::getPlaneIndices()
{
    return {
        0, 1, 2,
        0, 2, 3
    };
}

constexpr std::array<float, 192> GeometryCache::getCubeVertices()
{
    return {
        // FRONT
        0, 0, 0,    0.0,  0.0,  1.0,   0.0, 0.0, 
        0, 1, 0,    0.0,  0.0,  1.0,   0.0, 1.0, 
        1, 0, 0,    0.0,  0.0,  1.0,   1.0, 0.0, 
        1, 1, 0,    0.0,  0.0,  1.0,   1.0, 1.0, 
    
        // BACK
        0,  0,  1,   0.0,  0.0, -1.0,   1.0, 0.0, 
        0,  1,  1,   0.0,  0.0, -1.0,   1.0, 1.0, 
        1,  0,  1,   0.0,  0.0, -1.0,   0.0, 0.0, 
        1,  1,  1,   0.0,  0.0, -1.0,   0.0, 1.0, 
    
        // LEFT
        0,  0,  1,  -1.0,  0.0,  0.0,   0.0, 0.0, 
        0,  1,  1,  -1.0,  0.0,  0.0,   0.0, 1.0, 
        0,  0,  0,  -1.0,  0.0,  0.0,   1.0, 0.0, 
        0,  1,  0,  -1.0,  0.0,  0.0,   1.0, 1.0, 
    
        // RIGHT
        1,  0,  1,   1.0,  0.0,  0.0,   1.0, 0.0, 
        1,  1,  1,   1.0,  0.0,  0.0,   1.0, 1.0, 
        1,  0,  0,   1.0,  0.0,  0.0,   0.0, 0.0, 
        1,  1,  0,   1.0,  0.0,  0.0,   0.0, 1.0, 
    
        // TOP
        0,  1,  0,   0.0,  1.0,  0.0,   0.0, 1.0, 
        0,  1,  1,   0.0,  1.0,  0.0,   0.0, 0.0, 
        1,  1,  0,   0.0,  1.0,  0.0,   1.0, 1.0, 
        1,  1,  1,   0.0,  1.0,  0.0,   1.0, 0.0, 
    
        // BOTTOM
        0,  0,  0,   0.0, -1.0,  0.0,   0.0, 1.0, 
        0,  0,  1,   0.0, -1.0,  0.0,   0.0, 0.0, 
        1,  0,  0,   0.0, -1.0,  0.0,   1.0, 1.0, 
        1,  0,  1,   0.0, -1.0,  0.0,   1.0, 0.0
    };
}

constexpr std::array<unsigned, 36>  GeometryCache::getCubeIndices()
{
    return {
        // FRON
    
        0,  3,  2,    1,  3,  0,
        // BACK
        6,  7,  4,    4,  7,  5,
    
        // LEFT
        8, 11, 10,    9, 11,  8,
    
        // RIGHT
        14, 15, 12,   12, 15, 13,
    
        // TOP
        16, 19, 18,   17, 19, 16,
    
        // BOTTOM
        22, 23, 20,   20, 23, 21,
    };
}

constexpr std::array<unsigned, 24> GeometryCache::getCubeWireframeIndices()
{
    return {
        0, 1, 1, 2, 2, 3, 3, 0,

        // Top face
        4, 5, 5, 6, 6, 7, 7, 4,
    
        // Vertical edges
        0, 4, 1, 5, 2, 6, 3, 7
    };
}

constexpr std::array<float, 24> GeometryCache::getAABBVertices()
{
    return {
        // Bottom-left front
        -1.0f, -1.0f, -1.0f, // Vertex 0
        1.0f, -1.0f, -1.0f,  // Vertex 1
        -1.0f,  1.0f, -1.0f, // Vertex 2
        1.0f,  1.0f, -1.0f,  // Vertex 3

        // Bottom-left back
        -1.0f, -1.0f,  1.0f, // Vertex 4
        1.0f, -1.0f,  1.0f,  // Vertex 5
        -1.0f,  1.0f,  1.0f, // Vertex 6
        1.0f,  1.0f,  1.0f   // Vertex 7
    };
}

constexpr std::array<unsigned, 24> GeometryCache::getAABBIndices()
{
    return {
        0, 1,
        1, 3,
        2, 3,
        0, 2,
    
        4, 5,
        4, 6,
        6, 7,
        7, 5,
    
        0, 4,
        1, 5,
        2, 6,
        3, 7
    };
}

 constexpr std::array<float, 8> GeometryCache::get2DWidgetVertices()
{
    return {
        0.0f, 0.0f,  // bottom-left
        1, 0.0f,     // bottom-right
        1, 1,        // top right
        0.0f, 1      // top left
    };
}

constexpr std::array<float, 16> GeometryCache::getImage2DVertices() {
    return {
        -1.0f, -1.0f,  0.0f, 0.0f,  // Bottom-left
         1.0f, -1.0f,  1.0f, 0.0f,  // Bottom-right
         1.0f,  1.0f,  1.0f, 1.0f,  // Top-right
        -1.0f,  1.0f,  0.0f, 1.0f   // Top-left
    };
}

constexpr std::array<unsigned, 6> GeometryCache::getImage2DIndices() {
    return {
        0, 1, 2,
        2, 3, 0
    };
}

constexpr std::array<float, 40> GeometryCache::getTriangleVertices() {
    return {
        // pos.x, pos.y, pos.z,   norm.x, norm.y, norm.z,   uv.x, uv.y

        // Apex
         0.0f,  1.0f,  0.0f,     0.0f,  1.0f,  0.0f,       0.5f, 1.0f,

        // Base
        -1.0f,  0.0f, -1.0f,     0.0f, -1.0f,  0.0f,       0.0f, 0.0f,
         1.0f,  0.0f, -1.0f,     0.0f, -1.0f,  0.0f,       1.0f, 0.0f,
         1.0f,  0.0f,  1.0f,     0.0f, -1.0f,  0.0f,       1.0f, 1.0f,
        -1.0f,  0.0f,  1.0f,     0.0f, -1.0f,  0.0f,       0.0f, 1.0f
    };
}

inline constexpr std::array<unsigned, 18> GeometryCache::getTriangleIndices()
{
    return {
        // Side triangles (apex to base corners)
        0, 1, 2,   // Front
        0, 2, 3,   // Right
        0, 3, 4,   // Back
        0, 4, 1,   // Left

        // Base (two triangles)
        1, 2, 3,
        1, 3, 4
    };
}