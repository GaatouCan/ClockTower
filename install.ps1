Write-Output "正在创建第三方库"

$SOURCE_DIR = "./source"
$INSTALL_DIR = "D:/library/install"

$ASIO_DIR = "asio-1.30.2"

$cmake = "cmake"

if (-not (Test-Path -Path $SOURCE_DIR)) {
    New-Item -ItemType Directory -Path $SOURCE_DIR
    Write-Host "已创建源目录: $folderPath"
}

if (-not (Test-Path -Path $INSTALL_DIR)) {
    New-Item -ItemType Directory -Path $INSTALL_DIR
    Write-Host "已创建安装目录: $INSTALL_DIR"
}

Set-Location $SOURCE_DIR

# spdlog
# git clone -b v1.15.0 https://github.com/gabime/spdlog.git

# Set-Location ./spdlog
# & $cmake . -G "Visual Studio 17 2022" -B ./build -DCMAKE_DEBUG_POSTFIX='d' -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR"/spdlog -DSPDLOG_BUILD_EXAMPLE=OFF -DSPDLOG_BUILD_SHARED=ON

# Set-Location ./build
# & $cmake --build . --target install --config=Debug
# & $cmake --build . --target install --config=Release

# Set-Location ../../

#asio
# if ((Test-Path -Path $ASIO_DIR)) {
#     Set-Location $ASIO_DIR
#     & $cmake . -G "Visual Studio 17 2022" -B ./build -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR"/asio
#     Set-Location ./build
#     & $cmake --build . --target install --config=Release
#     Set-Location ../../
# }

# json
# git clone -b v3.11.3 https://github.com/nlohmann/json.git

# Set-Location ./json
# & $cmake . -G "Visual Studio 17 2022" -B ./build -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR"/nlohmann_json -DJSON_BuildTests=OFF -DBUILD_TESTING=OFF

# Set-Location ./build
# & $cmake --build . --target install --config=Release
# Set-Location ../../

# YAML
# git clone -b 0.8.0 https://github.com/jbeder/yaml-cpp.git

# Set-Location ./yaml-cpp
# & $cmake . -G "Visual Studio 17 2022" -B ./build -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR"/YAML_CPP -DYAML_BUILD_SHARED_LIBS=ON -DYAML_CPP_BUILD_TESTS=OFF -DBUILD_TESTING=OFF

# Set-Location ./build
# & $cmake --build . --target install --config=Debug
# & $cmake --build . --target install --config=Release

# Set-Location ../../

# zlib
# git clone -b v1.3.1 https://github.com/madler/zlib.git

Set-Location ./zlib
& $cmake . -G "Visual Studio 17 2022" -B ./build -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR"/zlib -DCMAKE_DEBUG_POSTFIX='d' -DINSTALL_BIN_DIR="$INSTALL_DIR"/zlib/bin -DINSTALL_INC_DIR="$INSTALL_DIR"/zlib/include -DINSTALL_LIB_DIR="$INSTALL_DIR"/zlib/lib -DINSTALL_MAN_DIR="$INSTALL_DIR"/zlib/share/man -DINSTALL_PKGCONFIG_DIR="$INSTALL_DIR"/zlib/share/pkgconfig

Set-Location ./build
& $cmake --build . --target install --config=Debug
& $cmake --build . --target install --config=Release

Set-Location ../../



Set-Location ../