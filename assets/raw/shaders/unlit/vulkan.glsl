// Vertex Shader
#version 450

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 0) out vec2 TexCoord;

layout(set = 0, binding = 0) uniform Matrices
{
	mat4 model;
	mat4 view;
	mat4 projection;
};

void main()
{
	vec4 clip = projection * view * model * vec4(aPos, 1.0);
	clip.y = -clip.y;
	clip.z = (clip.z + clip.w) * 0.5;
	gl_Position = clip;
	TexCoord = aTexCoord;
}

//=============SEPARATOR=============

// Fragment Shader
#version 450

layout(location = 0) in vec2 TexCoord;
layout(location = 0) out vec4 FragColor;

layout(set = 0, binding = 1) uniform sampler2D textureSampler;

void main()
{
	FragColor = texture(textureSampler, TexCoord);
}
