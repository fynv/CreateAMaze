cmake_minimum_required (VERSION 3.0)
project(CreateAMaze)

set(SOURCES
main.cpp
maze.cpp
maze.h
geometry.cpp
geometry.h
)

set(SOURCES_NODE
exports.cc
maze.cpp
maze.h
geometry.cpp
geometry.h
)


set (INCLUDE_DIR
glm/glm
node-addon-api
.
)

add_definitions(-DNAPI_VERSION=6)


set(CMAKE_CXX_STANDARD 17)
if (WIN32) 
set (DEFINES  ${DEFINES}
-D"_CRT_SECURE_NO_DEPRECATE"  
-D"_SCL_SECURE_NO_DEPRECATE" 
-D"_CRT_SECURE_NO_WARNINGS"
)
else()
add_compile_options(-fPIC)
endif()

include_directories(${CMAKE_JS_INC} ${INCLUDE_DIR})
add_definitions(${DEFINES})
add_executable(create ${SOURCES})

add_library(MazeNode SHARED ${SOURCES_NODE} ${CMAKE_JS_SRC})
set_target_properties(MazeNode PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(MazeNode ${CMAKE_JS_LIB})


if(MSVC AND CMAKE_JS_NODELIB_DEF AND CMAKE_JS_NODELIB_TARGET)
  # Generate node.lib
  execute_process(COMMAND ${CMAKE_AR} /def:${CMAKE_JS_NODELIB_DEF} /out:${CMAKE_JS_NODELIB_TARGET} ${CMAKE_STATIC_LINKER_FLAGS})
endif()



