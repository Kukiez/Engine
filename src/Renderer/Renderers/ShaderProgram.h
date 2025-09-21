#pragma once


class ShaderProgram {
public:
    struct BufferBinding {

    };
private:
    std::string filepath;

    std::vector<BufferBinding> bindings;
public:
    ShaderProgram(std::string filepath);
};