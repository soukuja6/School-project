#version 330	// GLSL version

// model-view transformation
uniform mat4 transformation;


// The position of a vertex (per-vertex, from the VBO)
layout (location = 0) in vec3 position; 

// Output vertex color (per-vertex, interpolated and passed to frag shader)
out vec4 color;

void main() {
	// translate the vertex
    gl_Position = transformation * vec4(position, 1.);	
	
	// set the color of the vertex
	color.r = 0.5;
	color.g = 0.5;
	color.b = 0.5;

	//color.r = clamp(gl_Position.x, 0., 1.);
	//color.g = clamp(gl_Position.y, 0., 1.);
	//color.b = clamp(gl_Position.z, 0., 1.);
	color.a = 1.;	
}