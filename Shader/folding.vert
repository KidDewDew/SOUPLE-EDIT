 #version 440
 layout(location = 0) in vec4 qt_Vertex;
 layout(location = 1) in vec2 qt_MultiTexCoord0;
 layout(location = 0) out vec2 coord;
 layout(location = 1) out float dark;
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
 void main() {
     coord = qt_MultiTexCoord0;
	 vec4 p = qt_Vertex;
	 float w0 = mx*w, w1 = w-w0;
	 if(mx >= 0.0){
		if(coord.x < mx) {
			p.y += coord.x * d / mx;
			float tx = w0-sqrt(w0*w0-d*d);
			p.x += tx*(1.0-coord.x/mx);
			float s = (mx-coord.x)/mx;
			dark = d>0.0?0.0:1.0;
		} else {
			p.y += (1.0-coord.x)*d/(1.0-mx);
			float tx = w1-sqrt(w1*w1-d*d);
			p.x -= tx*(coord.x-mx)/(1.0-mx);
			dark = d>0.0?1.0:0.0;
		}
		//d^2+(coord.x-x)^2=(coord.x-mx)^2
	 }
     gl_Position = qt_Matrix * p;
 }
