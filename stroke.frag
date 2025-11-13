#version 440

//片段着色器
layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;
layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
	vec4 strokeColor;
	int strokeWidth;
};

layout(binding = 1) uniform sampler2D source;

void main() {
    vec4 color = texture(source, qt_TexCoord0);
	vec2 size = textureSize(source,0); //获取纹理尺寸
	if(color.a > 0.999 || strokeWidth == 0) { //本像素是内容像素
		fragColor = color * qt_Opacity;
		return;
	}
	//检测周围是否有不透明的像素
	vec2 sw = vec2(strokeWidth / size.x,strokeWidth / size.y);
	float a = 0.0;
	for(float i = -1.0; i <= 1.0; i += 1) {
		for(float j = -1.0; j <= 1.0; j += 1) {
			if(i == 0 && j == 0) continue;
			a = max(a,texture(source,qt_TexCoord0+vec2(i*sw.x,j*sw.y)).a);
		}
	}
	if(a > 0.0) {
		fragColor = strokeColor*vec4(1.0,1.0,1.0,a) * qt_Opacity;
	} else {
		fragColor = color * qt_Opacity;
	}
}