#include "UniformData.h"

#include <gl/glew.h>
#include <openGL/shaders/Shader.h>


void UniformData::upload(const char* key, const Uniform& uniform)
{
    const int location = glGetUniformLocation(programID, key);
    const auto data = static_cast<const float*>(uniform.raw());

    const int amount = uniform.amount();

    switch (uniform.activeType()) {
        case Uniform::Type::INTEGER:
            glUniform1iv(location, amount, static_cast<const int*>(uniform.raw()));
            break;
        case Uniform::Type::FLOAT:
            glUniform1fv(location, amount, data);
            break;
        case Uniform::Type::VEC2: 
            glUniform2fv(location, amount, data);
            break;                    
        case Uniform::Type::VEC3: 
            glUniform3fv(location, amount, data);
            break;                    
        case Uniform::Type::VEC4: 
            glUniform4fv(location, amount, data);
            break;                    
        case Uniform::Type::MAT4: 
            glUniformMatrix4fv(location, amount, GL_FALSE, data);
            break;                    
        default: break;
    }

    if (strcmp(key, "model") == 0) {
      //  uniform.print();
    }
}
