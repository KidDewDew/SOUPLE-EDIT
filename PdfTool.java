package person.liushuo;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.Insets;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.imageio.ImageIO;

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

	private static Pattern pat_cmap = Pattern.compile("<([0-9a-fA-F]+)>\\s*<([0-9a-fA-F]+)>");
	
	public static void main(String[] args) throws IOException 
	{
		extractEmbeddedFonts("C:\\Users\\guest0\\Desktop\\pdf\\pdf_reference_1.7(1).pdf",
				"C:\\Users\\guest0\\Desktop\\ftest");
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
				System.out.println(font);
			} catch(Exception e) {
				continue; //发生异常意味着该obj是decendant font
			}
			
			System.out.println(dict);
			
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
				Matcher matcher = pat_cmap.matcher(cmap_text);
				if(matcher.find()) {
					while(true) {
						cmap_sb.append(matcher.group(1));
						cmap_sb.append(',');
						cmap_sb.append(matcher.group(2));
						if(matcher.find()) cmap_sb.append('\n');
						else break;
					}
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
}
