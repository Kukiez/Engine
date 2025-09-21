#pragma once

struct Skybox : PrimaryComponent, TrackedComponent {
    std::string file;

    Skybox(const std::string_view file) : file(file) {}
};
