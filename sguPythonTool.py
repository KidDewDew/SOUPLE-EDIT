import os.path
import sys
from itertools import filterfalse

from fontTools.cffLib import CFFFontSet
### sguPythonTool.py
### 读取 ./font_toAddCMap/下所有字体文件
### 添加CMap，并修改./font_toAddCMap/下原字体文件

from fontTools.ttLib import TTFont, newTable, TTLibError
from fontTools.ttLib.tables import _c_m_a_p
from fontTools.ttLib.tables import _n_a_m_e

# 补充名称表
def mendNameTable(font,fontName):
    name_table = newTable("name")

    # 为字体添加各种平台、编码下的名称定义，保证各个平台下可以被正确识别。
    name_table.setName('SoupleEdit 2025', 0, 1, 0, 0)
    name_table.setName(fontName, 1, 1, 0, 0)
    name_table.setName('Regular', 2, 1, 0, 0)
    name_table.setName('Souple 1.0', 3, 1, 0, 0)
    name_table.setName(fontName, 4, 1, 0, 0)
    name_table.setName('Version 1.0', 5, 1, 0, 0)
    name_table.setName(fontName, 6, 1, 0, 0)

    name_table.setName(fontName, 1, 3, 1, 1030)
    name_table.setName(fontName, 1, 3, 1, 1031)
    name_table.setName(fontName, 1, 3, 1, 1033)
    name_table.setName('Regular', 2, 3, 1, 1033)
    name_table.setName('Souple 1.0', 3, 3, 1, 1033)
    name_table.setName(fontName, 4, 3, 1, 1033)
    name_table.setName('Version 1.0', 5, 3, 1, 1033)
    name_table.setName(fontName, 6, 3, 1, 1033)
    name_table.setName(fontName, 1, 3, 0, 1033)
    name_table.setName(fontName, 1, 3, 1, 1034)
    name_table.setName(fontName, 1, 3, 1, 1035)
    name_table.setName(fontName, 1, 3, 1, 1036)
    name_table.setName(fontName, 1, 3, 1, 1040)
    name_table.setName(fontName, 1, 3, 1, 1043)
    name_table.setName(fontName, 1, 3, 1, 1044)
    name_table.setName(fontName, 1, 3, 1, 1051)
    name_table.setName(fontName, 1, 3, 1, 1053)
    name_table.setName(fontName, 1, 3, 1, 1060)
    name_table.setName(fontName, 1, 3, 1, 2070)
    name_table.setName(fontName, 1, 3, 1, 3082)

    name_table.setName(fontName, 1, 3, 1, 0)     #全编码

    font["name"] = name_table

def scan_and_mend_fontfile(font_file_path,toUnicode_file):
    try:
        font = TTFont(font_file_path)
    except TTLibError:
        return #无法解析的字体
    fontName = os.path.splitext(os.path.basename(font_file_path))[0]

    #如果fontName为子集形式，需要删除子集前缀
    # 如ABCDE+Wingdings -> Wingdings
    fontName = fontName.split('+')[-1]

    needSave = False

    if not font.has_key("name"):
        mendNameTable(font,fontName)
        needSave = True
    else:
        # 检查字体自带的名称表是否有问题
        names = font["name"].names
        #具体而言，检查名称中是否有为3_1_1033、1_0_0定义的
        has3_1_1033 = False
        has1_0_0 = False
        for name in names:
            if name.platformID == 3 and name.platEncID == 1 and name.langID == 1033:
                has3_1_1033 = True
            elif name.platformID == 1 and name.platEncID == 0 and name.langID == 0:
                has1_0_0 = True
            if has3_1_1033 and has1_0_0:
                break
        if has3_1_1033 == False or has1_0_0 == False:
            mendNameTable(font,fontName)  # 修补name表
            needSave = True

    # 如果有toUnicode文件，则补充cmap表
    if toUnicode_file != "null":
        needSave = True
        ## 补充cmap表
        cmap4_0_3 = _c_m_a_p.CmapSubtable.newSubtable(4)
        cmap4_0_3.platformID = 3
        cmap4_0_3.platEncID = 1
        cmap4_0_3.language = 0
        cmap4_0_3.cmap = {}

        with open(toUnicode_file,"r") as cmap_file:
            for line in cmap_file.readlines():
                glyph,char = line.strip('\n').split(',')
                char = int(char,16)
                glyph = int(glyph,16)
                print(char,'->',glyph)
                cmap4_0_3.cmap[char] = font.getGlyphName(glyph)

        cmap = newTable("cmap")
        cmap.tableVersion = 0
        cmap.tables = [cmap4_0_3]
        font["cmap"] = cmap

    if needSave:
        #font.save("C:\\Users\\guest0\\Desktop\\QWWWW2.ttf")
        font.save(font_file_path)

    return

if __name__ == "__main__":
    if len(sys.argv)  > 1:
        func = sys.argv[1]
        args = sys.argv[2:]
        n = len(args)
        if func == "fontMend":  #功能1 字体修复
            for i in range(n//2):
                scan_and_mend_fontfile(args[i*2],args[i*2+1])