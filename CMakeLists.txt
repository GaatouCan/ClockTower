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
list(APPEND CMAKE_PREFIX_PATH ${THIRD_LIBRARY_DIR}/mysql-connector-c++-9.0.0-winx64)

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

execute_process(
        COMMAND ${Python3_EXECUTABLE} "./protodef.py"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/protobuf
)

file(GLOB_RECURSE PROTOCOL
        ${CMAKE_SOURCE_DIR}/protobuf/Protocol.generated.h
        ${CMAKE_SOURCE_DIR}/protobuf/ProtoType.generated.h
        ${CMAKE_SOURCE_DIR}/protobuf/Protocol.generated.cpp
)

file(GLOB_RECURSE SOURCE_DIR
        ${CMAKE_SOURCE_DIR}/src/*.h
        ${CMAKE_SOURCE_DIR}/src/*.cpp
)

add_executable(server src/server.cpp ${SOURCE_DIR} ${PROTOCOL})

target_link_libraries(server PRIVATE spdlog::spdlog)
target_link_libraries(server PRIVATE asio::asio)
target_link_libraries(server PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(server PRIVATE protobuf::libprotobuf)
target_link_libraries(server PRIVATE yaml-cpp::yaml-cpp)
target_link_libraries(server PRIVATE mysql::concpp)