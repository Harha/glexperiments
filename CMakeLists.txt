# Force minimum CMake version
cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

# Create and init project
project(gldemo CXX)
set(DIR_SRC "${PROJECT_SOURCE_DIR}/src")
set(DIR_INC "${PROJECT_SOURCE_DIR}/inc")
set(DIR_DTA_NAME "glsl")
set(DIR_DTA "${PROJECT_SOURCE_DIR}/${DIR_DTA_NAME}")
set(DIR_BIN "${PROJECT_SOURCE_DIR}/bin/${CMAKE_PROJECT_NAME}.${CMAKE_SYSTEM_NAME}.${CMAKE_GENERATOR}")

# Define used CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/CMakeModules")

# Enable C++ and C
enable_language(CXX)
enable_language(C)

# Include source files
file(GLOB_RECURSE SRC_CPP "${DIR_SRC}/*.cpp")
file(GLOB_RECURSE SRC_C "${DIR_SRC}/*.c")
file(GLOB_RECURSE SRC_HPP "${DIR_SRC}/*.hpp")
file(GLOB_RECURSE SRC_H "${DIR_SRC}/*.h")
file(GLOB_RECURSE SRC_ALL "${DIR_SRC}/*.cpp" "${DIR_SRC}/*.c" "${DIR_SRC}/*.hpp" "${DIR_SRC}/*.h")
file(GLOB_RECURSE SRC_INC "${DIR_INC}/*.h" "${DIR_INC}/*.hpp")

# Force CXX compiler/linker for all source files
set_source_files_properties(${SRC_CPP} PROPERTIES LANGUAGE CXX)
set_source_files_properties(${SRC_C} PROPERTIES LANGUAGE CXX)

# Setup source file grouping for project files
source_group ("src" FILES ${SRC_ALL})

# Add to-be-linked exes/libs to project
link_directories(${DIR_LIB})
add_executable(gldemo ${SRC_ALL} ${SRC_INC})

# Set default VS_STARTUP_PROJECT
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT gldemo)

# Setup CXX flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Add to-be-linked dependencies
find_package(glfw3 3.2 REQUIRED)
find_package(OpenGL REQUIRED)
target_include_directories(gldemo PUBLIC "${OPENGL_INCLUDE_DIR}")
target_include_directories(gldemo PUBLIC "${DIR_INC}")
target_link_libraries(gldemo PUBLIC
  ${OPENGL_gl_LIBRARY}
  glfw
)

# Setup linker flags
target_link_libraries(gldemo PUBLIC
  -lstdc++
  -lm
  -ldl
  -lpthread
  -static-libgcc
  -static-libstdc++
)

# Set project build config
add_custom_command(
	TARGET gldemo POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${DIR_DTA}
    ${CMAKE_CURRENT_BINARY_DIR}/${DIR_DTA_NAME}
)

# Print info
message(STATUS "Target GENERATOR: ${CMAKE_GENERATOR}")
message(STATUS "OPENGL_INCLUDE_DIR: ${OPENGL_INCLUDE_DIR}")
message(STATUS "OPENGL_gl_LIBRARY: ${OPENGL_gl_LIBRARY}")

# Set project install config
install(TARGETS gldemo DESTINATION ${DIR_BIN})
install(DIRECTORY ${DIR_DTA} DESTINATION ${DIR_BIN})
if (MSVC)
  install(DIRECTORY ${DIR_LIB}/ DESTINATION ${DIR_BIN})
endif()

# Set project package(s)
set(CPACK_PACKAGE_NAME "gldemo")
set(CPACK_MONOLITHIC_INSTALL 1)
include(CPack)