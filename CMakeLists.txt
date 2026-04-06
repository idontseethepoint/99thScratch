cmake_minimum_required(VERSION 3.24)

project(MyQtCudaApp LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

# Qt6 setup — must be before find_package
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "C:/Qt/Qt-6.11.0")
set(Qt6_DIR "C:/Qt/Qt-6.11.0/lib/cmake/Qt6")

find_package(Qt6 REQUIRED COMPONENTS 
Core
Widgets
OpenGL
OpenGLWidgets)

find_package(OpenGL REQUIRED)

find_package(CUDAToolkit REQUIRED)


set(KERNEL_SOURCES
    CUDA/mykernels.cu
 )
set(SOURCES
 "main.cpp"
 "Application.cpp"
  
 "Scene3D/AABB.cpp" 
 "Scene3D/CuboidMesh.cpp" 
 "Scene3D/GeometryRenderer.cpp"
 "Scene3D/Material.cpp" 
 "Scene3D/SceneGraphModel.cpp"
 "Scene3D/SceneNode.cpp" 
 "Scene3D/SimulationSceneNode.cpp"
 "Scene3D/MeshRenderer.cpp" 
 "Scene3D/SphereMesh.cpp"
 "Scene3D/vec3D.cpp"
 "Scene3D/ConeMesh.cpp"

 "Sim/LJ_Simulation.cpp" 

 "Widgets/CollapsibleSection.cpp"
 "Widgets/LJ_AtomsOptions.cpp"
 "Widgets/MainWindow.cpp" 
 "Widgets/SceneTree.cpp" 
 "Widgets/SceneWidget.cpp"
 "Widgets/SimulationControls.cpp"
 "Widgets/VecDisplay.cpp" 
 "Widgets/SimulationDisplay.cpp"
 )

   
set(QRC_SOURCES
    assets/assets.qrc
    qml.qrc
    shaders/shaders.qrc
)

include_directories("./" "CUDA" "Widgets" "Scene3D" "Sim")

add_executable(MyQtCudaApp  ${KERNEL_SOURCES} ${SOURCES} ${QRC_SOURCES} "Widgets/SceneWidget.cpp")

target_compile_definitions(MyQtCudaApp PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX          # also prevents min/max macro conflicts with std::
    VC_EXTRA_LEAN     # strips even more from windows.h
)
target_compile_options(MyQtCudaApp PRIVATE
    $<$<COMPILE_LANGUAGE:CUDA>:-Xcompiler=/Zc:__cplusplus>
)

qt_add_resources(MyQtCudaApp qml.qrc)
message(STATUS "Added qml.qrc resource")
message(STATUS "Targets: ${CMAKE_PROJECT_NAME}")

set_target_properties(${PROJECT_NAME} PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
    CUDA_ARCHITECTURES "89"
)

target_link_libraries(MyQtCudaApp
    PRIVATE
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    OpenGL::GL
    CUDA::cudart
    CUDA::cuda_driver 
)

if(WIN32)
    add_custom_command(TARGET MyQtCudaApp POST_BUILD
        COMMAND $<TARGET_FILE:Qt6::windeployqt>
            --plugindir C:/Qt/Qt-6.11.0/plugins
            --qmldir C:/Qt/Qt-6.11.0/qml
            $<TARGET_FILE:MyQtCudaApp>
        COMMENT "Deploying Qt dependencies..."
    )
endif()