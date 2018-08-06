# Force minimum CMake version
cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

# Create and init project
project(gldemo CXX)
set(DIR_SRC "${PROJECT_SOURCE_DIR}/src")
set(DIR_INC "${PROJECT_SOURCE_DIR}/inc")
set(DIR_INC_PLATFORM "${PROJECT_SOURCE_DIR}/inc/${CMAKE_C_COMPILER}")
set(DIR_LIB_PLATFORM "${PROJECT_SOURCE_DIR}/lib/${CMAKE_C_COMPILER}")
set(DIR_DTA_NAME "data")
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
file(GLOB_RECURSE SRC_INC "${DIR_INC_PLATFORM}/*.h" "${DIR_INC_PLATFORM}/*.hpp")

# Force CXX compiler/linker for all source files
set_source_files_properties(${SRC_CPP} PROPERTIES LANGUAGE CXX)
set_source_files_properties(${SRC_C} PROPERTIES LANGUAGE CXX)

# Setup source file grouping for project files
source_group ("src" FILES ${SRC_ALL})

# Add to-be-linked exes/libs to project
link_directories(${DIR_LIB_PLATFORM})
add_executable(gldemo ${SRC_ALL} ${SRC_INC})

# Set default VS_STARTUP_PROJECT
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT gldemo)

# Setup CXX flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Setup linker flags
target_link_libraries(gldemo PUBLIC
  -lstdc++
  -lm
  -ldl
  -lpthread
  -static-libgcc
  -static-libstdc++
)

if (NOT MINGW)
  target_link_libraries(gldemo PUBLIC
    -lSDL2_mixer
  )
else()
  message(STATUS "MinGW detected! Linker flags set.")

  target_link_libraries(gldemo PUBLIC
    -lmingw32
    -mconsole
    -static
  )
endif()

# Add to-be-linked dependencies
find_package(OpenGL REQUIRED)
target_include_directories(gldemo PUBLIC "${OPENGL_INCLUDE_DIR}")
target_include_directories(gldemo PUBLIC "${DIR_INC}")
target_include_directories(gldemo PUBLIC "${DIR_INC_PLATFORM}")

if (NOT MINGW)
  find_package(glfw3 3.2 REQUIRED)
  find_package(SDL2 REQUIRED)
  target_include_directories(gldemo PUBLIC "${SDL2_INCLUDE_DIRS}")
  target_link_libraries(gldemo PUBLIC
    ${OPENGL_gl_LIBRARY}
    glfw
    ${SDL2_LIBRARIES}
  )
else()
  set(GLFW_LIBRARIES ${GLFW_LIBRARIES} "${DIR_LIB_PLATFORM}/libglfw3.a")
  set(SDL2_LIBRARY ${SDL2_LIBRARY}
    "${DIR_LIB_PLATFORM}/libogg.dll.a"
    "${DIR_LIB_PLATFORM}/libvorbis.dll.a"
    "${DIR_LIB_PLATFORM}/libvorbisenc.dll.a"
    "${DIR_LIB_PLATFORM}/libvorbisfile.dll.a"
    "${DIR_LIB_PLATFORM}/libSDL2main.a"
    "${DIR_LIB_PLATFORM}/libSDL2.dll.a"
    "${DIR_LIB_PLATFORM}/libSDL2_mixer.dll.a"
  )
  target_link_libraries(gldemo PUBLIC
    ${OPENGL_gl_LIBRARY}
    ${GLFW_LIBRARIES}
    ${SDL2_LIBRARY}
  )
endif()

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
message(STATUS "GLFW_LIBRARIES: ${GLFW_LIBRARIES}")

# Set project install config
install(TARGETS gldemo DESTINATION ${DIR_BIN})
install(DIRECTORY ${DIR_DTA} DESTINATION ${DIR_BIN})
if (MINGW)
  install(DIRECTORY ${DIR_LIB_PLATFORM}/ DESTINATION ${DIR_BIN})
endif()


# Set project package(s)
set(CPACK_PACKAGE_NAME "gldemo")
set(CPACK_MONOLITHIC_INSTALL 1)
include(CPack)