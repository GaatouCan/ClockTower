cmake_minimum_required(VERSION 3.29)
project(ClockTower)

set(CMAKE_CXX_STANDARD 23)

if (WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0A00)
    option(USE_SOLUTION_FOLDERS ON)
endif ()

add_compile_definitions(ASIO_STANDALONE)
add_compile_definitions(ASIO_HAS_CO_AWAIT)

# Import Third Library
set(THIRD_LIBRARY_DIR D:/library/install)

if (WIN32)
    list(APPEND CMAKE_PREFIX_PATH ${THIRD_LIBRARY_DIR}/spdlog)
    list(APPEND CMAKE_PREFIX_PATH ${THIRD_LIBRARY_DIR}/asio)
    list(APPEND CMAKE_PREFIX_PATH ${THIRD_LIBRARY_DIR}/nlohmann_json)
    list(APPEND CMAKE_PREFIX_PATH ${THIRD_LIBRARY_DIR}/protobuf)
    list(APPEND CMAKE_PREFIX_PATH ${THIRD_LIBRARY_DIR}/zlib)
    list(APPEND CMAKE_PREFIX_PATH ${THIRD_LIBRARY_DIR}/YAML_CPP)
    list(APPEND CMAKE_PREFIX_PATH ${THIRD_LIBRARY_DIR}/mysql-connector-c++-9.1.0-winx64)
else ()
    list(APPEND CMAKE_PREFIX_PATH ~/cpp_server/library/install/mysql-connector-c++-9.1.0)
endif ()

find_package(spdlog CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(mysql-concpp CONFIG REQUIRED)
#find_package(Python3 REQUIRED COMPONENTS Interpreter)

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

#    list(APPEND PROTOBUF_LIBS ${proto_name}.proto)
    list(APPEND PROTOBUF_FILES ${proto_src} ${proto_hdr})
endforeach ()

add_library(proto_static STATIC ${PROTOBUF_FILES})
target_link_libraries(proto_static PRIVATE protobuf::libprotobuf)

if (MSVC)
    execute_process(
            COMMAND ./pbt.exe "--config=config.json"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
else ()
    execute_process(
            COMMAND ./pbt "--config=config.json"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif ()

file(GLOB_RECURSE BASE_SRC
        ${CMAKE_SOURCE_DIR}/base/*.h
        ${CMAKE_SOURCE_DIR}/base/*.cpp
)

if (MSVC)
    foreach(file ${BASE_SRC})
        get_filename_component(PARENT_DIR "${file}" DIRECTORY)
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
        string(REPLACE "/" "\\" GROUP "${GROUP}")
        set(GROUP "${GROUP}")
        source_group("${GROUP}" FILES "${file}")
    endforeach()
endif ()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/base/lib)

add_library(base_static STATIC ${BASE_SRC})
target_link_libraries(base_static PUBLIC spdlog::spdlog)
target_link_libraries(base_static PUBLIC asio::asio)
target_link_libraries(base_static PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(base_static PUBLIC protobuf::libprotobuf)
target_link_libraries(base_static PUBLIC yaml-cpp::yaml-cpp)
target_link_libraries(base_static PUBLIC mysql::concpp)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB_RECURSE COMMON_SRC ${CMAKE_SOURCE_DIR}/server/common/*.h ${CMAKE_SOURCE_DIR}/server/common/*.cpp)
file(GLOB_RECURSE IMPL_SRC ${CMAKE_SOURCE_DIR}/server/impl/*.h ${CMAKE_SOURCE_DIR}/server/impl/*.cpp)
file(GLOB_RECURSE COMMAND_SRC ${CMAKE_SOURCE_DIR}/server/command/*.h ${CMAKE_SOURCE_DIR}/server/command/*.cpp)
file(GLOB_RECURSE PLAYER_SRC ${CMAKE_SOURCE_DIR}/server/player/*.h ${CMAKE_SOURCE_DIR}/server/player/*.cpp)
file(GLOB_RECURSE CONFIG_SRC ${CMAKE_SOURCE_DIR}/server/config/*.h ${CMAKE_SOURCE_DIR}/server/config/*.cpp)
file(GLOB_RECURSE WORLD_SRC ${CMAKE_SOURCE_DIR}/server/world/*.h ${CMAKE_SOURCE_DIR}/server/world/*.cpp)

set(SERVER_SRC ${COMMON_SRC} ${IMPL_SRC} ${COMMAND_SRC} ${CONFIG_SRC} ${PLAYER_SRC} ${WORLD_SRC} server/server.cpp)
if (MSVC)
    foreach(file ${SERVER_SRC})
        get_filename_component(PARENT_DIR "${file}" DIRECTORY)
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
        string(REPLACE "/" "\\" GROUP "${GROUP}")
        set(GROUP "${GROUP}")
        source_group("${GROUP}" FILES "${file}")
    endforeach()
endif ()

add_executable(server ${SERVER_SRC})
target_include_directories(server PRIVATE ${PROTOBUF_GENERATED_DIR})
target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/base)
target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/struct)
target_link_libraries(server PRIVATE base_static)
target_link_libraries(server PRIVATE proto_static)

add_executable(demo main.cpp)
target_link_libraries(demo PRIVATE asio::asio)
target_link_libraries(demo PRIVATE spdlog::spdlog)