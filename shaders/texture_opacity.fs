#version 330 core

out vec4 FragColor;

uniform sampler2D main_texture;

in vec2 tex_coords;

void main()
{
	//FragColor = vec4(texture(main_texture, tex_coords).xyz, 0.5f); 
	FragColor = texture(main_texture, tex_coords); 
}
