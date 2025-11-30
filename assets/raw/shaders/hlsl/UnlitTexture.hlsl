// Vertex Shader
cbuffer VPBuffer : register(b0) 
{
    matrix view;
    matrix projection;
};

cbuffer MBuffer : register(b1) 
{
    matrix model;
};

struct VS_INPUT 
{
    float3 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct PS_INPUT 
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

PS_INPUT VS(VS_INPUT input) 
{
    PS_INPUT output;
    float4 worldPos = mul(float4(input.Pos, 1.0), model);
    float4 viewPos = mul(worldPos, view);
    output.Pos = mul(viewPos, projection);
    output.Tex = input.Tex;
    return output;
}


//=============SEPARATOR=============


// Fragment Shader
Texture2D ObjTexture : register(t0);
SamplerState ObjSampler : register(s0);

struct PS_INPUT 
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

float4 PS(PS_INPUT input) : SV_Target 
{
    return ObjTexture.Sample(ObjSampler, input.Tex);
}