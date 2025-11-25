cmake_minimum_required(VERSION 3.16)
project(AppleCG C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_PREFIX_PATH
  /opt/homebrew
  /opt/homebrew/lib/cmake
  "C:/msys64/mingw64"
)

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm QUIET)

# ---- Sources ----
add_executable(app
  src/main.cpp
)

# ---- Include headers ----
target_include_directories(app PRIVATE
  ${CMAKE_SOURCE_DIR}/common
)

# ---- Links ----
if (TARGET glm::glm)
  target_link_libraries(app PRIVATE glm::glm)
else()
  target_include_directories(app PRIVATE
    /opt/homebrew/include
    "C:/msys64/mingw64/include"
  )
endif()

target_link_libraries(app PRIVATE glfw)
target_link_libraries(app PRIVATE GLEW::GLEW)

if (APPLE)
  target_compile_definitions(app PRIVATE GL_SILENCE_DEPRECATION)
  target_link_libraries(app PRIVATE "-framework OpenGL")
elseif (WIN32)
  target_link_libraries(app PRIVATE opengl32)
else()
  find_package(OpenGL REQUIRED)
  target_link_libraries(app PRIVATE OpenGL::GL)
endif()

# ---- Copy shaders ----
file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})

# ---- Copy assets ----
file(COPY src/Apple.obj DESTINATION ${CMAKE_BINARY_DIR}/src)
file(COPY src/apple_color.png DESTINATION ${CMAKE_BINARY_DIR}/src)
