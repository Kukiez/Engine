#pragma once

#include <GL/glew.h>
#include <iostream>
#include <openGL/DataTypeEnum.h>
#include <openGL/BufferObjects/BufferGeneral.h>

class VertexArrayObject {
    unsigned VAO = 0;
    unsigned next = 0;
public:
    VertexArrayObject() = default;

    VertexArrayObject(const VertexArrayObject& other) = delete;
    VertexArrayObject& operator=(const VertexArrayObject& other) = delete;

    VertexArrayObject(VertexArrayObject&& other) noexcept
    : VAO(other.VAO), next(other.next)
    {
        other.VAO = 0;
    }

    VertexArrayObject& operator=(VertexArrayObject&& other) noexcept {
        if (this != &other) {
            next = other.next;
            VAO = other.VAO;
            other.VAO = 0;
        }

        return *this;
    }

    ~VertexArrayObject() {
        glDeleteVertexArrays(1, &VAO);
    }

    void discard() {
        if (VAO) {
            glDeleteVertexArrays(1, &VAO);
            next = 0;
            VAO = 0;
        }
    }

    void allocate() {
        discard();
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
    }

    /**
     * Adds attributes sequentially, keeps tracks of binding point
     */
    void addAttribute(const uint8_t size, const uint8_t dataPerVertex, const int offset, GLType type = GLType::FLOAT, const size_t elementBytes = sizeof(float)) {
        glVertexAttribPointer(next ++, size, static_cast<int>(type),
            GL_FALSE, static_cast<GLsizei>(dataPerVertex * elementBytes), (void*)(offset * elementBytes));
        glEnableVertexAttribArray(next - 1);
    }

    /**
     * Adds attributes sequentially, keeps tracks of binding point
     */
    void addAttributeInteger(const uint8_t size, const uint8_t dataPerVertex, const int offset) {
        glVertexAttribIPointer(next ++, size, GL_INT, static_cast<GLsizei>(dataPerVertex * sizeof(int)), (void*)(offset * sizeof(int)));
        glEnableVertexAttribArray(next - 1);
    }
    /**
     * Adds attributes sequentially, keeps tracks of binding point
     */
    void addAttribute(const BufferCreateInfo::Attribute& attribute) {
        glVertexAttribPointer(
            attribute.index,
            attribute.size,
            static_cast<GLenum>(attribute.type),
            attribute.normalized,
            static_cast<GLsizei>(attribute.stride * attribute.bytesOfType),
            (void*)(attribute.offset * attribute.bytesOfType)
        );
        glEnableVertexAttribArray(attribute.index);
    }


    void bind() const {
        glBindVertexArray(VAO);
    }

    unsigned id() const {
        return VAO;
    }


    BufferMetadata metadata(unsigned storageID) const {
        return {
            .target = BufferTarget::VERTEX_ARRAY_OBJECT,
            .usage = BufferUsage::STATIC,
            .ID = VAO,
            .storageID = storageID,
            .buffer = { .attributes = next }
        };
    }
};
