cmake_minimum_required(VERSION 3.16)
project(SoupleService_PDFTableExtract LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_UNITY_BUILD OFF)

if (MINGW)
    add_compile_options(-Wa,-mbig-obj)
endif()

find_package(Qt6 6.10 REQUIRED COMPONENTS Core Quick)

file(GLOB MAIN_SRC
  "${CMAKE_SOURCE_DIR}/*.h"
  "${CMAKE_SOURCE_DIR}/*.hpp"
  "${CMAKE_SOURCE_DIR}/*.c"
  "${CMAKE_SOURCE_DIR}/*.cpp")

add_executable(SoupleService_PDFTableExtract
  main.cpp
  config.h
  worker.h
  helper_web.h
  ../souplemanager.h ../souplemanager.cpp
  ../anchorobj_phleft.cpp
  ../anchorobj_glue.cpp
  ../selectionmanager.cpp
  ../obj.cpp
  ../ExportHeader.cpp
  ../anchorobj_image.cpp
  ../anchorobj_flowtext.cpp
  ../NoRef_ResolveALL.cpp
  ../helper.cpp  ../anchorobj_latexformula.cpp
  ../frame_ofhlines.cpp ../anchorobj_jzrect.cpp
  ../anchorobj.cpp ../navline.cpp ../nav_create.cpp
  ../pdf2souple.h  ../pdf2souple.cpp
  ../anchorobj_phright.cpp ../free_frame.cpp
  ../turnbackmanager.cpp  ../anchorobj_phrect.cpp
  ../anchorobj_vline.cpp  ../freeobj.cpp  ../anchorobj_hline.cpp
  ../horline_base.cpp  ../columns_separate.cpp
  ../free_tableunit.cpp
  ../uniformkb.cpp ../anchorobj_path.cpp ../free_path.cpp
  ../anchorobj_path.cpp ../free_rich.cpp ../wordline.cpp
  ../wordpage_vline.cpp ../word_page_analyse.cpp
  ../tableline.cpp ../souple_pdfsaver.cpp
  ../blockinner_horline.cpp ../quick_callback.cpp
  ../anchorobj_spring.cpp  ../free_image.cpp ../anchorobj_rich.cpp
  ../anchorobj_hline.h
  ../wordline.h
  ../anchorobj_vline.h
  ../horline_base.h
  ../tableline.h
  ../table_extract_tool.cpp
  etc.cpp
)

# # 由于boost库使用WinSock.h，必须把main.cpp单独编译
# set_source_files_properties(
#     main.cpp
#     PROPERTIES
#     SKIP_UNITY_BUILD TRUE
# )

add_definitions(
-DPROJECT_TABLE_EXTRACT  #启用PROJECT宏
-DULONGLONG_OBJID  #启用unsigned long long类型的对象id
-DMULTITHREAD_SOUPLEMANAGER # 启用多线程分离模式的SoupleManager
-DMULTITHREAD_PDF2SOUPLE #启用线程安全的pdf2souple
-DOBJID_t=quint64
-DNO_CACHE_IMAGE   #不缓存图片
)

target_link_libraries(SoupleService_PDFTableExtract
  PRIVATE
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Quick)

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

if(WIN32)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/pdfium-win-x64/include)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/freetype/include)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/java/include)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/zlib/include)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/xlsx/include)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/minizip/include)
    target_link_libraries(SoupleService_PDFTableExtract PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../lib/pdfium-win-x64/lib/pdfium.dll.lib)
    target_link_libraries(SoupleService_PDFTableExtract PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../lib/dwmapi.lib)
    target_link_libraries(SoupleService_PDFTableExtract PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../lib/xlsx/libxlsxwriter.a)
    target_link_libraries(SoupleService_PDFTableExtract PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../lib/zlib/libzlib.dll.a)
    target_link_libraries(SoupleService_PDFTableExtract PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../lib/minizip/libminizip.dll.a)
    #target_link_libraries(appSoupleEdit PRIVATE E:/freetype/freetype2410.lib)
    #target_link_libraries(appSoupleEdit PRIVATE "C:/Program Files/Java/jdk1.8.0_331/lib/jvm.lib")
endif()
