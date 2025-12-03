cmake_minimum_required(VERSION 3.16)

project(SoupleEdit VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)

#set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -fconcepts-diagnostics-depth=3 -fstats")

set(CMAKE_BUILD_PARALLEL_LEVEL 8)

set(CMAKE_UNITY_BUILD ON)
set(CMAKE_UNITY_BUILD_BATCH_SIZE 3)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick PrintSupport Gui GuiPrivate)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appSoupleEdit
    main.cpp
    vcpkg.json
    main.rc
)


#include_directories(C:/Users/guest0/Downloads/poppler-23.07.0/qt6/src)

file(GLOB DLLS ${CMAKE_SOURCE_DIR}/auto_copy_dll/*.dll)
file(GLOB TOOLS ${CMAKE_SOURCE_DIR}/ExternalTool/*.*)
file(GLOB BACK_FONTS ${CMAKE_SOURCE_DIR}/back-fonts/*.*)

foreach(dll ${DLLS})
    message(STATUS "found dll: ${dll}")
    add_custom_command(TARGET appSoupleEdit POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${dll}
            $<TARGET_FILE_DIR:appSoupleEdit>
        COMMENT "Has copied dll aside .exe file."
    )
endforeach()

add_custom_command(TARGET appSoupleEdit POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    "$<TARGET_FILE_DIR:appSoupleEdit>/tools/"
    COMMAND ${CMAKE_COMMAND} -E make_directory
    "$<TARGET_FILE_DIR:appSoupleEdit>/back-fonts/"
)


foreach(tool ${TOOLS})
    add_custom_command(TARGET appSoupleEdit POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${tool}
            "$<TARGET_FILE_DIR:appSoupleEdit>/tools/"
    )
endforeach()

foreach(font ${BACK_FONTS})
    add_custom_command(TARGET appSoupleEdit POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${font}
            "$<TARGET_FILE_DIR:appSoupleEdit>/back-fonts/"
    )
endforeach()

if (ANDROID)
    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/android)
        set_target_properties(appSoupleEdit PROPERTIES
            QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
        )
    endif()
endif ()

qt_add_qml_module(appSoupleEdit
    URI SoupleEdit
    VERSION 1.0
    QML_FILES
        Main.qml
        QML_FILES SoupleEdit.qml
        QML_FILES HorBaseline.qml
        QML_FILES VerBaseline.qml
        QML_FILES AnchorObj.qml
        QML_FILES Obj_TextEdit.qml
        QML_FILES AnchorPoint.qml
        RESOURCES main.qrc
        QML_FILES SelectedButton.qml
        QML_FILES ColorSelector.qml
        QML_FILES MyToolButton.qml
        QML_FILES ObjContainer.qml
        QML_FILES
        QML_FILES MyFunctions.qml
        QML_FILES MyDoubleSpinBox.qml
        QML_FILES ColorLabel.qml Line.qml LineText.qml
        QML_FILES RectButton.qml
        SOURCES helper.h helper.cpp
        QML_FILES PH_Right.qml
        QML_FILES PH_Rect.qml
        QML_FILES PH_JZRect.qml
        QML_FILES Obj_Image.qml
        SOURCES pdf2souple.h pdf2souple.cpp
        SOURCES soupleserializer.h soupleserializer.cpp
        RESOURCES vcpkg.json
        SOURCES souplemanager.h souplemanager.cpp
        SOURCES anchorobj.h anchorobj.cpp
        SOURCES anchorobj_hline.h anchorobj_hline.cpp
        SOURCES anchorobj_vline.h anchorobj_vline.cpp
        SOURCES obj.h obj.cpp
        SOURCES anchorobj_flowtext.h anchorobj_flowtext.cpp
        SOURCES anchorobj_phright.h anchorobj_phright.cpp
        SOURCES anchorobj_phrect.h anchorobj_phrect.cpp
        SOURCES anchorobj_image.h anchorobj_image.cpp
        SOURCES anchorobj_jzrect.h anchorobj_jzrect.cpp
        SOURCES myimageprovider.h myimageprovider.cpp
        SOURCES uiitempool.h uiitempool.cpp
        SOURCES ExportHeader.h
        QML_FILES DashLine.qml
        QML_FILES Triangle.qml
        QML_FILES ShapeBracket.qml
        QML_FILES MyProgressBar.qml
        SOURCES anchorobj_path.h anchorobj_path.cpp
        QML_FILES Obj_Path.qml
        QML_FILES Main_mobile.qml
        SOURCES souple_pdfsaver.h souple_pdfsaver.cpp
        QML_FILES HorRuler.qml
        SOURCES PlatformMethod.h
        QML_FILES FileBrowser.qml
        RESOURCES android/AndroidManifest.xml android/build.gradle android/res/drawable-hdpi/icon.png android/res/drawable-ldpi/icon.png android/res/drawable-mdpi/icon.png android/res/drawable-xhdpi/icon.png android/res/drawable-xxhdpi/icon.png android/res/drawable-xxxhdpi/icon.png android/res/values/libs.xml android/res/xml/file_paths.xml android/gradle/wrapper/gradle-wrapper.jar android/gradle/wrapper/gradle-wrapper.properties android/gradle.properties android/gradlew android/gradlew.bat
        SOURCES free_image.h free_image.cpp
        SOURCES free_path.h free_path.cpp
        SOURCES free_text.h free_text.cpp
        SOURCES free_rich.h free_rich.cpp
        SOURCES freeobj.h freeobj.cpp
        QML_FILES FreeObj_Image.qml
        QML_FILES FreeObj_Path.qml
        QML_FILES FreeObj_Text.qml
        QML_FILES FreeObj_Rich.qml
        QML_FILES FreeObjPositionPropertyGrid.qml
        SOURCES anchorobj_phleft.h anchorobj_phleft.cpp
        QML_FILES PH_Left.qml
        QML_FILES HLineSelector.qml
        SOURCES
        SOURCES quick_callback.h
        QML_FILES PdfLoadHelper.qml
        SOURCES horline_base.h horline_base.cpp
        QML_FILES BaseHorLine.qml
        SOURCES
        SOURCES JavaPkg.h
        SOURCES  PdfFontExtractor.h ToolExecuter.h
        SOURCES blockinner_horline.h blockinner_horline.cpp
        SOURCES anchorobj_rowgroup.h anchorobj_rowgroup.cpp
        SOURCES obj_tablerow.h obj_tablerow.cpp
        QML_FILES InnerHorLine.qml
        QML_FILES S_TextInput.qml
        SOURCES selectionmanager.h selectionmanager.cpp
        QML_FILES ChooseBoard.qml
        QML_FILES SingleCharButton.qml
        QML_FILES SquareButton.qml
        QML_FILES SelectionPropertyGrid.qml
        QML_FILES SelectionObj.qml
        SOURCES anchorobj_rich.h anchorobj_rich.cpp
        SOURCES DecorationInterface.h
        SOURCES tableline.h tableline.cpp
        SOURCES free_tableunit.h free_tableunit.cpp
        SOURCES TableFactory.h
        QML_FILES Table_HorLine.qml
        QML_FILES FreeObj_Unit.qml
        QML_FILES RightToolBar.qml
        QML_FILES ImageButton.qml
        QML_FILES CMenu.qml
        SOURCES pdf_table_analyse.hpp
        SOURCES word_page_analyse.cpp
        SOURCES
        SOURCES wordpage_vline.h wordpage_vline.cpp
        SOURCES ExportHeader.cpp
        QML_FILES SmallComboBox.qml
        QML_FILES ListIndicator.qml
        QML_FILES WordPage_VLine.qml
        SOURCES TT_Str.h
        SOURCES TT_Str_Macro.h
        SOURCES anchorobj_glue.h anchorobj_glue.cpp
        QML_FILES Obj_Page.qml
        SOURCES Obj_Page.h
        QML_FILES RectsProgressBar.qml
        QML_FILES MainWindowTitleBar.qml
        QML_FILES CustomToolButton.qml
        QML_FILES DocumentSpec_StartPage.qml
        SOURCES pdf2souple_template_impl.hpp
        QML_FILES Obj_Glue.qml
        QML_FILES ShaderEffect_Stroke.qml
        QML_FILES PH_Glue.qml
        QML_FILES AddToolButton.qml
        SOURCES frame_ofhlines.h frame_ofhlines.cpp
        SOURCES
        SOURCES anchorobj_latexformula.h anchorobj_latexformula.cpp
        QML_FILES
        QML_FILES Obj_Latex_Formula.qml
        SOURCES turnbackmanager.h
        SOURCES turnbackmanager.cpp
        SOURCES turnback_template.hpp
        SOURCES free_frame.h free_frame.cpp
        QML_FILES Obj_Frame.qml
        SOURCES LLException.h
        QML_FILES Editable_Bound.qml
        QML_FILES Dialog_setJAVA_HOME.qml
        QML_FILES ToolBar_01.qml
        QML_FILES GlowImageButton.qml
        QML_FILES DotBar.qml
        QML_FILES Expandable_LineText.qml
        SOURCES safe_pointer.h
        SOURCES LiveValue.h
        SOURCES magicalcursor.h magicalcursor.cpp
        QML_FILES TText.qml
        QML_FILES Dialog_DocumentDirTool.qml
        QML_FILES FlatButton.qml
        QML_FILES TipButton.qml
        SOURCES NoRef_SoupleManager.h
        SOURCES NoRef_ResolveALL.cpp
        QML_FILES TCombo.qml
        QML_FILES TTextField.qml
        QML_FILES Dialog_FileSelect.qml
        QML_FILES TStackView.qml
        SOURCES navline.h navline.cpp
        SOURCES anchorobj_spring.h anchorobj_spring.cpp
        QML_FILES Obj_Nav.qml
        QML_FILES Obj_Spring.qml
        SOURCES nav_create.cpp
        QML_FILES FontEdit.qml
        QML_FILES Slider_and_ASB.qml
        QML_FILES NavDirPreview.qml
        SOURCES obj_start_sign.h
        QML_FILES Obj_StartSign.qml
        SOURCES serialization.h
        SOURCES serialization.cpp
        SOURCES souple_aux.h
        SOURCES columns_separate.h
        SOURCES columns_separate.cpp
        QML_FILES SepLine.qml
        QML_FILES FlatDoubleSpinBox.qml
        QML_FILES TToolBar_Start.qml
        QML_FILES ComboBox_forFontSize.qml
        #RESOURCES vcpkg.json
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appSoupleEdit PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appSoupleEdit
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(WIN32)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/pdfium-win-x64/include)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/freetype/include)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/java/include)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/zlib/include)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/xlsx/include)
    target_link_libraries(appSoupleEdit PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/pdfium-win-x64/lib/pdfium.dll.lib)
    target_link_libraries(appSoupleEdit PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/dwmapi.lib)
    target_link_libraries(appSoupleEdit PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/xlsx/libxlsxwriter.a)
    target_link_libraries(appSoupleEdit PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/zlib/zlib.lib)
    #target_link_libraries(appSoupleEdit PRIVATE E:/freetype/freetype2410.lib)
    #target_link_libraries(appSoupleEdit PRIVATE "C:/Program Files/Java/jdk1.8.0_331/lib/jvm.lib")
endif()

if(ANDROID)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/pdfium-android-arm64/include)
    add_library(PDF_LIBRARY SHARED IMPORTED)
    set_target_properties(PDF_LIBRARY PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/pdfium-android-arm64/lib/libpdfium.so)
    target_link_libraries(appSoupleEdit PRIVATE PDF_LIBRARY)
endif()

target_link_libraries(appSoupleEdit
    PRIVATE Qt6::Quick
    PRIVATE Qt6::PrintSupport
)

include(GNUInstallDirs)
install(TARGETS appSoupleEdit
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

#include(C:/Users/guest0/Downloads/poppler-25.03.0.tar/poppler-25.03.0/poppler-25.03.0/qt6/src
add_subdirectory(souple_pdf)
