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

find_package(Qt6 REQUIRED COMPONENTS 
Core
Widgets
OpenGL
OpenGLWidgets)
qt_standard_project_setup()

find_package(OpenGL REQUIRED)

find_package(CUDAToolkit REQUIRED)


set(KERNEL_SOURCES
    CUDA/mykernels.cu
    CUDA/EM.cu
 )
set(SOURCES
 "main.cpp"
 "Application.cpp"
  
 "Scene3D/AABB.cpp" 
 "Scene3D/ArrowMesh.cpp"
 "Scene3D/ConeMesh.cpp"
 "Scene3D/CuboidMesh.cpp" 
 "Scene3D/CylinderMesh.cpp"
 "Scene3D/GeometryRenderer.cpp"
 "Scene3D/Material.cpp" 
 "Scene3D/MeshRenderer.cpp" 
 "Scene3D/SceneGraphModel.cpp"
 "Scene3D/SceneNode.cpp" 
 "Scene3D/SimulationSceneNode.cpp"
 "Scene3D/SphereMesh.cpp"
 "Scene3D/vec3D.cpp"
 "Scene3D/VectorFieldRenderer.cpp"
 "Scene3D/VectorField.cpp"

 "Sim/LJ_Simulation.cpp" 
 "Sim/EM_Simulation.cpp" 

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

message(STATUS "QT_DEPLOY_PLUGINS_DIR: ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/Qt6/plugins")
message(STATUS "Destination: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

# Copy plugins and runtime dependencies after build
add_custom_command(TARGET MyQtCudaApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory(${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/platforms)
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/$<$<CONFIG:Debug>:debug/>/Qt6/plugins/platforms/qwindows$<$<CONFIG:Debug>:d>.dll"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/platforms/"

)