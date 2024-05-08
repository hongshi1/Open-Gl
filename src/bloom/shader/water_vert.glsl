#version 330 core
 
uniform mat4 ModelMatrix;
uniform mat4 IT_ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectMatrix;
 
 
attribute vec4 a_position;
attribute vec3 a_normal;
attribute vec3 a_tangent;
attribute vec2 a_texcoord;
 
varying vec2 v_texcoord;
varying vec3 v_normal;
varying vec3 v_tangent;
varying vec3 worldPos;
 
void main()
{
    gl_Position = ModelMatrix * a_position;
    worldPos = vec3(gl_Position);
    gl_Position = ViewMatrix * gl_Position;
    gl_Position = ProjectMatrix * gl_Position;
    v_texcoord = a_texcoord;
 
    v_normal = mat3(IT_ModelMatrix) * a_normal;
    v_tangent = mat3(ModelMatrix) * a_tangent;
}