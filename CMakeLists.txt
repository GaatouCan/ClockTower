cmake_minimum_required(VERSION 3.29)
project(ClockTower)

add_compile_definitions(_WIN32_WINNT=0x0A00)
add_compile_definitions(ASIO_STANDALONE)
add_compile_definitions(ASIO_HAS_CO_AWAIT)

set(CMAKE_CXX_STANDARD 23)

# Import Third Library
set(THIRD_LIBRARY_DIR D:/library/install)

list(APPEND CMAKE_PREFIX_PATH ${THIRD_LIBRARY_DIR}/spdlog)
list(APPEND CMAKE_PREFIX_PATH ${THIRD_LIBRARY_DIR}/asio)
list(APPEND CMAKE_PREFIX_PATH ${THIRD_LIBRARY_DIR}/nlohmann_json)
list(APPEND CMAKE_PREFIX_PATH ${THIRD_LIBRARY_DIR}/protobuf)
list(APPEND CMAKE_PREFIX_PATH ${THIRD_LIBRARY_DIR}/zlib)
list(APPEND CMAKE_PREFIX_PATH ${THIRD_LIBRARY_DIR}/YAML_CPP)

find_package(spdlog CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)

# Setting Protobuf Generated Directory
message(STATUS "Using Protobuf ${Protobuf_VERSION}")

file(GLOB_RECURSE SOURCE_DIR
        ${CMAKE_SOURCE_DIR}/src/*.h
        ${CMAKE_SOURCE_DIR}/src/*.cpp
)

add_executable(server src/server.cpp ${SOURCE_DIR})

target_link_libraries(server PRIVATE spdlog::spdlog)
target_link_libraries(server PRIVATE asio::asio)
target_link_libraries(server PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(server PRIVATE protobuf::libprotobuf)
target_link_libraries(server PRIVATE yaml-cpp::yaml-cpp)