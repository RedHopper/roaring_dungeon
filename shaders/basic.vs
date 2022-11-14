#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

uniform mat4 m_model;
uniform mat4 m_projection;

out vec2 tex_coords;
out vec3 frag_pos;
out vec3 normal;

void main()
{
	tex_coords = aTexCoords;
	normal = mat3(transpose(inverse(m_model))) * aNormal;
	frag_pos = vec3(m_model * vec4(aPos, 1.0f));
	gl_Position = m_projection * m_model * vec4(aPos, 1.0f);
}
