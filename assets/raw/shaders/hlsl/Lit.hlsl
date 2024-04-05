// Vertex Shader
cbuffer MVPBuffer : register(b0) 
{
    matrix model;
    matrix view;
    matrix projection;
};

struct VS_INPUT 
{
    float3 Pos : POSITION;
    float3 Nor : NORMAL;
    float2 Tex : TEXCOORD0;
};

struct PS_INPUT 
{
    float4 Pos : SV_POSITION;
    float3 Nor : NORMAL;
    float2 Tex : TEXCOORD0;
    float3 FragPos : TEXCOORD1; // We will pass it from here.
};

PS_INPUT VS(VS_INPUT input) 
{
    PS_INPUT output;
    float4 worldPos = mul(float4(input.Pos, 1.0), model);
    output.FragPos = worldPos.xyz; // Pass world position to fragment shader
    float4 viewPos = mul(worldPos, view);
    output.Pos = mul(viewPos, projection);
    float3x3 normalMatrix = (float3x3)model;
    output.Nor = mul(input.Nor, normalMatrix);
    output.Nor = normalize(output.Nor);
    output.Tex = input.Tex;
    return output;
}

//=============SEPARATOR=============

// Fragment Shader
Texture2D ObjTexture : register(t0);
SamplerState ObjSampler : register(s0);

#define MAX_LIGHTS 20

struct Light
{
    int type;
    float3 padding;
    float3 color;
    float padding2;
    float intensity;
    float3 padding3;
    float3 position;
    float padding4;
    float3 direction;
    float padding5;
    float3 attenuation;
    float padding6;
    float range;
    float3 padding7;
};

cbuffer LightBuffer : register(b1)
{
    Light lights[MAX_LIGHTS];
    int numLights;
};


struct PS_INPUT 
{
    float4 Pos : SV_POSITION;
    float3 Nor : NORMAL;
    float2 Tex : TEXCOORD0;
    float3 FragPos : TEXCOORD1;
};

float3 calculateLightEffect(Light light, float3 norm, float3 fragPos)
{
    float3 lightEffect = float3(0.0, 0.0, 0.0);

    if (light.type == 0) // Directional Light
    {
        float3 lightDir = normalize(-light.direction);
        float diff = max(dot(norm, lightDir), 0.0);
        lightEffect = diff * light.color * light.intensity;
    }
    else if (light.type == 1) // Point Light
    {
        float3 lightDir = normalize(light.position - fragPos);
        float distance = length(light.position - fragPos);

        if (distance < light.range)
        {
            float attenuationFactor = 1.0 / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * distance * distance);
            float diff = max(dot(norm, lightDir), 0.0);
            lightEffect = diff * light.color * light.intensity * attenuationFactor;
        }
    }

    return lightEffect;
}

float4 PS(PS_INPUT input) : SV_Target 
{
    float3 norm = normalize(input.Nor);
    float3 lightEffect = float3(0.0, 0.0, 0.0);

    for (int i = 0; i < numLights; ++i)
    {
        lightEffect += calculateLightEffect(lights[i], norm, input.FragPos);
    }
	
    float3 texColor = ObjTexture.Sample(ObjSampler, input.Tex).rgb;
    float3 ambient = float3(0.1, 0.1, 0.1);
    float3 result = (ambient + lightEffect) * texColor;
    return float4(result, 1.0);
}