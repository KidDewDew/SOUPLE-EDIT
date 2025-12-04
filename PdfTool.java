package person.liushuo;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.Insets;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.imageio.ImageIO;
import javax.swing.text.StyledEditorKit.ForegroundAction;

import org.apache.pdfbox.cos.COSDictionary;
import org.apache.pdfbox.cos.COSDocument;
import org.apache.pdfbox.cos.COSInputStream;
import org.apache.pdfbox.cos.COSName;
import org.apache.pdfbox.cos.COSObject;
import org.apache.pdfbox.cos.COSStream;
import org.apache.pdfbox.pdmodel.PDDocument;
import org.apache.pdfbox.pdmodel.common.PDStream;
import org.apache.pdfbox.pdmodel.font.PDFont;
import org.apache.pdfbox.pdmodel.font.PDFontDescriptor;
import org.apache.pdfbox.pdmodel.font.PDFontFactory;
import org.json.JSONArray;
import org.json.JSONObject;
import org.scilab.forge.jlatexmath.TeXConstants;
import org.scilab.forge.jlatexmath.TeXFormula;
import org.scilab.forge.jlatexmath.TeXIcon;


public class PdfTool {

	//private static Pattern pat_cmap = Pattern.compile("<([0-9a-fA-F]+)>\\s*<([0-9a-fA-F]+)>");
	
	private static Pattern pat_cmap = Pattern.compile("<([0-9a-fA-F]+)>");
	
	public static void main(String[] args) throws IOException 
	{
		extractEmbeddedFonts("C:\\Users\\guest0\\Desktop\\作业一刘硕2327406019.pdf",
				"C:\\Users\\guest0\\Desktop\\ftest");
		//System.out.print(json);
	}
	
	/**
	 * @brief 把latex公式转换为图片(png格式)，利用开源库jlatexmath
	 * @param latex: latex公式字符串
	 * @param image_save_path: 转换的image保存路径
	 * @return 返回"ok": 成功 返回其他内容：转换失败，具体为报错输出。
	 */
	public static String latexFormula2image(String latex,float fontSize,String image_save_path) 
	{
		try {
			TeXFormula formula = new TeXFormula(latex);
			TeXIcon icon = formula.createTeXIcon(TeXConstants.STYLE_DISPLAY, fontSize);
	        icon.setInsets(new Insets(5, 5, 5, 5)); // 公式边距
	        
	        BufferedImage image = new BufferedImage(
	                icon.getIconWidth(), 
	                icon.getIconHeight(), 
	                BufferedImage.TYPE_INT_ARGB
	        );
	        
	        Graphics2D g2d = image.createGraphics();
	        
	        // 设置背景：透明（new Color(0,0,0,0)）或白色（Color.WHITE）
	        g2d.setColor(new Color(0, 0, 0, 0));
	        g2d.fillRect(0, 0, icon.getIconWidth(), icon.getIconHeight());
	        // 渲染公式
	        icon.paintIcon(null, g2d, 0, 0);
	        g2d.dispose(); // 释放资源
	        
	        ImageIO.write(image, "png", new File(image_save_path));
		} catch(Exception e) {
			return e.toString();
		}
		return "ok";
	}
	
	/**
	 * @brief 提取pdf内嵌字体以及内嵌CMAP表(如果有)到save_path目录。
	 * @param pdf_file: 待提取的pdf文件路径。
	 * @param save_path: 字体文件和cmap文件保存到的路径
	 * @return 返回JSON字符串
	 * @throws IOException
	 * @notice 保存的cmap文件格式：
	 * 			1.后缀名 = ".cmap"
	 *          2.文件名称 = %字体名%+后缀名
	 *          3.cmap格式:(数字为字符串格式的无前后缀的16进制)
	 *          		1000,1230\n
	 *                  198A,334F\n
	 *                  ...
	 *                  1200,234F
	 *                  (注：末尾行没有\n)
	 * @notice 该函数可以自动创建save_path目录(如果未创建的话)
	 */
	public static String extractEmbeddedFonts(String pdf_file,String save_path) throws IOException
	{
		File save_dir = new File(save_path);
		if(!save_dir.exists()) {
			save_dir.mkdirs();
		}
		
		PDDocument document = PDDocument.load(new File(pdf_file));
		
		JSONArray json = new JSONArray();
		
		if(document == null) 
			return "";
		
		COSDocument cosDocument = document.getDocument();
		List<COSObject> font_objs = cosDocument.getObjectsByType(COSName.FONT);
		
		//遍历所有pdf内嵌字体
		for(COSObject font_obj : font_objs) 
		{
			COSDictionary dict = (COSDictionary) font_obj.getObject();
			//从字典中获取字体名称
			String fontName = dict.getCOSName(COSName.BASE_FONT).getName();
			JSONObject json_object = new JSONObject();
			
			json_object.put("FontName",fontName);
			
			//1.提取字体文件
			PDFont font = null;
			try {
				font = PDFontFactory.createFont(dict);
				//System.out.println(font);
			} catch(Exception e) {
				continue;
				 //发生异常意味着该obj是decendant font
			}
			
			if(font.isDamaged()) { //读取失败
				continue;
			}
			
			//System.out.println(font.isDamaged());
			
			if(font != null) {
				
				PDFontDescriptor fd = font.getFontDescriptor();
				PDStream s1 = fd.getFontFile();
				PDStream s2 = fd.getFontFile2();
				PDStream s3 = fd.getFontFile3();
				COSInputStream cos_is = null;
				String suffix = new String();
				if(s1 != null) {
					cos_is = s1.createInputStream();
					suffix = ".pfb";
				} else if(s2 != null) {
					cos_is = s2.createInputStream();
					suffix = ".ttf";
				} else if(s3 != null) {
					cos_is = s3.createInputStream();
					suffix = ".otf";
				}
				if(cos_is == null) {
					continue; //data流打开失败
				}
				json_object.put("FontFile",save_path+"\\"+fontName+suffix);
				int length = cos_is.available();
				byte[] data = new byte[length];
				cos_is.read(data,0,length);
				cos_is.close();
				FileOutputStream os = new FileOutputStream(save_path+"\\"+fontName+suffix);
				os.write(data);
				os.close();
			}
			
			
			
			//2.如果有内嵌cmap，提取内嵌cmap
			if(dict.containsKey(COSName.TO_UNICODE)) 
			{
				json_object.put("hasCMapFile",true);
				COSStream cmap = (COSStream) dict.getDictionaryObject(COSName.TO_UNICODE);
				String cmap_text = cmap.toTextString();
				//System.out.println(cmap_text);
				StringBuilder cmap_sb = new StringBuilder();
				//咱们要把cmap_text里面的有用的东西提取出来
//				Matcher matcher = pat_cmap.matcher(cmap_text);
//				System.out.println(cmap_text);
//				if(matcher.find()) {
//					while(true) {
//						cmap_sb.append(matcher.group(1));
//						cmap_sb.append(',');
//						cmap_sb.append(matcher.group(2));
//						if(matcher.find()) cmap_sb.append('\n');
//						else break;
//					}
//				}
				
				boolean firstLine = true;
				for(String line : cmap_text.split("\n")) {
					Matcher matcher = pat_cmap.matcher(line);
					ArrayList<String> list = new ArrayList<String>();
					while(matcher.find()) {
						list.add(matcher.group(1));
					}
					if(list.size() < 2) continue;
					if(list.size() <= 3) {
						cmap_sb.append(list.get(0));
						cmap_sb.append(',');
						cmap_sb.append(list.get(list.size()-1));
						cmap_sb.append('\n');
					} else {
						// <0000> <0001> [<dddd> ... <dddd>]
						int from = hexStringToInt(list.get(0));
						for(int i = 2; i < list.size(); ++i) {
							cmap_sb.append(intToHexString(from,true));
							cmap_sb.append(',');
							cmap_sb.append(list.get(i));
							cmap_sb.append('\n');
							++ from;
						}
					}
				}
				
				if(cmap_sb.length() > 0) {
					cmap_sb.deleteCharAt(cmap_sb.length() - 1);
				}
				
				json_object.put("CMapFile",save_path+"\\"+fontName+".cmap");
				FileOutputStream os = new FileOutputStream(save_path+"\\"+fontName+".cmap");
				os.write(cmap_sb.toString().getBytes());
				os.close();
				//补充.
			} else {
				json_object.put("hasCMapFile",false);
			}
			
			
			json.put(json_object);
		}
		
		document.close();
		
		return json.toString();
    }
	
	
	/**
	 * 将十六进制字符串转换为整数
	 * @param hexStr 十六进制字符串（支持大小写，无前缀）
	 * @return 对应的整数值
	 * @throws NumberFormatException 如果字符串不是有效的十六进制格式
	 */
	public static int hexStringToInt(String hexStr) {
	    if (hexStr == null || hexStr.isEmpty()) {
	        throw new NumberFormatException("Hex string is null or empty");
	    }
	    
	    // 处理可能的负号
	    boolean isNegative = false;
	    if (hexStr.charAt(0) == '-') {
	        isNegative = true;
	        hexStr = hexStr.substring(1);
	    }
	    
	    if (hexStr.isEmpty()) {
	        throw new NumberFormatException("Hex string contains only minus sign");
	    }
	    
	    // 转换为大写，统一处理（十六进制不区分大小写）
	    hexStr = hexStr.toUpperCase();
	    int result = 0;
	    
	    for (int i = 0; i < hexStr.length(); i++) {
	        char c = hexStr.charAt(i);
	        int digit;
	        
	        if (c >= '0' && c <= '9') {
	            digit = c - '0';
	        } else if (c >= 'A' && c <= 'F') {
	            digit = 10 + (c - 'A');
	        } else {
	            throw new NumberFormatException("Invalid hex character: " + c);
	        }
	        
	        result = result * 16 + digit;
	    }
	    
	    return isNegative ? -result : result;
	}
	
	/**
	 * 将整数转换为十六进制字符串
	 * @param number 要转换的整数
	 * @param uppercase 是否使用大写字母（true-大写，false-小写）
	 * @return 十六进制字符串（无前缀）
	 */
	public static String intToHexString(int number, boolean uppercase) {
	    if (number == 0) {
	        return "0";
	    }
	    
	    // 处理负数使用补码表示
	    long num = number & 0xFFFFFFFFL;
	    StringBuilder hexBuilder = new StringBuilder();
	    
	    // 十六进制字符集
	    char[] hexChars = uppercase ? 
	        new char[]{'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'} :
	        new char[]{'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
	    
	    // 转换为十六进制
	    while (num > 0) {
	        int digit = (int)(num % 16);
	        hexBuilder.append(hexChars[digit]);
	        num = num / 16;
	    }
	    
	    return hexBuilder.reverse().toString();
	}

	/**
	 * 重载方法，默认使用小写
	 */
	public static String intToHexString(int number) {
	    return intToHexString(number, false);
	}
	
}


//import org.apache.poi.xwpf.usermodel.*;
//
//import java.io.FileOutputStream;
//import java.io.IOException;
//
//public class PdfTool {
//    public static void main(String[] args) throws IOException {
//        // 1. 创建空文档
//        XWPFDocument document = new XWPFDocument();
//        
//        // 2. 创建段落
//        XWPFParagraph title = document.createParagraph();
//        title.setAlignment(ParagraphAlignment.CENTER);
//        
//        // 3. 创建文本运行并设置样式
//        XWPFRun titleRun = title.createRun();
//        titleRun.setText("Apache POI示例文档");
//        titleRun.setBold(true);
//        titleRun.setFontSize(16);
//        
//        // 4. 添加正文段落
//        XWPFParagraph body = document.createParagraph();
//        body.setAlignment(ParagraphAlignment.LEFT);
//        body.setIndentationFirstLine(600); // 首行缩进
//        
//        XWPFRun bodyRun = body.createRun();
//        bodyRun.setText("这是使用Apache POI创建的Word文档。");
//        bodyRun.addBreak(); // 换行
//        bodyRun.setText("POI支持丰富的文本格式设置。");
//        
//        // 5. 保存文档
//        try (FileOutputStream out = new FileOutputStream("E:/SimpleDocument.docx")) {
//            document.write(out);
//        }
//        
//        document.close();
//        System.out.println("文档创建成功！");
//    }
//}
//
