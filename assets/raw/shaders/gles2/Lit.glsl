// Vertex Shader
#version 100

attribute vec3 aPos;
attribute vec2 aTexCoord;
attribute vec3 aNormal;

varying vec2 TexCoord;
varying vec3 Normal;
varying vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));

	Normal = mat3(model) * aNormal;

	gl_Position =
	projection *
	view *
	model *
	vec4(aPos, 1.0);

	TexCoord = aTexCoord;
}


//=============SEPARATOR=============


// Fragment Shader
#version 100

precision mediump float;

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
uniform sampler2D tex0;

varying vec2 TexCoord;
varying vec3 Normal;
varying vec3 FragPos;

vec3 calculateLightEffect(
	Light light,
	vec3 normal,
	vec3 fragPos
)
{
	vec3 lightEffect = vec3(0.0);

	if (light.type == 0)
	{
		vec3 lightDir = normalize(light.direction);

		float diff =
		max(dot(normal, lightDir), 0.0);

		lightEffect +=
		diff *
		light.color *
		light.intensity;
	}
	else if (light.type == 1)
	{
		vec3 lightVector =
		light.position - fragPos;

		vec3 lightDir =
		normalize(lightVector);

		float distance =
		length(lightVector);

		if (distance < light.range)
		{
			float attenuation =
			1.0 /
			(
				light.attenuation.x +
				light.attenuation.y * distance +
				light.attenuation.z * distance * distance
			);

			float diff =
			max(dot(normal, lightDir), 0.0);

			lightEffect +=
			diff *
			light.color *
			light.intensity *
			attenuation;
		}
	}

	return lightEffect;
}

void main()
{
	vec3 norm = normalize(Normal);

	vec3 ambient =
	vec3(0.1);

	vec3 diffuse =
	vec3(0.0);

	for (int i = 0; i < MAX_LIGHTS; ++i)
	{
		if (i >= numLights)
		{
			break;
		}

		diffuse +=
		calculateLightEffect(
			lights[i],
			norm,
			FragPos
		);
	}

	vec3 result =
	(ambient + diffuse) *
	texture2D(tex0, TexCoord).rgb;

	gl_FragColor =
	vec4(result, 1.0);
}
