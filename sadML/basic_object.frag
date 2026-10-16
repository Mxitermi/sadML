#version 330 core

layout(location = 0) out vec4 f_color;

in vec3 v_pos;
in vec4 v_color;


void main()
{
    
    vec3 p = abs(v_pos);
    float ratio = 0.99;

    if((p.x > ratio && p.y < -ratio) || (p.x > ratio && p.y > ratio) || (p.x > ratio && p.z > ratio) || (p.z > ratio && p.y > ratio) ){
    f_color = vec4(0.3, 0.3, 0.3, 1.0);
    }else {
    f_color = vec4(0.15, 0.15, 0.15, 1.0);
    }
    
}