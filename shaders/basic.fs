#version 330 core
out vec4 FragColor;

uniform sampler2D green_texture;
uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec3 camera_pos;

in vec2 tex_coords;
in vec3 frag_pos;
in vec3 normal;

void main()
{
	vec3 n_normal = normalize(normal);
	vec3 light_dir = normalize(light_pos - frag_pos);
	vec3 obj_color = vec3(texture(green_texture, tex_coords));
	vec3 l_ambient = light_color * 0.2f;
	float lf_diffuse = max(dot(light_dir, n_normal), 0.1f);
	vec3 l_diffuse = light_color * lf_diffuse;
	
	vec3 view_dir = normalize(camera_pos - frag_pos);
	vec3 reflect_dir = reflect(-light_dir, n_normal);
	float lf_specular = pow(max(dot(reflect_dir, view_dir), 0.0f), 32);
	if (lf_diffuse - 0.1f < 0.001f) lf_specular = 0.0f;
	vec3 l_specular = lf_specular * light_color;

	vec3 l_combined = (l_ambient + l_diffuse + l_specular) * obj_color; 
	FragColor = vec4(l_combined, 1.0f);
}


