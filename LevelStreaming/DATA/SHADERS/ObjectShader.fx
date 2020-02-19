//NEW NON POINT LIGHT OBJECT SHADER
//STILL TAKES IN POINT LIGHT B/C REASONS

Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 colourTint;
	float4 ambientLight;
	float4 pointlightPosition; //W is used for intensity here
	float4 pointlightColour;
}

struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
    float3 Norm : NORMAL;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 WorldPos : POSITION;
    float2 Tex : TEXCOORD0;
    float3 Norm : NORMAL;
};

//Base Vertex Shader
PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul( input.Pos, World );
    output.WorldPos = mul( input.Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Tex = input.Tex;
    output.Norm = input.Norm.xyz;
    
    return output;
}

//Base Pixel Shader
float4 PS( PS_INPUT input) : SV_Target
{
	float4 colouredTex = (txDiffuse.Sample( samLinear, input.Tex ) * colourTint);
    return saturate((ambientLight * colouredTex) + (saturate((input.Norm.x + input.Norm.y + input.Norm.z)/3)*ambientLight));
}