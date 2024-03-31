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
};

PS_INPUT VS(VS_INPUT input) 
{
    PS_INPUT output;
    float4 worldPos = mul(float4(input.Pos, 1.0), model);
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

cbuffer LightBuffer : register(b1)
{
    float3 LightDirection;
    float3 LightColor;
};

struct PS_INPUT 
{
    float4 Pos : SV_POSITION;
    float3 Nor : NORMAL;
    float2 Tex : TEXCOORD0;
};

float4 PS(PS_INPUT input) : SV_Target 
{
    float3 norm = normalize(input.Nor);
    float3 lightEffect = float3(0.0, 0.0, 0.0);
    float3 lightDir = normalize(-LightDirection);
    float diff = max(dot(norm, lightDir), 0.0);
    lightEffect = diff * LightColor;
    float3 texColor = ObjTexture.Sample(ObjSampler, input.Tex).rgb;
    float3 result = lightEffect * texColor;
    return float4(result, 1.0);
}