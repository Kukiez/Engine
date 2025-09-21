#pragma once
#include <iostream>
#include <unordered_map>

class ShaderCompiler {
public:
    static std::unordered_map<std::string, std::string> imports;

    static void loadImports(const char* file);

    static std::string readFile(const std::string& filePath);

    static unsigned compileShader(const char* source, unsigned shaderType);

    static unsigned createShaderProgram(const char* vertex_shader, const char* frag_shader);

    static unsigned linkComputeShader(unsigned computeShader);

    static unsigned createComputeShader(const char* src);

};
