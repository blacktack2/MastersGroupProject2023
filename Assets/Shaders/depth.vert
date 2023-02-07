#version 460 core
/**
 * @file   depth.vert
 * @brief  Simple depth vertex shader.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */

 uniform mat4 modelMatrix;
 uniform mat4 projViewMatrix;
 
layout(location = 0) in vec3 position;

 void main() {
	gl_Position = projViewMatrix * modelMatrix * vec4(position, 1.0);
 }
