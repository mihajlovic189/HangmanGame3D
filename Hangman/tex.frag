#version 330 core
in vec2 vUV;
out vec4 outColor;
uniform sampler2D uTex;
uniform float uAlpha;
void main(){
	 vec4 t = texture(uTex, vUV);
	 outColor = vec4(t.rgb, t.a * uAlpha);
}