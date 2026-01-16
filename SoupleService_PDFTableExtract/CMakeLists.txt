cmake_minimum_required(VERSION 3.16)
project(SoupleService_PDFTableExtract LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick)

add_executable(SoupleService_PDFTableExtract
  main.cpp
  config.h
  worker.h
  helper.h
)
target_link_libraries(SoupleService_PDFTableExtract PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick)

include(GNUInstallDirs)
install(TARGETS SoupleService_PDFTableExtract
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)



# target_link_libraries(SoupleService_PDFTableExtract PRIVATE
#   "E:/vcpkg/vcpkg/installed/x64-windows/lib/boost_date_time-vc143-mt-x64-1_89.lib")
# target_link_libraries(SoupleService_PDFTableExtract PRIVATE
#   "E:/vcpkg/vcpkg/installed/x64-windows/lib/boost_context-vc143-mt-x64-1_89.lib")
# target_link_libraries(SoupleService_PDFTableExtract PRIVATE
#   "E:/vcpkg/vcpkg/installed/x64-windows/lib/boost_container-vc143-mt-x64-1_89.lib")
include_directories(E:/vcpkg/vcpkg/installed/x64-windows/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/json)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_CURRENT_SOURCE_DIR}/../lib/json")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ljsoncpp")
