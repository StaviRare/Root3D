// Vertex Shader
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time; // Uniform variable for time

void main()
{
	vec3 pos = aPos;
	pos.x += sin(time + pos.y) * 0.1; // Wobble effect along the X-axis
	pos.y += sin(time + pos.x) * 0.1; // Wobble effect along the Y-axis

	FragPos = vec3(model * vec4(pos, 1.0)); 
	Normal = mat3(transpose(inverse(model))) * aNormal; 

	gl_Position = projection * view * model * vec4(pos, 1.0);
	TexCoord = aTexCoord;
}


// Separator
//--SHADER-END--


// Fragment Shader
#version 330 core
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D textureSampler;

void main()
{
	FragColor = texture(textureSampler, TexCoord);
}
