echo "Creating Third Library..."

SOURCE_DIR="source"
INSTALL_DIR="../../install"

ASIO_DIR="asio-1.30.2"

ZLIB_INC_DIR="$INSTALL_DIR/zlib/include"
ZLIB_LIB_DEBUG="$INSTALL_DIR/zlib/lib/libz.so"
ZLIB_LIB_RELEASE="$INSTALL_DIR/zlib/lib/libz.so"

# cmake = "F:\MyApps\cmake-3.30.5-windows-x86_64\bin\cmake.exe"

[ -d "$SOURCE_DIR" ] || mkdir -p "$SOURCE_DIR"
[ -d "$INSTALL_DIR" ] || mkdir -p "$INSTALL_DIR"


cd "$SOURCE_DIR"

# spdlog
# git clone -b v1.15.0 https://github.com/gabime/spdlog.git

cd spdlog
cmake . -G "Unix Makefiles" -B ./build -DCMAKE_DEBUG_POSTFIX='d' -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR"/spdlog -DSPDLOG_BUILD_EXAMPLE=OFF -DSPDLOG_BUILD_SHARED=ON

cd build
cmake --build . --target install --config=Debug
cmake --build . --target install --config=Release

cd ../../

# asio
if [ -d "$ASIO_DIR" ]; then
     cd "$ASIO_DIR"
     cmake . -G "Unix Makefiles" -B ./build -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR"/asio
     
     cd build
     cmake --build . --target install --config=Release
     
     cd ../..
fi

# json
# git clone -b v3.11.3 https://github.com/nlohmann/json.git

cd json
cmake . -G "Unix Makefiles" -B ./build -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR"/nlohmann_json -DJSON_BuildTests=OFF -DBUILD_TESTING=OFF

cd build
cmake --build . --target install --config=Release
cd ../../

# YAML
# git clone -b 0.8.0 https://github.com/jbeder/yaml-cpp.git

cd yaml-cpp
cmake . -G "Unix Makefiles" -B ./build -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR"/YAML_CPP -DYAML_BUILD_SHARED_LIBS=ON -DYAML_CPP_BUILD_TESTS=OFF -DBUILD_TESTING=OFF

cd build
cmake --build . --target install --config=Release

cd ../../

# zlib
# git clone -b v1.3.1 https://github.com/madler/zlib.git

cd zlib
cmake . -G "Unix Makefiles" -B ./build -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR"/zlib -DINSTALL_BIN_DIR="$INSTALL_DIR"/zlib/bin -DINSTALL_INC_DIR="$INSTALL_DIR"/zlib/include -DINSTALL_LIB_DIR="$INSTALL_DIR"/zlib/lib -DINSTALL_MAN_DIR="$INSTALL_DIR"/zlib/share/man -DINSTALL_PKGCONFIG_DIR="$INSTALL_DIR"/zlib/share/pkgconfig

cd build
cmake --build . --target install --config=Release

cd ../../

# protobuf
# git clone -b v29.0 https://github.com/protocolbuffers/protobuf.git

cd protobuf
# git submodule update --init --recursive

cmake . -G "Unix Makefiles" -B ./build -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR"/protobuf -DBUILD_TESTING=OFF -DABSL_PROPAGATE_CXX_STD=ON -DCMAKE_CXX_STANDARD=20 -Dprotobuf_BUILD_LIBPROTOC=ON -Dprotobuf_BUILD_SHARED_LIBS=ON -Dprotobuf_BUILD_TESTS=OFF -DZLIB_INCLUDE_DIR="$ZLIB_INC_DIR" -DZLIB_LIBRARY_DEBUG="$ZLIB_LIB_DEBUG" -DZLIB_LIBRARY_RELEASE="$ZLIB_LIB_RELEASE"

cd build
cmake --build . --target install --config=Release

cd ../../

cd ../
echo "Third Library Build Finished."