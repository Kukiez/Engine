#pragma once

#include <openGL/DataTypeEnum.h>
#include <iostream>
#include <cassert>
#include <vector>
#include <memory>

template<typename T>
inline constexpr bool always_false = false;

enum class RenderPriority {
    UI,             
    OpaqueObjects,  
    TransparentObjects, 
    Shadows,        
    Reflections,    
    Background,     
    PostProcessing  
};

enum class GLDrawFunction {
    ARRAYS,
    ARRAYS_MULTI,
    ARRAYS_INSTANCED,
    ARRAYS_INSTANCED_BASE,
    ARRAYS_INDIRECT,
    ARRAYS_INDIRECT_MULTI,
    ELEMENTS,
    ELEMENTS_RANGE,
    ELEMENTS_MULTI,
    ELEMENTS_BASE_VERTEX,
    ELEMENTS_INSTANCED,
    ELEMENTS_INDIRECT,
    ELEMENTS_INDIRECT_MULTI
};

enum class GLDrawPrimitive {
    POINTS = 0x0000,
    LINES = 0x0001,
    LINES_STRIP = 0x0003,     // connected sequence of lines
    LINES_LOOP = 0x0002,       // connected sequence of lines, last vertex connects back to first
    TRIANGLES = 0x0004, 
    TRIANGLES_STRIP = 0x0005, // connected sequence of triangles
    TRIANGLES_FAN = 0x0006,   // connected triangles that share a single vertex
    PATCHES = 0x000E
};

struct DrawArraysIndirectCommand {
    unsigned count;         // number of indices to draw
    unsigned instanceCount; // number of instances to draw
    unsigned first;
    unsigned baseInstance;
};

struct DrawElementsIndirectCommand {
    unsigned count;         // number of indices to draw
    unsigned instanceCount; // number of instances to draw
    unsigned firstIndex;    // first index to draw
    unsigned baseVertex;    // base vertex for indices
    unsigned baseInstance;  // base instance for instancing
};

struct DispatchIndirectCommand {
    unsigned workgroups_x;
    unsigned workgroups_y;
    unsigned workgroups_z;
};


struct DrawArrays {
    GLDrawFunction function = GLDrawFunction::ARRAYS;
    GLDrawPrimitive primitive;
    int first_vertex;       // the index of the first vertex to draw
    int vertex_count;       // the number of vertices to draw

    void draw() const {
        glDrawArrays(static_cast<GLenum>(primitive), first_vertex, vertex_count);
    }
};

struct DrawArraysInstanced {
    GLDrawFunction function = GLDrawFunction::ARRAYS_INSTANCED;
    GLDrawPrimitive primitive;
    int first_vertex;           // the index of the first vertex to draw
    int vertex_count;           // the index of the first vertex to draw
    int instances;              // number of times to draw

    void draw() const {
        glDrawArraysInstanced(static_cast<GLenum>(primitive), first_vertex, vertex_count, instances);
    }
};

/**
 * Represents a draw call for instanced rendering with a base instance.
 *
 * \param primitive The type of primitive to render (e.g., POINTS).
 * \param first_vertex The index of the first vertex to draw.
 * \param vertex_count The number of vertices to draw.
 * \param instances The number of instances to render.
 * \param first_instance The starting instance ID for instanced rendering.
 */
struct DrawArraysInstancedBase {
    GLDrawFunction function = GLDrawFunction::ARRAYS_INSTANCED_BASE;
    GLDrawPrimitive primitive;
    int first_vertex;
    int vertex_count;
    int instances;
    int first_instance;

    void draw() const {
        glDrawArraysInstancedBaseInstance(static_cast<GLenum>(primitive), first_vertex, vertex_count, instances, first_instance);
    }
};

struct DrawArraysIndirect {
    GLDrawFunction function = GLDrawFunction::ARRAYS_INDIRECT;
    GLDrawPrimitive primitive;
    DrawArraysIndirectCommand command;
};

struct MultiDrawArrays {
    GLDrawFunction function = GLDrawFunction::ARRAYS_MULTI;
    GLDrawPrimitive primitive;
    int* first_index;          // starting index of each draw call
    int* vertex_counts;         // number of vertices of each draw call
    int draw_count;             // number of draw calls

    inline MultiDrawArrays(const int draw_count);
    inline MultiDrawArrays(const MultiDrawArrays& other);
    inline MultiDrawArrays(MultiDrawArrays&& other) noexcept;
    inline MultiDrawArrays& operator=(const MultiDrawArrays& other);
    inline MultiDrawArrays& operator=(MultiDrawArrays&& other) noexcept;
    inline ~MultiDrawArrays();

};

struct MultiDrawArraysIndirect {
    GLDrawFunction function = GLDrawFunction::ARRAYS_INDIRECT_MULTI;
    GLDrawPrimitive primitive;
    DrawArraysIndirectCommand* commands;
    int draw_count;             // number of draw calls

    inline MultiDrawArraysIndirect(const int draw_count);
    inline MultiDrawArraysIndirect(const MultiDrawArraysIndirect& other);
    inline MultiDrawArraysIndirect(MultiDrawArraysIndirect&& other) noexcept;
    inline MultiDrawArraysIndirect& operator=(const MultiDrawArraysIndirect& other);
    inline MultiDrawArraysIndirect& operator=(MultiDrawArraysIndirect&& other) noexcept;
    inline ~MultiDrawArraysIndirect();
};

/**
 * Example usage: glDrawElements(primitive, indices_count, type, indices)
 * primitive: TRIANGLES
 * indices_count: 36
 * type: UNSIGNED_INT
 * indices = 0
 */
struct DrawElements {
    GLDrawFunction function = GLDrawFunction::ELEMENTS;
    GLDrawPrimitive primitive;
    int indices_count;          // number of indices to render
    GLType type;                // type of the index values: [UNSIGNED_INT, UNSIGNED_SHORT, UNSIGNED_BYTE]
    const unsigned* indices;    // pointer to the location of the indices or offset in the EBO
    
    DrawElements() : indices(nullptr) {}

    void draw() const {
        glDrawElements(static_cast<GLenum>(primitive), indices_count, static_cast<GLenum>(type), indices);
    }
};


struct DrawElementsRange {
    GLDrawFunction function = GLDrawFunction::ELEMENTS_RANGE;
    GLDrawPrimitive primitive;
    int min_index;              // indices will be clamped to range [min_index, max_index]
    int max_index;              // indices will be clamped to range [min_index, max_index]
    int indices_count;          // number of indices to render
    GLType type;                // type of the index values: [UNSIGNED_INT, UNSIGNED_SHORT, UNSIGNED_BYTE]
    const unsigned* indices;    // pointer to the location of the indices or offset in the EBO
    
    DrawElementsRange() : indices(nullptr) {}
};

struct DrawElementsBase {
    GLDrawFunction function = GLDrawFunction::ELEMENTS_BASE_VERTEX;
    GLDrawPrimitive primitive;
    int indices_count;          // number of indices to render
    GLType type;                // type of the index values: [UNSIGNED_INT, UNSIGNED_SHORT, UNSIGNED_BYTE]
    unsigned* indices;          // pointer to the location of the indices
    int base_vertex;            // the starting vertex index
    
    DrawElementsBase() : indices(nullptr) {}
};

struct DrawElementsInstanced {
    GLDrawFunction function = GLDrawFunction::ELEMENTS_INSTANCED;
    GLDrawPrimitive primitive;
    int indices_count;          // number of indices to render
    GLType type;                // type of the index values: [UNSIGNED_INT, UNSIGNED_SHORT, UNSIGNED_BYTE]
    const unsigned* indices;    // pointer to the location of the indices or offset in the EBO    
    int instances;              // number of times to draw
    
    DrawElementsInstanced() : indices(nullptr) {}

    void draw() {
        glDrawElementsInstanced((GLenum)primitive, indices_count, (GLenum)type, indices, instances);
    }
};

struct MultiDrawElements {
    GLDrawFunction function = GLDrawFunction::ELEMENTS_MULTI;
    GLDrawPrimitive primitive;
    GLType type;                // type of the index values: [UNSIGNED_INT, UNSIGNED_SHORT, UNSIGNED_BYTE]
    int* indices_count;         // array of number of indices to render each draw call
    const unsigned* indices;    // array of pointers to the indices or offset in the EBO each draw call    
    int draw_count;             // number of draw calls

    inline MultiDrawElements(const int draw_count);
    inline MultiDrawElements(const MultiDrawElements& other);
    inline MultiDrawElements(MultiDrawElements&& other) noexcept;
    inline MultiDrawElements& operator=(const MultiDrawElements& other);
    inline MultiDrawElements& operator=(MultiDrawElements&& other) noexcept;
    inline ~MultiDrawElements();
};

struct DrawElementsIndirect {
    GLDrawFunction function = GLDrawFunction::ELEMENTS_INDIRECT;
    GLDrawPrimitive primitive;
    GLType type;                // type of the index values: [UNSIGNED_INT, UNSIGNED_SHORT, UNSIGNED_BYTE]
    DrawElementsIndirectCommand command;
};


struct MultiDrawElementsIndirect {
    GLDrawFunction function = GLDrawFunction::ELEMENTS_INDIRECT_MULTI;
    GLDrawPrimitive primitive;
    GLType type;                            // type of the index values: [UNSIGNED_INT, UNSIGNED_SHORT, UNSIGNED_BYTE]
    DrawElementsIndirectCommand* commands;
    int draw_count;                         // number of draw calls

    inline MultiDrawElementsIndirect(int draw_count);
    inline MultiDrawElementsIndirect(const MultiDrawElementsIndirect& other);
    inline MultiDrawElementsIndirect(MultiDrawElementsIndirect&& other) noexcept;
    inline MultiDrawElementsIndirect& operator = (const MultiDrawElementsIndirect& other);
    inline MultiDrawElementsIndirect& operator=(MultiDrawElementsIndirect&& other) noexcept;
    inline ~MultiDrawElementsIndirect();
};

#include "RenderingDetails.inl" 

class RenderingOperation {
    union Operation {
        struct {
            GLDrawFunction function;
            GLDrawPrimitive primitive;            
        };
        DrawArrays                  arrays;
        MultiDrawArrays             arrays_multi;
        DrawArraysInstanced         arrays_instanced;
        DrawArraysInstancedBase     arrays_instanced_base;
        DrawArraysIndirect          arrays_indirect;
        MultiDrawArraysIndirect     arrays_multi_instanced;

        DrawElements                elements;
        MultiDrawElements           elements_multi;
        DrawElementsBase            elements_base_vertex;
        DrawElementsInstanced       elements_instanced;
        DrawElementsIndirect        elements_indirect;
        MultiDrawElementsIndirect   elements_multi_indirect;

        Operation() {}

        ~Operation() {
            switch (function) {
                case GLDrawFunction::ARRAYS_MULTI:
                    arrays_multi.~MultiDrawArrays();
                    break;
                case GLDrawFunction::ARRAYS_INDIRECT_MULTI:
                    arrays_multi_instanced.~MultiDrawArraysIndirect();
                    break;
                case GLDrawFunction::ELEMENTS_MULTI:
                    elements_multi.~MultiDrawElements();
                    break;
                case GLDrawFunction::ELEMENTS_INDIRECT_MULTI:
                    elements_multi_indirect.~MultiDrawElementsIndirect();
                    break;
                default:
                    break;
            }
        }
    };
    Operation op;
public:
    template <typename DrawCommand>
    void setTemplated(DrawCommand&& command) {
        using cmd = std::decay_t<DrawCommand>;

        if constexpr (std::is_same_v<cmd, DrawArrays>) {
            op.arrays = std::forward<DrawCommand>(command);
        } else if constexpr (std::is_same_v<cmd, MultiDrawArrays>) {
            op.arrays_multi = std::forward<DrawCommand>(command);
        } else if constexpr (std::is_same_v<cmd, DrawArraysInstanced>) {
            op.arrays_instanced = std::forward<DrawCommand>(command);
        } else if constexpr (std::is_same_v<cmd, DrawArraysIndirect>) {
            op.arrays_indirect = std::forward<DrawCommand>(command);
        } else if constexpr (std::is_same_v<cmd, MultiDrawArraysIndirect>) {
            op.arrays_multi_instanced = std::forward<DrawCommand>(command);
        } else if constexpr (std::is_same_v<cmd, DrawElements>) {
            op.elements = std::forward<DrawCommand>(command);
        } else if constexpr (std::is_same_v<cmd, MultiDrawElements>) {
            op.elements_multi = std::forward<DrawCommand>(command);
        } else if constexpr (std::is_same_v<cmd, DrawElementsBase>) {
            op.elements_base_vertex = std::forward<DrawCommand>(command);
        } else if constexpr (std::is_same_v<cmd, DrawElementsInstanced>) {
            op.elements_instanced = std::forward<DrawCommand>(command);
        } else if constexpr (std::is_same_v<cmd, DrawElementsIndirect>) {
            op.elements_indirect = std::forward<DrawCommand>(command);
        } else if constexpr (std::is_same_v<cmd, MultiDrawElementsIndirect>) {
            op.elements_multi_indirect = std::forward<DrawCommand>(command);
        } else {
                assert(false && "No DrawCommand Overload found");
                std::cout << "FUCKING HELL\n\n\n";
                std::exit(1);
        }
    }

    RenderingOperation() = default;

    template <typename DrawCommand>
    RenderingOperation(DrawCommand&& command) {
        setTemplated<DrawCommand>(std::forward<DrawCommand>(command));
    }

    RenderingOperation(const RenderingOperation& other) {
        memcpy(this, &other, sizeof(*this));
    }

    RenderingOperation& operator = (const RenderingOperation& other) {
        if (this != &other) {
            memcpy(this, &other, sizeof(*this));
        }
        return *this;
    }

    RenderingOperation(RenderingOperation&& other) noexcept {
        std::memcpy(&op, &other.op, sizeof(op));
        std::memset(&other.op, 0, sizeof(op));
    }

    RenderingOperation& operator = (RenderingOperation&& other) noexcept {
        if (this != &other) {
            memcpy(&op, &other.op, sizeof(op));
            memset(&other.op, 0, sizeof(op));
        }
        return *this;
    }

    template <typename DrawCommand>
    RenderingOperation& operator =(DrawCommand&& command) {
        setTemplated<DrawCommand>(std::forward<DrawCommand>(command));

        return *this;
    }

    void draw() const;
};



struct RenderPacket {
    struct Binding {
        unsigned buffer;
        unsigned location;
    };

    RenderingOperation draw_call;
    std::string shader;
    unsigned vao_binding;
    std::vector<Binding> uniform_bindings;
    std::vector<Binding> ssbo_bindings;
};