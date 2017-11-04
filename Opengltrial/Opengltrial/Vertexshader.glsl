#version 330	// GLSL version

// In this version, the illumination is computed in the vertex shader, the 
// resulting color is then interpolated and, in the fragment shader it is 
// assigned to the fragment. Note that you can also pass the normals from the 
// vertex to the fragment shader and compute the illumination in the fragment
// shader. It would be more computationally expensive, but the results look 
// far better.

// model-view transformation
uniform mat4 transformation;
uniform mat4 vertextransformation;
uniform mat4 normaltransformation;


// vertex attributes
layout (location = 0) in vec3 position; 
layout (location = 1) in vec2 tex_coords; 
layout (location = 2) in vec3 normal; 


// pass the texture coordinates to the fragment shader
out vec2 cur_tex_coords;

// compute the color here and pass it to the fragment shader
out vec4 fcolor;
out vec3 positionout;
out vec3 normalout;

void main() {
	// transform the vertex
    gl_Position = transformation * vertextransformation * vec4(position, 1.);	
	// pass the texture coordinates to the fragment shader
	cur_tex_coords = tex_coords;
	positionout = vec3(vertextransformation*vec4(position, 1.));
	normalout = normalize(vec3(normaltransformation*vec4(normal,1.))) ;
	
}


