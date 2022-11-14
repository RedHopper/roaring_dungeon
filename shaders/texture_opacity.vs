#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 m_model;
uniform mat4 m_projection;

out vec2 tex_coords;

void main()
{
	gl_Position = m_projection * m_model * vec4(aPos, 1.0f);
	tex_coords = aTexCoords;
}
