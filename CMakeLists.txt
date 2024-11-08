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
list(APPEND CMAKE_PREFIX_PATH ${THIRD_LIBRARY_DIR}/mysql-connector-c++-9.1.0-winx64)

find_package(spdlog CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(mysql-concpp CONFIG REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

# Setting Protobuf Generated Directory
message(STATUS "Using Protobuf ${Protobuf_VERSION}")

if (CMAKE_CROSSCOMPILING)
    find_program(PROTOBUF_PROTOC protoc)
else ()
    set(PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
endif ()

set(PROTOBUF_DEFINE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/protobuf/def)
set(PROTOBUF_GENERATED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/protobuf/gen)
set(PROTOBUF_LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/protobuf/lib)

# Add All Protobuf Files
file(GLOB_RECURSE PROTOBUF_LIST ${PROTOBUF_DEFINE_DIR}/*.proto)

# Generate *.pb.h & *.pb.cc File And Generate Binary *.proto.lib File
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROTOBUF_LIBRARY_DIR})
foreach (proto ${PROTOBUF_LIST})
    get_filename_component(proto_name ${proto} NAME_WE)
    set(proto_src ${PROTOBUF_GENERATED_DIR}/${proto_name}.pb.cc)
    set(proto_hdr ${PROTOBUF_GENERATED_DIR}/${proto_name}.pb.h)
    add_custom_command(
            OUTPUT ${proto_src} ${proto_hdr}
            COMMAND ${PROTOBUF_PROTOC}
            ARGS --cpp_out ${PROTOBUF_GENERATED_DIR} -I ${PROTOBUF_DEFINE_DIR} ${proto}
            DEPENDS ${proto})
    add_library(${proto_name}.proto STATIC ${proto_src} ${proto_hdr})
    target_link_libraries(${proto_name}.proto PRIVATE protobuf::libprotobuf)

    list(APPEND PROTOBUF_LIBS ${proto_name}.proto)
endforeach ()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# Link To Generated Files
include_directories(${PROTOBUF_GENERATED_DIR})
link_directories(${PROTOBUF_LIBRARY_DIR}/Debug)

include_directories(${CMAKE_SOURCE_DIR}/struct)
link_directories(${CMAKE_SOURCE_DIR}/struct/lib/Debug)

#execute_process(
#        COMMAND ./PreBuildTool.exe "--config=build.json"
#        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#)

include_directories(${CMAKE_SOURCE_DIR}/struct)

file(GLOB_RECURSE BASE_SRC
        ${CMAKE_SOURCE_DIR}/base/*.h
        ${CMAKE_SOURCE_DIR}/base/*.cpp
)

file(GLOB_RECURSE COMMON_SRC ${CMAKE_SOURCE_DIR}/server/common/*.h ${CMAKE_SOURCE_DIR}/server/common/*.cpp)
file(GLOB_RECURSE IMPL_SRC ${CMAKE_SOURCE_DIR}/server/impl/*.h ${CMAKE_SOURCE_DIR}/server/impl/*.cpp)
file(GLOB_RECURSE PLAYER_SRC ${CMAKE_SOURCE_DIR}/server/player/*.h ${CMAKE_SOURCE_DIR}/server/player/*.cpp)

file(GLOB_RECURSE WORLD_SRC ${CMAKE_SOURCE_DIR}/server/world/*.h ${CMAKE_SOURCE_DIR}/server/world/*.cpp)

add_library(base_static STATIC ${BASE_SRC})
target_link_libraries(base_static PUBLIC spdlog::spdlog)
target_link_libraries(base_static PUBLIC asio::asio)
target_link_libraries(base_static PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(base_static PUBLIC protobuf::libprotobuf)
target_link_libraries(base_static PUBLIC yaml-cpp::yaml-cpp)
target_link_libraries(base_static PUBLIC mysql::concpp)

include_directories(${BASE_SRC})
add_executable(server server/server.cpp ${COMMON_SRC} ${IMPL_SRC} ${PLAYER_SRC})
target_link_libraries(server PRIVATE base_static)
target_link_libraries(server PRIVATE ${PROTOBUF_LIBS})

add_executable(demo main.cpp)
target_link_libraries(demo PRIVATE asio::asio)
target_link_libraries(demo PRIVATE spdlog::spdlog)