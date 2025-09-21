
inline MultiDrawArrays::MultiDrawArrays(const int draw_count) : draw_count(draw_count) {
    first_index = new int[draw_count]();
    vertex_counts = new int[draw_count]();
}

inline MultiDrawArrays::MultiDrawArrays(const MultiDrawArrays& other)
    : function(other.function), primitive(other.primitive), draw_count(other.draw_count) {
    first_index = new int[draw_count]();
    vertex_counts = new int[draw_count]();
    memcpy(first_index, other.first_index, sizeof(int) * draw_count);
    memcpy(vertex_counts, other.vertex_counts, sizeof(int) * draw_count);
}

inline MultiDrawArrays& MultiDrawArrays::operator=(const MultiDrawArrays& other) {
    if (this != &other) {
        delete[] first_index;
        delete[] vertex_counts;

        function = other.function;
        draw_count = other.draw_count;
        primitive = other.primitive;

        first_index = new int[draw_count]();
        vertex_counts = new int[draw_count]();
        memcpy(first_index, other.first_index, sizeof(int) * draw_count);
        memcpy(vertex_counts, other.vertex_counts, sizeof(int) * draw_count);
    }
    return *this;
}

inline MultiDrawArrays::MultiDrawArrays(MultiDrawArrays&& other) noexcept
    : function(other.function), primitive(other.primitive), draw_count(other.draw_count),
      first_index(other.first_index), vertex_counts(other.vertex_counts) {
    other.first_index = nullptr;
    other.vertex_counts = nullptr;
    other.draw_count = 0;
}

inline MultiDrawArrays& MultiDrawArrays::operator=(MultiDrawArrays&& other) noexcept {
    if (this != &other) {
        delete[] first_index;
        delete[] vertex_counts;

        function = other.function;
        primitive = other.primitive;
        draw_count = other.draw_count;
        first_index = other.first_index;
        vertex_counts = other.vertex_counts;

        other.first_index = nullptr;
        other.vertex_counts = nullptr;
        other.draw_count = 0;
    }
    return *this;
}

inline MultiDrawArrays::~MultiDrawArrays() {
    delete[] first_index;
    delete[] vertex_counts;
    std::cout << "MultiDraw Array Destroyed";
}



inline MultiDrawArraysIndirect::MultiDrawArraysIndirect(const int draw_count) : draw_count(draw_count) {
    commands = new DrawArraysIndirectCommand[draw_count]();
}

inline MultiDrawArraysIndirect::MultiDrawArraysIndirect(const MultiDrawArraysIndirect& other)
    : function(other.function), primitive(other.primitive), draw_count(other.draw_count) {
    commands = new DrawArraysIndirectCommand[draw_count]();
    memcpy(commands, other.commands, sizeof(DrawArraysIndirectCommand) * draw_count);
}

inline MultiDrawArraysIndirect& MultiDrawArraysIndirect::operator=(const MultiDrawArraysIndirect& other) {
    if (this != &other) {
        delete[] commands;

        function = other.function;
        draw_count = other.draw_count;
        primitive = other.primitive;

        commands = new DrawArraysIndirectCommand[draw_count]();
        memcpy(commands, other.commands, sizeof(DrawArraysIndirectCommand) * draw_count);
    }
    return *this;
}

inline MultiDrawArraysIndirect::MultiDrawArraysIndirect(MultiDrawArraysIndirect&& other) noexcept
    : function(other.function), primitive(other.primitive), draw_count(other.draw_count), commands(other.commands) {
    other.commands = nullptr;
    other.draw_count = 0;
}

inline MultiDrawArraysIndirect& MultiDrawArraysIndirect::operator=(MultiDrawArraysIndirect&& other) noexcept {
    if (this != &other) {
        delete[] commands;

        function = other.function;
        primitive = other.primitive;
        draw_count = other.draw_count;
        commands = other.commands;

        other.commands = nullptr;
        other.draw_count = 0;
    }
    return *this;
}

inline MultiDrawArraysIndirect::~MultiDrawArraysIndirect() {
    delete[] commands;
    std::cout << "MultiDraw Arrays Indirect Destroyed";
}

inline MultiDrawElements::MultiDrawElements(const int draw_count) : draw_count(draw_count) {
    indices_count = new int[draw_count]();
}

inline MultiDrawElements::MultiDrawElements(const MultiDrawElements& other)
    : function(other.function), primitive(other.primitive), type(other.type), draw_count(other.draw_count) {
    indices_count = new int[draw_count]();
    memcpy(indices_count, other.indices_count, sizeof(int) * draw_count);
}

inline MultiDrawElements& MultiDrawElements::operator=(const MultiDrawElements& other) {
    if (this != &other) {
        delete[] indices_count;

        function = other.function;
        draw_count = other.draw_count;
        primitive = other.primitive;
        type = other.type;

        indices_count = new int[draw_count]();
        memcpy(indices_count, other.indices_count, sizeof(int) * draw_count);
    }
    return *this;
}

inline MultiDrawElements::MultiDrawElements(MultiDrawElements&& other) noexcept
    : function(other.function), primitive(other.primitive), type(other.type), draw_count(other.draw_count), indices_count(other.indices_count) {
    other.indices_count = nullptr;
    other.draw_count = 0;
}

inline MultiDrawElements& MultiDrawElements::operator=(MultiDrawElements&& other) noexcept {
    if (this != &other) {
        delete[] indices_count;

        function = other.function;
        primitive = other.primitive;
        type = other.type;
        draw_count = other.draw_count;
        indices_count = other.indices_count;

        other.indices_count = nullptr;
        other.draw_count = 0;
    }
    return *this;
}

inline MultiDrawElements::~MultiDrawElements() {
    delete[] indices_count;
    std::cout << "MultiDraw Destroyed";
}

inline MultiDrawElementsIndirect::MultiDrawElementsIndirect(int draw_count) : draw_count(draw_count) {
    commands = new DrawElementsIndirectCommand[draw_count]();
}

inline MultiDrawElementsIndirect::MultiDrawElementsIndirect(const MultiDrawElementsIndirect& other)
    : function(other.function), primitive(other.primitive), type(other.type), draw_count(other.draw_count) {
    commands = new DrawElementsIndirectCommand[draw_count]();
    memcpy(commands, other.commands, sizeof(DrawElementsIndirectCommand) * draw_count);
}

inline MultiDrawElementsIndirect& MultiDrawElementsIndirect::operator=(const MultiDrawElementsIndirect& other) {
    if (this != &other) {
        delete[] commands;

        function = other.function;
        primitive = other.primitive;
        type = other.type;
        draw_count = other.draw_count;

        commands = new DrawElementsIndirectCommand[draw_count]();
        memcpy(commands, other.commands, sizeof(DrawElementsIndirectCommand) * draw_count);
    }
    return *this;
}

inline MultiDrawElementsIndirect::MultiDrawElementsIndirect(MultiDrawElementsIndirect&& other) noexcept
    : function(other.function), primitive(other.primitive), type(other.type), draw_count(other.draw_count), commands(other.commands) {
    other.commands = nullptr;
    other.draw_count = 0;
}

inline MultiDrawElementsIndirect& MultiDrawElementsIndirect::operator=(MultiDrawElementsIndirect&& other) noexcept {
    if (this != &other) {
        delete[] commands;

        function = other.function;
        primitive = other.primitive;
        type = other.type;
        draw_count = other.draw_count;
        commands = other.commands;

        other.commands = nullptr;
        other.draw_count = 0;
    }
    return *this;
}

inline MultiDrawElementsIndirect::~MultiDrawElementsIndirect() {
    delete[] commands;
    std::cout << "MultiDraw Indirect Destroyed";
}