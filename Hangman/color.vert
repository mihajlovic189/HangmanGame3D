#version 330 core

    layout(location=0) in vec2 aPos;
    uniform vec2 uPos; 
    uniform vec2 uScale;
    void main(){
        vec2 p = aPos * uScale + uPos;
        gl_Position = vec4(p,0.0,1.0);
    }