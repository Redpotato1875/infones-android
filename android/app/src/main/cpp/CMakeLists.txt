cmake_minimum_required(VERSION 3.22.1)
project(infones CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 仓库根目录：android/app/src/main/cpp 向上 5 级
set(INFONES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../../..")

add_library(infones SHARED
    "${INFONES_ROOT}/core/InfoNES.cpp"
    "${INFONES_ROOT}/core/K6502.cpp"
    "${INFONES_ROOT}/core/InfoNES_pAPU.cpp"
    "${INFONES_ROOT}/core/InfoNES_Mapper.cpp"
    "${INFONES_ROOT}/port/android/InfoNES_Port_Android.cpp"
    "${INFONES_ROOT}/port/android/InfoNES_Sound_Android.cpp"
    "infones_jni.cpp"
)

target_include_directories(infones PRIVATE
    "${INFONES_ROOT}/core"
    "${INFONES_ROOT}/port/android"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# core/InfoNES_Mapper.cpp 用相对路径 #include 137 个 mapper 文件，
# 由同一编译单元处理，无需加入 add_library。

find_library(log-lib log)
find_library(android-lib android)
find_library(aaudio-lib aaudio)

target_link_libraries(infones PRIVATE
    ${log-lib}
    ${android-lib}
    ${aaudio-lib}
)

target_compile_options(infones PRIVATE -Wall -Wextra -Wno-unused-parameter -O3)
