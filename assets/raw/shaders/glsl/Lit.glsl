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

void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal;

	gl_Position = projection * view * model * vec4(aPos, 1.0);
	TexCoord = aTexCoord;
}


//=============SEPARATOR=============


// Fragment Shader
#version 330 core

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

uniform Light lights[MAX_LIGHTS];
uniform int numLights;
uniform sampler2D textureSampler;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

vec3 calculateLightEffect(const Light light, const vec3 normal, const vec3 fragPos) 
{
    vec3 lightEffect = vec3(0.0);

    if (light.type == 0) // Directional
	{
        vec3 lightDir = normalize(light.direction);
        float diff = max(dot(normal, lightDir), 0.0);
        lightEffect += diff * light.color * light.intensity;
    }
	else if (light.type == 1) // Point
	{
        vec3 lightDir = normalize(light.position - fragPos);
        float distance = length(light.position - fragPos);

        if (distance < light.range) 
		{
            float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * pow(distance, 2));
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

    for (int i = 0; i < numLights; ++i) 
	{
        diffuse += calculateLightEffect(lights[i], norm, FragPos);
    }

    vec3 result = (ambient + diffuse) * texture(textureSampler, TexCoord).rgb;
    FragColor = vec4(result, 1.0);
}