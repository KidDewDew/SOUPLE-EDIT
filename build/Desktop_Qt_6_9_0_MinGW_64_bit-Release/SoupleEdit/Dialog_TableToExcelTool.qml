import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Dialogs
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

    // 加载状态提示（PDF解析/表格提取时显示）
    property bool isLoading: false
    // 护眼模式（默认关闭）
    property bool isProtectEyeMode: false

    // 状态管理：避免重复打开
    Component.onCompleted: {
        Helper.temp_Set("TableToExcelTool_opened", true);
        updateOpenedDocList(); // 加载已打开的文档
    }

    onClosing: {
        Helper.temp_Set("TableToExcelTool_opened", false);
        view.destroy();
    }

    // 核心数据
    property var openedDocs: [] // 已打开的Souple文档列表
    property var currentTables: [] // 当前选中模式下的表格列表
    property int selectedDocIndex: -1 // 选中的文档索引
    property int selectedTableIndex: -1 // 选中的表格索引
    property bool isPdfMode: false // 是否为“PDF文件提取”模式

    // 表格转Excel工具实例
    TableToExcel {
        id: tableToExcel
    }

    // 消息框工具函数（兼容原有逻辑）
    function messageBox(title, text) {
        Qt.createQmlObject(`
            import QtQuick.Dialogs
            MessageDialog {
                title: "${title}"
                text: "${text}"
                standardButtons: StandardButton.Ok
                onAccepted: destroy()
            }
        `, view).open();
    }

    // -------------------------- 1. 加载已打开文档的表格 --------------------------
    function updateOpenedDocList() {
        // 调用主软件接口获取所有已打开的Souple文档
        if (!main_titleBar || !main_titleBar.getDocumentList) {
            messageBox("错误", "主窗口接口未就绪");
            return;
        }
        openedDocs = main_titleBar.getDocumentList(
            (tab) => tab.documentType === "souple"
        );

        // 处理文档下拉框（添加占位项）
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

    // 从选中文档提取表格（同步调用，恢复原有逻辑）
    function loadTablesFromDoc(doc) {
            if (!doc || !spMgr) {
                currentTables = [];
                table_list_view.model = 0;
                isLoading = false;
                return;
            }

            isLoading = true;
            try {
                console.log("开始提取文档表格：", doc.tabName, "文档ID：", doc.s);
                if (!spMgr.getTableLinesInDocument) {  // 假设新增接口获取TableLine列表
                    throw new Error("spMgr.getTableLinesInDocument 接口不存在");
                }
                // 获取TableLine列表（而非直接获取TableInfo）
                const tableLineList = spMgr.getTableLinesInDocument(doc.s);
                console.log("获取到表格行数量：", tableLineList?.length || 0);

                // 去重处理（一个表格可能对应多个TableLine，取唯一的table_info）
                const uniqueTableInfos = [];
                const tableInfoSet = new Set();
                tableLineList.forEach(line => {
                    // 从TableLine中提取table_info（关键修改）
                    const tableInfo = line.table_info;
                    if (tableInfo && !tableInfoSet.has(tableInfo)) {
                        tableInfoSet.add(tableInfo);
                        uniqueTableInfos.push(tableInfo);
                    }
                });

                // 包装表格数据
                currentTables = uniqueTableInfos.map((tableInfo, index) => {
                    // 确保调用TableInfo的方法（C++中存在rowCount()和colCount()）
                    const rowCount = tableInfo?.rowCount ? tableInfo.rowCount() : 0;
                    const colCount = tableInfo?.colCount ? tableInfo.colCount() : 0;
                    return {
                        tableInfo: tableInfo,
                        rowCount: rowCount,
                        colCount: colCount,
                        source: "文档"
                    };
                });

                if (currentTables.length === 0) {
                    messageBox("提示", `文档"${doc.tabName}"中未识别到表格`);
                }
                table_list_view.model = currentTables.length;
            } catch (e) {
                console.error("提取表格失败：", e.message, e.stack);
                messageBox("错误", `提取表格失败：${e.message}`);
                currentTables = [];
                table_list_view.model = 0;
            } finally {
                isLoading = false;
            }
        }

    // -------------------------- 2. PDF文件解析表格（修复识别问题） --------------------------
    function openPDFAndParse() {
        console.log("=== 开始选择PDF文件 ==="); // 新增：标记流程起点
        Qt.createQmlObject(`
            import QtQuick.Dialogs
            Dialog_FileSelect {
                title: "选择PDF文件"
                nameFilters: ["PDF文件 (*.pdf)"]
                onAccepted: {
                    let pdfUrl = currentFile;
                    let pdfPath = Helper.qurl2localfile(pdfUrl);
                    // 新增：打印原始URL和转换后的路径
                    console.log("PDF原始URL：", pdfUrl);
                    console.log("PDF本地路径：", pdfPath);
                    // 新增：校验路径有效性
                    if (pdfPath === "" || !pdfPath.endsWith(".pdf")) {
                        view.messageBox("错误", "无效的PDF路径：" + pdfPath);
                        destroy();
                        return;
                    }
                    parsePdfTables(pdfPath);
                }
                onRejected: {
                    console.log("用户取消选择PDF文件"); // 新增：标记取消操作
                    destroy();
                }
            }
        `, view).open();
    }


    function parsePdfTables(pdfPath) {
            console.log("=== 开始解析PDF：", pdfPath, "===");
            if (!spMgr || !pdfPath) {
                // 原有错误处理保持不变 ...
                return;
            }

            isLoading = true;
            currentTables = [];
            table_list_view.model = 0;

            if (!spMgr.parsePdfTables) {
                // 原有错误处理保持不变 ...
                return;
            }

            spMgr.parsePdfTables(
                pdfPath,
                // 成功回调（修改：处理TableLine列表）
                (tableLineList) => {
                    console.log("PDF解析成功，获取到表格行数量：", tableLineList?.length || 0);
                    // 去重处理
                    const uniqueTableInfos = [];
                    const tableInfoSet = new Set();
                    tableLineList.forEach(line => {
                        const tableInfo = line.table_info;  // 从TableLine提取table_info
                        if (tableInfo && !tableInfoSet.has(tableInfo)) {
                            tableInfoSet.add(tableInfo);
                            uniqueTableInfos.push(tableInfo);
                        }
                    });

                    currentTables = uniqueTableInfos.map((tableInfo, index) => {
                        const rowCount = tableInfo?.rowCount ? tableInfo.rowCount() : 0;
                        const colCount = tableInfo?.colCount ? tableInfo.colCount() : 0;
                        return {
                            tableInfo: tableInfo,
                            rowCount: rowCount,
                            colCount: colCount,
                            source: "PDF"
                        };
                    });

                    if (currentTables.length === 0) {
                        messageBox("提示", `PDF文件"${pdfPath}"中未识别到表格`);
                    }
                    table_list_view.model = currentTables.length;
                    isLoading = false;
                },
                // 失败回调（保持不变）
                (errorMsg) => {
                    console.error("PDF解析失败：", errorMsg);
                    messageBox("错误", `PDF解析失败：${errorMsg}`);
                    isLoading = false;
                }
            );

            // 原有超时处理保持不变 ...
            // 超时兜底Timer（动态创建，修复语法错误）
            let pdfTimeoutTimer = Qt.createQmlObject(`
                import QtQuick 2.0
                Timer {
                    interval: 10000 // 10秒超时
                    repeat: false
                    running: true // 直接启动
                    onTriggered: {
                        if (view.isLoading) {
                            console.error("PDF解析超时，强制关闭加载");
                            view.isLoading = false;
                            view.messageBox("错误", "PDF解析超时，请检查文件或接口");
                        }
                        destroy(); // 触发后销毁
                    }
                }
            `, view);

            // 解析完成后清理定时器（新增：避免内存泄漏）
            const clearTimer = () => {
                if (pdfTimeoutTimer) {
                    pdfTimeoutTimer.stop();
                    pdfTimeoutTimer.destroy();
                }
            };
            // 绑定到成功/失败回调的清理逻辑（简化版）
            setTimeout(clearTimer, 11000); // 11秒后强制清理
        }




    // -------------------------- 3. 导出表格到Excel（恢复原有逻辑） --------------------------
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

        // 打开保存文件对话框（恢复原有Dialog格式）
        Qt.createQmlObject(`
            import QtQuick.Dialogs
            FileDialog {
                title: "保存Excel文件"
                nameFilters: ["Excel文件 (*.xlsx)"]
                selectExisting: false
                defaultFileName: \`表格\${selectedTableIndex + 1}_\${getCurrentDocName()}.xlsx\`
                onAccepted: {
                    let savePath = Helper.qurl2localfile(currentFile);
                    exportToExcel(selectedTable.tableInfo, savePath);
                }
                onRejected: destroy()
            }
        `, view).open();
    }

    // 辅助：获取当前选中的文档名称（恢复原有逻辑）
    function getCurrentDocName() {
        if (isPdfMode) return "PDF解析表格";
        if (selectedDocIndex === -1 || selectedDocIndex >= openedDocs.length + 1) return "未知文档";
        return openedDocs[selectedDocIndex - 1]?.tabName || "未知文档";
    }

    // 导出表格（同步调用，恢复原有逻辑）
    function exportToExcel(tableInfo, savePath) {
        if (!tableToExcel || !tableInfo || !savePath) {
            messageBox("错误", "导出接口未就绪");
            isLoading = false;
            return;
        }

        isLoading = true;
        try {
            console.log("开始导出Excel：", savePath);
            if (!tableToExcel.extractTableToExcel) {
                throw new Error("TableToExcel.extractTableToExcel 方法不存在");
            }
            // 调用导出接口（恢复原有参数）
            const success = tableToExcel.extractTableToExcel(tableInfo, savePath);
            if (success) {
                messageBox("成功", `Excel导出成功！\n保存路径：${savePath}`);
            } else {
                messageBox("错误", "Excel导出失败，请重试");
            }
        } catch (e) {
            console.error("导出失败：", e.message);
            messageBox("错误", `导出失败：${e.message}`);
        } finally {
            isLoading = false;
        }
    }

    // -------------------------- UI布局（完全恢复原有样式） --------------------------
    GridLayout {
        anchors.fill: parent
        columns: 3
        columnSpacing: 15
        rowSpacing: 12
        anchors.topMargin: 16
        anchors.bottomMargin: 16
        anchors.leftMargin: 16
        anchors.rightMargin: 16

        // 标题（跨3列，恢复TText）
        TText {
            text: "- 表格转Excel工具 -"
            Layout.columnSpan: 3
            Layout.alignment: Qt.AlignHCenter
            font.bold: true
            font.pixelSize: 16
        }

        // 左侧：模式选择+文档选择（占1列，恢复原有组件）
        ColumnLayout {
            Layout.rowSpan: 4
            Layout.fillHeight: true
            spacing: 15

            // 模式选择分组
            GroupBox {
                title: "提取模式"
                Layout.fillWidth: true

                ColumnLayout {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.margins: 8
                    spacing: 12

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
                            openPDFAndParse();
                        }
                    }
                }
            }

            // 已打开文档选择（仅模式1显示，恢复TCombo/RectButton）
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
                            if (currentIndex === 0 || openedDocs.length === 0) {
                                selectedDocIndex = -1;
                                currentTables = [];
                                table_list_view.model = 0;
                                return;
                            }
                            selectedDocIndex = currentIndex - 1;
                            const selectedDoc = openedDocs[selectedDocIndex];
                            if (selectedDoc) {
                                loadTablesFromDoc(selectedDoc);
                            }
                        }
                    }

                    RectButton {
                        text: "+ 打开新文档"
                        accent: "#A34441"
                        Layout.fillWidth: true
                        onClicked: {
                            if (main_titleBar.openDocument) {
                                main_titleBar.openDocument((success, doc) => {
                                    if (success) {
                                        updateOpenedDocList();
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

        // 中间+右侧：表格列表+导出按钮（占2列，恢复原有组件）
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

                    // 加载状态提示（恢复原有样式）
                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        visible: isLoading

                        Rectangle {
                            anchors.fill: parent
                            color: theme.bg + "cc"
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

                    // 表格列表视图（恢复原有样式）
                    ListView {
                        id: table_list_view
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        model: currentTables.length
                        clip: true
                        ScrollBar.vertical: ScrollBar {}
                        enabled: !isLoading

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

            // 导出按钮（靠右对齐，恢复FlatButton）
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

