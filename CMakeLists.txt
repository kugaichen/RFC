cmake_minimum_required(VERSION 3.14)  # 最低 CMake 版本
project(RFC VERSION 1.0)              # 项目名称和版本

#  设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF) # 通常建议关闭编译器特定扩展，以提高可移植性

# 添加可执行文件（从 main.cpp 构建）
add_executable(main 
    src/main.cpp 
    src/Classifier_building.cpp
    src/Classifier_lookuping.cpp
    src/log_creating.cpp    
)

# --- 添加包含目录 ---
# target_include_directories 使得这些目录只对 rfc_classifier 目标可见
# PUBLIC:      头文件路径会传递给链接到此目标的其他目标
# INTERFACE:   头文件路径仅传递给链接到此目标的其他目标，不用于编译此目标
# PRIVATE:     头文件路径仅用于编译此目标，不传递
target_include_directories(main PRIVATE 
    ${PROJECT_SOURCE_DIR}/include
)


# --- 设置构建类型 (可选, 但推荐) ---
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build: Debug, Release, MinSizeRel, RelWithDebInfo." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# --- 设置构建类型 (可选, 但推荐) ---
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build: Debug, Release, MinSizeRel, RelWithDebInfo." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# --- 安装规则 (可选, 用于部署) ---
include(GNUInstallDirs) # 提供 CMAKE_INSTALL_BINDIR, CMAKE_INSTALL_LIBDIR 等标准变量

install(TARGETS main
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # 安装可执行文件到 bin 目录
)

# 如果你有需要安装的头文件 (例如，如果这是一个库项目)，可以添加：
# install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

# --- CPack 配置 (可选, 用于创建安装包) ---
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "RFC Classifier Application")
set(CPACK_PACKAGE_VENDOR "Your Name/Organization") # 修改为你自己的信息
set(CPACK_PACKAGE_CONTACT "your.email@example.com") # 修改为你自己的信息

# 根据目标平台选择打包器
if(WIN32)
    set(CPACK_GENERATOR "NSIS") # Windows 平台使用 NSIS 创建安装程序
    # set(CPACK_GENERATOR "ZIP") # 或者创建一个简单的 ZIP 包
elseif(UNIX)
    if(APPLE)
        set(CPACK_GENERATOR "DragNDrop") # macOS 平台创建 DMG
    else()
        set(CPACK_GENERATOR "TGZ;DEB")   # Linux 平台创建 tar.gz 和 Debian 包
    endif()
else()
    set(CPACK_GENERATOR "ZIP") # 其他平台默认创建 ZIP 包
endif()

# 在 CMakeLists.txt 的末尾包含 CPack 模块
include(CPack)
