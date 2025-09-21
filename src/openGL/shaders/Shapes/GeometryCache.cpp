#include "GeometryCache.h"

#include <Icosphere.h>

GeometryCache::PackedBuffers GeometryCache::qcreateNext(GeometryCache::Query q)
{
    auto& VAO = vertexArrays.emplace_back();
    auto& VBO = vertexBuffers.emplace_back();
    auto& EBO = elementBuffers.emplace_back();

    std::array<size_t, 3> maps = {
        vertexBuffers.size() - 1,
        vertexArrays.size() - 1,
        elementBuffers.size() - 1
    };

    bufferMapper[q] = maps;
    return GeometryCache::PackedBuffers(VAO, EBO, VBO);
}

void GeometryCache::createPlaneBuffers()
{
    auto& VAO = vertexArrays.emplace_back();
    auto& VBO = vertexBuffers.emplace_back();
    auto& EBO = elementBuffers.emplace_back();

    std::array<size_t, 3> maps = {
        vertexBuffers.size() - 1,
        vertexArrays.size() - 1,
        elementBuffers.size() - 1
    };

    bufferMapper[GeometryCache::Query::PLANE] = maps;

    VAO.allocate();

    auto vertices = getPlaneVertices();
    VBO.allocate(vertices.size() * sizeof(float), BufferUsage::STATIC);
    VBO.uploadData(vertices.data());

    auto indices = getPlaneIndices();
    EBO.allocate(indices.size(), sizeof(unsigned), BufferUsage::STATIC);
    EBO.uploadData(indices.data());

    constexpr int stride = 8;

    VAO.addAttribute(3, stride, 0);
    VAO.addAttribute(3, stride, 3);
    VAO.addAttribute(2, stride, 6);
}

void GeometryCache::createCubeBuffers()
{
    auto& VAO = vertexArrays.emplace_back();
    auto& VBO = vertexBuffers.emplace_back();
    auto& EBO = elementBuffers.emplace_back();

    std::array<size_t, 3> maps = {
        vertexBuffers.size() - 1,
        vertexArrays.size() - 1,
        elementBuffers.size() - 1
    };

    bufferMapper[GeometryCache::Query::CUBE_SOLID] = maps;

    VAO.allocate();

    auto vertices = getCubeVertices();
    auto indices = getCubeIndices();
    
    VBO.allocate(vertices.size() * sizeof(float), BufferUsage::STATIC);
    VBO.uploadData(vertices.data());

    EBO.allocate(indices.size(), sizeof(unsigned), BufferUsage::STATIC);
    EBO.uploadData(indices.data());

    constexpr int stride = 8;

    VAO.addAttribute(3, stride, 0);
    VAO.addAttribute(3, stride, 3);
    VAO.addAttribute(2, stride, 6);

    /**
     * Wireframe Cube VAO/EBO
     */

    auto& wVAO = vertexArrays.emplace_back();
    auto& wEBO = elementBuffers.emplace_back();

    auto wIndices = getCubeWireframeIndices();

    wVAO.allocate();
    wVAO.addAttribute(3, stride, 0);
    wVAO.addAttribute(3, stride, 3);
    wVAO.addAttribute(2, stride, 6);

    wEBO.allocate(wIndices.size(), sizeof(unsigned), BufferUsage::STATIC);
    wEBO.uploadData(wIndices.data());

    maps[1] = vertexArrays.size() - 1;
    maps[2] = elementBuffers.size() - 1;

    bufferMapper[GeometryCache::Query::CUBE_WIREFRAME] = maps;
}

void GeometryCache::createTriangleBuffers()
{
    auto& VAO = vertexArrays.emplace_back();
    auto& VBO = vertexBuffers.emplace_back();
    auto& EBO = elementBuffers.emplace_back();

    std::array<size_t, 3> maps = {
        vertexBuffers.size() - 1,
        vertexArrays.size() - 1,
        elementBuffers.size() - 1
    };

    bufferMapper[GeometryCache::Query::TRIANGLE] = maps;

    VAO.allocate();

    auto vertices = getTriangleVertices();
    auto indices = getTriangleIndices();

    VBO.allocate(vertices.size() * sizeof(float), BufferUsage::STATIC);
    VBO.uploadData(vertices.data());

    EBO.allocate(indices.size(), sizeof(unsigned), BufferUsage::STATIC);
    EBO.uploadData(indices.data());
    constexpr int stride = 8;
    VAO.addAttribute(3, stride, 0);
    VAO.addAttribute(3, stride, 3);
    VAO.addAttribute(2, stride, 6);
}

void GeometryCache::create2DWidgetBuffers()
{
    auto& VAO = vertexArrays.emplace_back();
    auto& VBO = vertexBuffers.emplace_back();
    auto& EBO = elementBuffers.emplace_back();

    const std::array maps = {
        vertexBuffers.size() - 1,
        vertexArrays.size() - 1,
        elementBuffers.size() - 1
    };

    bufferMapper[Query::WIDGET] = maps;

    VAO.allocate();

    const auto vertices = get2DWidgetVertices();

    VBO.allocate(vertices.size() * sizeof(float), BufferUsage::STATIC);
    VBO.uploadData(vertices.data());

    VAO.addAttribute(2, 2, 0);
}

void GeometryCache::createCircleBuffers()
{

}

void GeometryCache::createAABBBuffers()
{
    auto& VAO = vertexArrays.emplace_back();
    auto& VBO = vertexBuffers.emplace_back();
    auto& EBO = elementBuffers.emplace_back();

    std::array<size_t, 3> maps = {
        vertexBuffers.size() - 1,
        vertexArrays.size() - 1,
        elementBuffers.size() - 1
    };

    bufferMapper[GeometryCache::Query::AABB] = maps;

    VAO.allocate();

    auto vertices = getAABBVertices();
    auto indices = getAABBIndices();
    
    VBO.allocate(vertices.size() * sizeof(float), BufferUsage::STATIC);
    VBO.uploadData(vertices.data());

    EBO.allocate(indices.size(), sizeof(unsigned), BufferUsage::STATIC);
    EBO.uploadData(indices.data());

    VAO.addAttribute(3, 3, 0);
}

void GeometryCache::createImage2DBuffers() {
    auto& VAO = vertexArrays.emplace_back();
    auto& VBO = vertexBuffers.emplace_back();
    auto& EBO = elementBuffers.emplace_back();

    std::array<size_t, 3> maps = {
        vertexBuffers.size() - 1,
        vertexArrays.size() - 1,
        elementBuffers.size() - 1
    };

    bufferMapper[GeometryCache::Query::IMAGE_2D] = maps;

    VAO.allocate();

    const auto vertices = getImage2DVertices();
    const auto indices = getImage2DIndices();

    VBO.allocate(vertices.size() * sizeof(float), BufferUsage::STATIC);
    VBO.uploadData(vertices.data());

    EBO.allocate(indices.size(), sizeof(unsigned), BufferUsage::STATIC);
    EBO.uploadData(indices.data());

    VAO.addAttribute(2, 4, 0);
    VAO.addAttribute(2, 4, 2);
}

void GeometryCache::createSphere_10Buffers() {
    const Icosphere icosphere(1, 1, true);

    auto& VAO = vertexArrays.emplace_back();
    auto& VBO = vertexBuffers.emplace_back();
    auto& EBO = elementBuffers.emplace_back();

    const std::array maps = {
        vertexBuffers.size() - 1,
        vertexArrays.size() - 1,
        elementBuffers.size() - 1
    };
    icosphere.createBuffers(VAO, VBO, EBO);
    bufferMapper[Q::SPHERE_SMOOTH_10] = maps;
}


GeometryCache::GeometryCache()
{
    createPlaneBuffers();
    createCubeBuffers();
    createAABBBuffers();
    create2DWidgetBuffers();
    createImage2DBuffers();
    createTriangleBuffers();
    createSphere_10Buffers();
    
}

auto GeometryCache::query(GeometryCache::Query query) const -> std::array<BufferMetadata, 3> 
{
    std::array<size_t, 3> result = bufferMapper.find(query)->second;

    return {
        vertexBuffers[result[0]].metadata(0),
        vertexArrays[result[1]].metadata(0),
        elementBuffers[result[2]].metadata(0)
    };
}

auto GeometryCache::query(Query query, Index index) const -> BufferMetadata
{
    return this->query(query)[(int)index];
}