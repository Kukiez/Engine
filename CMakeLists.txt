cmake_minimum_required(VERSION 3.10)
project(idk VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${PROJECT_SOURCE_DIR}/ext)
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/ext/icosphere)
include_directories(${PROJECT_SOURCE_DIR}/src/Core)
include_directories(${PROJECT_SOURCE_DIR}/src/Creator)
include_directories(${PROJECT_SOURCE_DIR}/ext/openexr)
include_directories(${PROJECT_SOURCE_DIR}/ext/openexr/OpenEXR)
include_directories(${PROJECT_SOURCE_DIR}/ext/openexr/Imath)
include_directories("C:/Main/ECS/test")

file(GLOB SOURCES "src/*.h")

add_executable(main main.cpp ${SOURCES}
    ${CMAKE_SOURCE_DIR}/../test/ECS/Global/Allocator.cpp
    ${CMAKE_SOURCE_DIR}/../test/ECS/ECS.cpp
        src/Core/World/TerrainWorld.cpp
        src/Math/Shapes/geom.cpp
        src/Core/Model/ModelAnimationPlayer.cpp
        src/Core/Model/ModelPose.cpp
        src/Core/Model/Skeleton.cpp
        src/Core/Model/ModelLoaderSystem.cpp
        src/Core/Model/Model.cpp
        src/Core/Model/ModelAnimation.cpp
        src/openGL/Texture/TextureLoader.cpp
        src/openGL/BufferObjects/BufferGeneral.cpp
        src/openGL/shaders/UniformData.cpp
        src/Minecraft/Voxel/VoxelWorld.cpp
        src/Util/Random.cpp
        src/Renderer/Particle/ParticleSystem.h
        src/Renderer/Particle/ParticleSystem.cpp
        src/Renderer/GeometrySystem.h
        src/Renderer/GeometrySystem.cpp
        src/Renderer/Renderers/MaterialSystem.cpp
        src/Renderer/Renderers/Renderer.cpp
        src/openGL/shaders/Shader.cpp
        src/openGL/shaders/ShaderCompiler.cpp
        src/openGL/shaders/ShaderProgramsInitializer.cpp
        src/Renderer/RenderingSystem.cpp
        src/Renderer/Light.h
        src/Core/World/FrustumCulling.h
        src/Renderer/Systems/Commands.h
        src/Core/Player/PlayerController.h
        src/Core/Player/PlayerController.cpp
        src/Core/Collision/MovementSystem.h
        src/Core/Collision/CollisionSystem.cpp
        src/Core/Collision/CollisionSystem.h
        src/Renderer/Systems/SkyboxSystem.h
        src/Renderer/Resource/BufferSystem.h
)

target_compile_options(main PRIVATE /bigobj)

target_include_directories(main PRIVATE ${SDL2_INCLUDE_DIR})
file(GLOB LIBS "${PROJECT_SOURCE_DIR}/lib/*.lib")
target_link_libraries(main PRIVATE ${LIBS})

find_package(OpenGL REQUIRED)
target_link_libraries(main PRIVATE OpenGL::GL)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

file(GLOB DLL_FILES "${CMAKE_SOURCE_DIR}/dll/*")
file(COPY ${DLL_FILES} DESTINATION ${CMAKE_BINARY_DIR})

target_include_directories(main PRIVATE "C:/Program Files (x86)/Intel/oneAPI/tbb/latest/include")
target_link_libraries(main PRIVATE "C:/Program Files (x86)/Intel/oneAPI/tbb/latest/lib/tbb12_debug.lib")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

add_custom_target(copy_shaders ALL
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/shaders
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_BINARY_DIR}/shaders
)
