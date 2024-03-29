// Vertex Shader
cbuffer ConstantBuffer : register(b0) 
{
    matrix worldViewProj;
};

struct VS_INPUT 
{
    float3 Pos : POSITION;
    float4 Col : COLOR;
    float2 Tex : TEXCOORD0;
};

struct PS_INPUT 
{
    float4 Pos : SV_POSITION;
    float4 Col : COLOR;
    float2 Tex : TEXCOORD0;
};

PS_INPUT VS(VS_INPUT input) 
{
    PS_INPUT output;
    output.Pos = mul(float4(input.Pos, 1.0), worldViewProj);
    output.Col = input.Col;
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