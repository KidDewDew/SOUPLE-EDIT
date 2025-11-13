#version 440

//片段着色器
layout(location = 0) in vec2 coord;
layout(location = 1) in float dark;
layout(location = 0) out vec4 fragColor;
layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
	vec2 light_pos;
    float qt_Opacity;
	 float w;
	 float h;
	 float d;
	 float mx;
	 float darkv;
	 float light_r;
};

layout(binding = 1) uniform sampler2D src;

void main() {
    vec4 color = texture(src, coord);
	if(dark>0.5) {
		color.rgb = color.rgb * darkv;
	}
	float ld = length(coord*vec2(w*0.75,h)-light_pos);
	color.rgb = color.rgb * ((1.0-smoothstep(0.0,light_r,ld))*0.8+1.0);
	float alpha = 1.0;
    if(coord.x<0.02) alpha = coord.x * 50.0;
	else if(coord.x>0.98) alpha = (1.0-coord.x) * 50.0;
	else if(coord.y<0.02) alpha = coord.y * 50.0;
	else if(coord.y>0.98) alpha = (1.0-coord.y) * 50.0;
	fragColor = color * qt_Opacity * alpha;
}