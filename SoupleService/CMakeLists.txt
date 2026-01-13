cmake_minimum_required(VERSION 3.16)
project(SoupleService LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick)

add_executable(SoupleService
  main.cpp
)
target_link_libraries(SoupleService PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick)

include(GNUInstallDirs)
install(TARGETS SoupleService
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

include_directories(E:/vcpkg/vcpkg/installed/x64-windows/include)
target_link_libraries(SoupleService PRIVATE
  "E:/vcpkg/vcpkg/installed/x64-windows/lib/boost_date_time-vc143-mt-x64-1_89.lib")
target_link_libraries(SoupleService PRIVATE
  "E:/vcpkg/vcpkg/installed/x64-windows/lib/boost_context-vc143-mt-x64-1_89.lib")
target_link_libraries(SoupleService PRIVATE
  "E:/vcpkg/vcpkg/installed/x64-windows/lib/boost_container-vc143-mt-x64-1_89.lib")

