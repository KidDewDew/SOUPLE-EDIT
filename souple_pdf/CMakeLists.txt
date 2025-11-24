cmake_minimum_required(VERSION 3.16)

project(souple_pdf LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(souple_pdf SHARED
  souple_pdf_global.h
  souplepdf.cpp
  souplepdf.h
)

target_link_libraries(souple_pdf PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_compile_definitions(souple_pdf PRIVATE SOUPLE_PDF_LIBRARY)
