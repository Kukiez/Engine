#include "ShaderCompiler.h"

#include <filesystem>
#include <fstream>
#include <gl/glew.h>
#include <sstream>
#include <regex>

std::unordered_map<std::string, std::string> ShaderCompiler::imports;

void ShaderCompiler::loadImports(const char *file) {
    const std::ifstream fileStream(file);

    if (!file) {
        std::cerr << "Error opening file " << file << std::endl;
        return;
    }

    std::string line;

    static std::regex lookup(R"(export\s+?(.*\s*?)\{\s*([\s\S]*)\s*\})");

    std::stringstream ss;
    ss << fileStream.rdbuf();

    line = ss.str();

    auto begin = std::sregex_iterator(line.begin(), line.end(), lookup);
    auto end = std::sregex_iterator();

    std::cout << "File: " << line << std::endl;
    for (auto it = begin; it != end; ++it) {
        std::smatch match = *it;
        std::string codeInsideBraces = match[2].str();
        std::cout << "Key: " << match[1].str() << std::endl;
        std::cout << "Matched code: " << codeInsideBraces << std::endl;
    }
}

std::string ShaderCompiler::readFile(const std::string &filePath)
{
    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "File not found: " << filePath << std::endl;
        return "";
    }

    std::stringstream buffer;
    std::string line;
    std::string result;
    std::filesystem::path basePath = std::filesystem::path(filePath).parent_path();

    while (std::getline(file, line)) {
        if (line.starts_with("#include")) {
            // Parse the include path
            size_t firstQuote = line.find_first_of("\"<");
            size_t lastQuote  = line.find_last_of("\">");

            if (firstQuote != std::string::npos && lastQuote != std::string::npos && lastQuote > firstQuote) {
                std::string includePath = line.substr(firstQuote + 1, lastQuote - firstQuote - 1);
                std::filesystem::path fullIncludePath = basePath / includePath;
                std::cout << "Reading File: " << fullIncludePath.string() << std::endl;
                result += readFile(fullIncludePath.string());  // Recursive include
            }
        } else {
            result += line + "\n";
        }
    }
    return result;
}

GLuint ShaderCompiler::compileShader(const char *source, const GLenum shaderType)
{
    GLuint shader = glCreateShader(shaderType);
    const char* src = source;
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Failed to Compile: " << source << std::endl;
        std::cerr <<  "ERROR::SHADER::COMPILATION_FAILED\n" + std::string(infoLog);
        std::exit(-1);
        shader = 0;
    }

    return shader;
}

GLuint ShaderCompiler::createShaderProgram(const char *vertex_shader, const char *frag_shader)
{
    const std::string vertexShaderSource = readFile(vertex_shader);
    const std::string fragmentShaderSource = readFile(frag_shader);

    const GLuint vertexShader = compileShader(vertexShaderSource.c_str(), GL_VERTEX_SHADER);
    const GLuint fragmentShader = compileShader(fragmentShaderSource.c_str(), GL_FRAGMENT_SHADER);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::VALIDATION_FAILED\n" + std::string(infoLog);
        shaderProgram = 0;
    } else {
        glValidateProgram(shaderProgram);
        glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
            std::cerr << "ERROR::SHADER::PROGRAM::INVALID_VALUE\n" + std::string(infoLog);
            shaderProgram = 0;
        }
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

GLuint ShaderCompiler::linkComputeShader(GLuint computeShader)
{
    const GLuint program = glCreateProgram();
    glAttachShader(program, computeShader);
    glLinkProgram(program);

    // Check for linking errors
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "Compute Shader Program Linking Failed:\n" << log << std::endl;
        return 0;
    }

    return program;
}

GLuint ShaderCompiler::createComputeShader(const char *src)
{
    const std::string shaderSource = readFile(src);

    return linkComputeShader(compileShader(shaderSource.c_str(), GL_COMPUTE_SHADER));
}

