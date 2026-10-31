// Vertex Shader
#version 450

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

layout(location = 0) out vec2 TexCoord;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec3 FragPos;

layout(set = 0, binding = 0) uniform Matrices
{
	mat4 model;
	mat4 view;
	mat4 projection;
};

void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = mat3(model) * aNormal;

	vec4 clip = projection * view * model * vec4(aPos, 1.0);
	clip.y = -clip.y;
	clip.z = (clip.z + clip.w) * 0.5;
	gl_Position = clip;

	TexCoord = aTexCoord;
}

//=============SEPARATOR=============

// Fragment Shader
#version 450

#define MAX_LIGHTS 20

struct Light
{
	int type;
	vec3 color;
	float range;
	float intensity;
	vec3 position;
	vec3 direction;
	vec3 attenuation;
};

layout(location = 0) in vec2 TexCoord;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec3 FragPos;
layout(location = 0) out vec4 FragColor;

layout(set = 0, binding = 1) uniform sampler2D textureSampler;

layout(set = 0, binding = 2) uniform LightBuffer
{
	int numLights;
	Light lights[MAX_LIGHTS];
};

vec3 calculateLightEffect(Light light, vec3 normal, vec3 fragPos)
{
	vec3 lightEffect = vec3(0.0);

	if (light.type == 0)
	{
		vec3 lightDir = normalize(light.direction);
		float diff = max(dot(normal, lightDir), 0.0);
		lightEffect += diff * light.color * light.intensity;
	}
	else if (light.type == 1)
	{
		vec3 lightVector = light.position - fragPos;
		vec3 lightDir = normalize(lightVector);
		float distance = length(lightVector);

		if (distance < light.range)
		{
			float attenuation = 1.0 /
			(light.attenuation.x +
			light.attenuation.y * distance +
			light.attenuation.z * distance * distance);

			float diff = max(dot(normal, lightDir), 0.0);
			lightEffect += diff * light.color * light.intensity * attenuation;
		}
	}

	return lightEffect;
}

void main()
{
	vec3 norm = normalize(Normal);
	vec3 ambient = vec3(0.1);
	vec3 diffuse = vec3(0.0);

	for (int i = 0; i < MAX_LIGHTS; ++i)
	{
		if (i >= numLights)
		{
			break;
		}

		diffuse += calculateLightEffect(lights[i], norm, FragPos);
	}

	vec3 result = (ambient + diffuse) * texture(textureSampler, TexCoord).rgb;
	FragColor = vec4(result, 1.0);
}
