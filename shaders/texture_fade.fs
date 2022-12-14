#version 330 core

out vec4 FragColor;

uniform sampler2D main_texture;
uniform vec3 color;

in vec2 tex_coords;


void main()
{
	float opacity = 0.0f;
	vec4 pixel = texture(main_texture, tex_coords);
	if (pixel.w > 0.01f) opacity = pixel.x;
	FragColor = vec4(color, opacity*pixel.w);
}
