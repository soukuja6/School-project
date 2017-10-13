#version 330	// GLSL version

// Camera position and direction;
uniform vec3 camera_position;
uniform vec3 cameradir;


//directional or head mounted
uniform bool directional;

// Directional light
uniform vec3 d_light_direction;
uniform vec3 d_light_a_color;
uniform float d_light_a_intensity;
uniform vec3 d_light_d_color;
uniform float d_light_d_intensity;
uniform vec3 d_light_s_color;
uniform float d_light_s_intensity;

// Head-mounted light //TODO: implement it yourself!
uniform vec3 p_light_a_color;
uniform float p_light_a_intensity;
uniform vec3 p_light_d_color;
uniform float p_light_d_intensity;
uniform vec3 p_light_s_color;
uniform float p_light_s_intensity;
uniform float klinear;
uniform float ksquared;
uniform float anglerest;
// TODO: other parameters

// Object material
// Notice that all of this values may be also specified per-vertex or 
//  through a texture.
uniform vec3 material_a_color;
uniform vec3 material_d_color;
uniform vec3 material_s_color;
uniform float material_shininess;


// Per-fragment color coming from the vertex shader
in vec3 positionout;
in vec3 normalout;

// Per-frgament output color
out vec4 FragColor;


void main() { 

    vec3 normal_nn = normalize(normalout);	
	vec3 view_dir_nn = normalize(camera_position - positionout);
	vec4 fcolor;
	float dist = length(camera_position - positionout);

	if(directional){
		// --- directional light ----
		// compute the required values and vectors
		// notice that input variables cannot be modified, so copy them first
		//vec3 d_light_dir_nn = normalize(mat3(vertextransformation)*d_light_direction);	
		
		vec3 d_light_dir_nn = normalize(d_light_direction);	
		
		float dot_d_light_normal = dot(-d_light_dir_nn, normal_nn);   // notice the minus!
		vec3 d_reflected_dir_nn = d_light_dir_nn + 2. * dot_d_light_normal * normal_nn;
		// should be already normalized, but we "need" to correct numerical errors
		d_reflected_dir_nn = normalize(d_reflected_dir_nn); 
	
		// compute the color contribution	
		vec3 color;
		vec3 amb_color = clamp(
				material_a_color * d_light_a_color * d_light_a_intensity,
				0.0, 1.0);
		vec3 diff_color = clamp(
				material_d_color * dot_d_light_normal * d_light_d_intensity * d_light_d_color,
				0.0, 1.0);      //added d_light_d_color because it should be there according to me and the presentation
		vec3 spec_color = clamp(
				material_s_color *  d_light_s_color * d_light_s_intensity *
				pow(clamp(dot(d_reflected_dir_nn, view_dir_nn),0.0,1.0), material_shininess),    // clamp to get rid of case when the cosinus is negative
				0.0, 1.0);     //added d_light_d_color because it should be there according to me and the presentation
		color = clamp(
				amb_color + diff_color + spec_color,
				0.0, 1.0);
		
		fcolor = vec4(color, 1.0);
	}
	else{
		// TODO: do the same for the headlight!
		// notice that for the headlight dot(view_dir, light_dir) = ...

		//p_light_dir_nn = view_dir_nn;
	
		vec3 p_light_dir_nn = -view_dir_nn;
	
		float dot_p_light_normal = dot(-p_light_dir_nn, normal_nn);   // notice the minus!
		vec3 p_reflected_dir_nn = p_light_dir_nn + 2. * dot_p_light_normal * normal_nn;
		// should be already normalized, but we "need" to correct numerical errors
		p_reflected_dir_nn = normalize(p_reflected_dir_nn); 
	
		//decrease intensity with distance(calculate coeficient of decreasing)
		float distancecoef = 1 / (1.0f + klinear*dist+ksquared*dist*dist);
		
		//restrict light to angle
		float intensitydeccoef=distancecoef;
		float lightToSurfaceAngle = degrees(acos(dot(p_light_dir_nn, normalize(cameradir))));
		if(lightToSurfaceAngle > anglerest){
			intensitydeccoef = 0.0;
		}

		


		// compute the color contribution
		vec3 color;
		vec3 amb_color = clamp(
				material_a_color * p_light_a_color * p_light_a_intensity * intensitydeccoef,
				0.0, 1.0);
		vec3 diff_color = clamp(
				material_d_color * dot_p_light_normal * p_light_d_intensity * intensitydeccoef* p_light_d_color,
				0.0, 1.0);
		vec3 spec_color = clamp(
				material_s_color *  p_light_s_color *  p_light_s_intensity * intensitydeccoef *
				pow(clamp(dot(p_reflected_dir_nn, view_dir_nn),0.0,1.0), material_shininess),
				0.0, 1.0);
		color = clamp(
				amb_color + diff_color + spec_color,
				0.0, 1.0);    
		
		// pass the reuslt to the fragment shader
		fcolor = vec4(color, 1.0);
	}

	// Set the output color according to the input
    FragColor = fcolor;
}