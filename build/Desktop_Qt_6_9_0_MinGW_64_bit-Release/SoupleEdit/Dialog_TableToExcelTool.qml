import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import com.custom 1.0

// 表格转Excel工具窗口
ApplicationWindow {
    id: view
    color: theme.bg
    width: 650
    height: 500
    title: "表格转Excel工具 v1.0"
    Material.foreground: theme.fg
    Material.background: theme.bg
    Material.primary: theme.bg
    Material.theme: isProtectEyeMode ? Material.Dark : Material.Light

    // 新增：加载状态提示（PDF解析/表格提取时显示）
    property bool isLoading: false

    // 状态管理：避免重复打开
    Component.onCompleted: {
        Helper.temp_Set("TableToExcelTool_opened", true);
        updateOpenedDocList(); // 加载已打开的文档
    }

    onClosing: {
        Helper.temp_Set("TableToExcelTool_opened", false);
        view.destroy();
    }

    // 核心数据（完善后）
    property var openedDocs: [] // 已打开的Souple文档列表（{s:文档ID, tabName:文档名称}）
    property var currentTables: [] // 当前选中模式下的表格列表（{tableInfo:TableInfo*, rowCount:行数, colCount:列数}）
    property int selectedDocIndex: -1 // 选中的文档索引（-1=未选中）
    property int selectedTableIndex: -1 // 选中的表格索引（-1=未选中）
    property bool isPdfMode: false // 是否为“PDF文件提取”模式

    // -------------------------- 1. 加载已打开文档的表格（对接主软件接口）--------------------------
    function updateOpenedDocList() {
        // 调用主软件接口获取所有已打开的Souple文档
        openedDocs = main_titleBar.getDocumentList(
            (tab) => tab.documentType === "souple"
        );

        // 处理文档下拉框（添加占位项，兼容TCombo只读特性）
        let docTitles = [];
        if (openedDocs.length === 0) {
            docTitles = ["暂无已打开文档"];
            selectedDocIndex = -1;
            currentTables = [];
            table_list_view.model = 0;
        } else {
            docTitles = ["选择文档"].concat(openedDocs.map(doc => doc.tabName));
            selectedDocIndex = 0; // 默认选中占位项
        }
        combo_opened_docs.model = docTitles;
    }

    // 从选中文档提取表格（核心：调用spMgr获取TableInfo列表）
    function loadTablesFromDoc(doc) {
        if (!doc || !spMgr) {
            currentTables = [];
            table_list_view.model = 0;
            return;
        }

        // 显示加载状态
        isLoading = true;
        // 调用主软件接口：获取文档中所有表格的TableInfo（异步模拟，实际为主软件同步/异步接口）
        setTimeout(() => {
            try {
                // 核心：获取文档中所有表格的TableInfo实例（主软件提供的接口）
                const tableInfoList = spMgr.getTablesInDocument(doc.s); // 返回 TableInfo* 数组

                // 包装表格数据（QML可识别的结构，关联TableInfo指针）
                currentTables = tableInfoList.map((tableInfo, index) => {
                    return {
                        tableInfo: tableInfo, // 核心：C++的TableInfo指针（用于后续导出）
                        rowCount: tableInfo.rowCount(), // 假设TableInfo有rowCount()方法获取行数
                        colCount: tableInfo.colCount()  // 假设TableInfo有colCount()方法获取列数
                    };
                });

                // 无表格时提示
                if (currentTables.length === 0) {
                    messageBox("提示", `文档"${doc.tabName}"中未识别到表格`);
                }
                table_list_view.model = currentTables.length;
            } catch (e) {
                messageBox("错误", `提取表格失败：${e.message}`);
                currentTables = [];
                table_list_view.model = 0;
            } finally {
                isLoading = false;
            }
        }, 300); // 模拟加载延迟，实际可去掉
    }

    // -------------------------- 2. PDF文件解析表格（对接主软件PDF接口）--------------------------
    function openPDFAndParse() {
        Qt.createQmlObject(`
            import QtQuick
            Dialog_FileSelect {
                title: "选择PDF文件"
                nameFilters: ["PDF文件 (*.pdf)"]
                onAccepted: {
                    let pdfPath = Helper.qurl2localfile(currentFile);
                    parsePdfTables(pdfPath); // 调用解析逻辑
                }
            }
        `, view).open();
    }

    // PDF表格解析核心逻辑
    function parsePdfTables(pdfPath) {
        if (!spMgr || !pdfPath) {
            messageBox("错误", "PDF解析接口未就绪");
            return;
        }

        // 显示加载状态
        isLoading = true;
        currentTables = [];
        table_list_view.model = 0;

        // 调用主软件PDF表格解析接口（异步接口，解析完成后回调）
        spMgr.parsePdfTables(
            pdfPath,
            // 成功回调：返回TableInfo列表
            (tableInfoList) => {
                isLoading = false;
                if (!tableInfoList || tableInfoList.length === 0) {
                    messageBox("提示", "该PDF中未识别到表格");
                    return;
                }

                // 包装表格数据（与文档提取的格式统一）
                currentTables = tableInfoList.map((tableInfo, index) => {
                    return {
                        tableInfo: tableInfo,
                        rowCount: tableInfo.rowCount(),
                        colCount: tableInfo.colCount()
                    };
                });
                table_list_view.model = currentTables.length;
                messageBox("成功", `PDF解析完成，共识别到 ${currentTables.length} 个表格`);
            },
            // 失败回调
            (errorMsg) => {
                isLoading = false;
                messageBox("错误", `PDF解析失败：${errorMsg}`);
            }
        );
    }

    // -------------------------- 3. 导出表格到Excel（对接C++ TableToExcel类）--------------------------
    function exportSelectedTable() {
        // 校验选中状态
        if (selectedTableIndex === -1 || currentTables.length === 0) {
            messageBox("提示", "请先选择一个表格");
            return;
        }

        const selectedTable = currentTables[selectedTableIndex];
        if (!selectedTable.tableInfo) {
            messageBox("错误", "表格数据无效");
            return;
        }

        // 打开保存文件对话框
        Qt.createQmlObject(`
            import QtQuick.Dialogs
            FileDialog {
                title: "保存Excel文件"
                nameFilters: ["Excel文件 (*.xlsx)"]
                selectExisting: false
                // 默认文件名：表格1_文档名.xlsx
                defaultFileName: \`表格\${selectedTableIndex + 1}_\${getCurrentDocName()}.xlsx\`
                onAccepted: {
                    let savePath = Helper.qurl2localfile(currentFile);
                    exportToExcel(selectedTable.tableInfo, savePath);
                }
            }
        `, view).open();
    }

    // 辅助：获取当前选中的文档名称
    function getCurrentDocName() {
        if (isPdfMode) return "PDF解析表格";
        if (selectedDocIndex === -1 || selectedDocIndex >= openedDocs.length + 1) return "未知文档";
        return openedDocs[selectedDocIndex - 1]?.tabName || "未知文档";
    }

    // 核心：调用C++ TableToExcel::extractTableToExcel导出
    function exportToExcel(tableInfo, savePath) {
        if (!TableToExcel || !tableInfo || !savePath) {
            messageBox("错误", "导出接口未就绪");
            return;
        }

        // 显示导出中提示
        isLoading = true;
        // 调用C++静态方法（注意：TableToExcel需注册到QML，extractTableToExcel需为Q_INVOKABLE）
        setTimeout(() => {
            try {
                // 核心调用：C++ TableToExcel::extractTableToExcel(TableInfo*, const char*)
                const success = TableToExcel.extractTableToExcel(tableInfo, savePath);
                if (success) {
                    messageBox("成功", `Excel导出成功！\n保存路径：${savePath}`);
                } else {
                    messageBox("错误", "Excel导出失败，请重试");
                }
            } catch (e) {
                messageBox("错误", `导出失败：${e.message}`);
            } finally {
                isLoading = false;
            }
        }, 500); // 模拟导出延迟，实际可去掉
    }

    // -------------------------- UI布局（保持原有，新增加载状态显示）--------------------------
    GridLayout {
        anchors.fill: parent
        columns: 3
        columnSpacing: 15
        rowSpacing: 12
        anchors.topMargin: 16
        anchors.bottomMargin: 16
        anchors.leftMargin: 16
        anchors.rightMargin: 16

        // 标题（跨3列）
        TText {
            text: "- 表格转Excel工具 -"
            Layout.columnSpan: 3
            Layout.alignment: Qt.AlignHCenter
            font.bold: true
            font.pixelSize: 16
        }

        // 左侧：模式选择+文档选择（占1列）
        ColumnLayout {
            Layout.rowSpan: 4
            Layout.fillHeight: true
            spacing: 15

            // 模式选择分组（修复偏下问题）
            GroupBox {
                title: "提取模式"
                Layout.fillWidth: true

                ColumnLayout {
                    anchors.top: parent.top // 顶部对齐
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.margins: 8
                    spacing: 12 // 增大选项间距，视觉更舒适

                    // 模式1：从已打开文档提取
                    RadioButton {
                        id: rb_opened_doc
                        text: "从已打开文档提取"
                        checked: true
                        Material.accent: "#B8991E"
                        onClicked: {
                            isPdfMode = false;
                            updateOpenedDocList();
                        }
                    }

                    // 模式2：从PDF文件提取
                    RadioButton {
                        id: rb_pdf_file
                        text: "从PDF文件提取"
                        Material.accent: "#B8991E"
                        onClicked: {
                            isPdfMode = true;
                            openPDFAndParse(); // 触发PDF选择
                        }
                    }
                }
            }

            // 已打开文档选择（仅模式1显示）
            GroupBox {
                title: "已打开文档"
                Layout.fillWidth: true
                visible: rb_opened_doc.checked
                Layout.preferredHeight: 120

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 8

                    TCombo {
                        id: combo_opened_docs
                        Layout.fillWidth: true
                        onCurrentIndexChanged: {
                            // 过滤占位项（索引0为"选择文档"或"暂无已打开文档"）
                            if (currentIndex === 0 || openedDocs.length === 0) {
                                selectedDocIndex = -1;
                                currentTables = [];
                                table_list_view.model = 0;
                                return;
                            }
                            // 真实文档索引 = 当前索引 - 1（跳过占位项）
                            selectedDocIndex = currentIndex - 1;
                            const selectedDoc = openedDocs[selectedDocIndex];
                            if (selectedDoc) {
                                loadTablesFromDoc(selectedDoc); // 切换文档时重新加载表格
                            }
                        }
                    }

                    RectButton {
                        text: "+ 打开新文档"
                        accent: "#A34441"
                        Layout.fillWidth: true
                        onClicked: {
                            // 调用主软件打开Souple文档接口（主软件提供）
                            if (main_titleBar.openDocument) {
                                main_titleBar.openDocument((success, doc) => {
                                    if (success) {
                                        updateOpenedDocList(); // 打开后刷新文档列表
                                    }
                                });
                            } else {
                                messageBox("提示", "打开文档功能未就绪");
                            }
                        }
                    }
                }
            }
        }

        // 中间+右侧：表格列表+导出按钮（占2列）
        ColumnLayout {
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 12

            // 表格列表分组
            GroupBox {
                title: "已识别表格"
                Layout.fillWidth: true
                Layout.fillHeight: true

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 8

                    TText {
                        text: currentTables.length === 0
                            ? "暂无表格（选择文档或PDF后自动识别）"
                            : `共识别到 ${currentTables.length} 个表格`;
                        color: currentTables.length === 0 ? "gray" : theme.fg;
                    }

                    // 加载状态提示（覆盖在列表上）
                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        visible: isLoading

                        Rectangle {
                            anchors.fill: parent
                            color: theme.bg + "cc" // 半透明背景
                            BusyIndicator {
                                anchors.centerIn: parent
                                running: isLoading
                                Material.accent: "#B8991E"
                                width: 40
                                height: 40
                            }
                            TText {
                                anchors.centerIn: parent
                                anchors.topMargin: 50
                                text: "正在提取表格..."
                                color: theme.fg
                            }
                        }
                    }

                    // 表格列表视图
                    ListView {
                        id: table_list_view
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        model: currentTables.length
                        clip: true
                        ScrollBar.vertical: ScrollBar {}
                        enabled: !isLoading // 加载时禁用列表

                        delegate: Item {
                            width: parent.width
                            height: 45
                            Rectangle {
                                anchors.fill: parent
                                color: index === selectedTableIndex ? "#B8991E33" : "transparent"
                                border.color: index === selectedTableIndex ? "#B8991E" : "transparent"
                                border.width: 1

                                ColumnLayout {
                                    anchors.centerIn: parent
                                    spacing: 4
                                    TText {
                                        text: `表格 ${index + 1}`;
                                        font.bold: true;
                                        color: theme.fg;
                                    }
                                    TText {
                                        // 显示真实表格行列数（从currentTables获取）
                                        text: `${currentTables[index].rowCount} 行 × ${currentTables[index].colCount} 列`;
                                        font.pixelSize: 11;
                                        color: "gray";
                                    }
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    selectedTableIndex = index;
                                }
                            }
                        }
                    }
                }
            }

            // 导出按钮（靠右对齐）
            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 40

                FlatButton {
                    text: "导出选中表格到Excel"
                    anchors.right: parent.right
                    padding_vertical: 8
                    padding_horizontal: 20
                    foldV: 0.2
                    accent: "#B8991E"
                    enabled: !isLoading && selectedTableIndex !== -1 && currentTables.length > 0
                    onClicked: {
                        exportSelectedTable();
                    }
                }
            }
        }
    }
}
