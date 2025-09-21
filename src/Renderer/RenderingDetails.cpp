#include "RenderingDetails.h"
#include <gl/glew.h>

void RenderingOperation::draw() const
{
    const auto primitive = static_cast<GLenum>(op.primitive);

    switch (op.function) {
        case GLDrawFunction::ARRAYS:
            glDrawArrays(primitive, op.arrays.first_vertex, op.arrays.vertex_count);
            break;
        case GLDrawFunction::ARRAYS_MULTI:
            glMultiDrawArrays(primitive, op.arrays_multi.first_index, op.arrays_multi.vertex_counts, op.arrays_multi.draw_count);
            break;
        case GLDrawFunction::ARRAYS_INSTANCED:
            glDrawArraysInstanced(primitive, op.arrays_instanced.first_vertex, op.arrays_instanced.vertex_count, op.arrays_instanced.instances);
            break;
        case GLDrawFunction::ARRAYS_INDIRECT:
            glDrawArraysIndirect(primitive, &op.arrays_indirect.command);
            break;
        case GLDrawFunction::ARRAYS_INDIRECT_MULTI:
            glMultiDrawArraysIndirect(primitive, op.arrays_multi_instanced.commands, op.arrays_multi_instanced.draw_count, 0);
            break;
        case GLDrawFunction::ELEMENTS:
            glDrawElements(primitive, op.elements.indices_count, static_cast<GLenum>(op.elements.type), op.elements.indices);
            break;
        case GLDrawFunction::ELEMENTS_MULTI:
            glMultiDrawElements(primitive, op.elements_multi.indices_count, static_cast<GLenum>(op.elements_multi.type), reinterpret_cast<const void* const*>(op.elements_multi.indices), op.elements_multi.draw_count);
            break;
        case GLDrawFunction::ELEMENTS_BASE_VERTEX:
            glDrawElementsBaseVertex(primitive, op.elements_base_vertex.indices_count, static_cast<GLenum>(op.elements_base_vertex.type), op.elements_base_vertex.indices, op.elements_base_vertex.base_vertex);
            break;
        case GLDrawFunction::ELEMENTS_INSTANCED:
            glDrawElementsInstanced(primitive, op.elements_instanced.indices_count, static_cast<GLenum>(op.elements_instanced.type), op.elements_instanced.indices, op.elements_instanced.instances);
            break;
        case GLDrawFunction::ELEMENTS_INDIRECT:
            glDrawElementsIndirect(primitive, static_cast<GLenum>(op.elements_indirect.type), &op.elements_indirect.command);
            break;
        case GLDrawFunction::ELEMENTS_INDIRECT_MULTI:
            glMultiDrawElementsIndirect(primitive, static_cast<GLenum>(op.elements_multi_indirect.type), op.elements_multi_indirect.commands, op.elements_multi_indirect.draw_count, 0);
            break;
        case GLDrawFunction::ARRAYS_INSTANCED_BASE:
            op.arrays_instanced_base.draw();
        default:
            std::cout << "Invalid GLDrawFunction::" << (int)op.function << std::endl;
            break;
    }
}
