// Vertex Shader
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;   // Add normal data

out vec2 TexCoord;
out vec3 Normal;         // Pass normal data
out vec3 FragPos;        // Pass fragment position

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0)); // Calculate world position of vertex
	Normal = mat3(transpose(inverse(model))) * aNormal; // Calculate normal

	gl_Position = projection * view * model * vec4(aPos, 1.0);
	TexCoord = aTexCoord;
}


//=============SEPARATOR=============


// Fragment Shader
#version 330 core
in vec2 TexCoord;
in vec3 Normal;          // Added normal vector
in vec3 FragPos;         // Fragment position

out vec4 FragColor;

uniform sampler2D textureSampler;
uniform vec3 lightDir;   // Direction of the light
uniform vec3 lightColor; // Color of the light

void main()
{
	// Ambient lighting
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	// Diffuse lighting
	vec3 norm = normalize(Normal);
	float diff = max(dot(norm, -lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// Combine results
	vec3 result = (ambient + diffuse) * texture(textureSampler, TexCoord).rgb;
	FragColor = vec4(result, 1.0);
}