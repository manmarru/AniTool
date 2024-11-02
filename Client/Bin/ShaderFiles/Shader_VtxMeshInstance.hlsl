
#include "Shader_Engine_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
texture2D g_NoiseTexture;

vector g_vCamPosition;
float3 g_Color;

struct VS_IN
{
	/* InputSlot : 0 */
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    
	/* InputSlot : 1 */
    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
    float4 vLook : TEXCOORD3;
    float4 vTranslation : TEXCOORD4;
    float2 vLifeTime : TEXCOORD5;
};

struct VS_OUT
{
	/* SV_ : ShaderValue */
	/* ���� �ؾ��� ������ ��� ���ĳ��� ���̹Ƿ� ���� dx�� �߰������� �ؾ��� �̸��� �ض�. */
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

/* 1. ������ ��ȯ������ �����Ѵ�. */
/* 2. ������ ���� ������ �����Ѵ�. */
VS_OUT VS_MAIN( /*����*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    matrix TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	/* ���û��¿����� ������ ����. */
    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

	/* ���忡 Ư�� ���·� ��ġ�Ѵ�. */
    vPosition = mul(vPosition, g_WorldMatrix);

    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);

    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;
    Out.vLifeTime = In.vLifeTime;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);


    if (In.vLifeTime.y >= In.vLifeTime.x)
        discard;

    return Out;
}

PS_OUT PS_MAIN_NONISE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    vector vNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor *= (vNoise * 1.5);

    if (In.vLifeTime.y >= In.vLifeTime.x)
        discard;

    return Out;
}



technique11 DefaultTechnique
{
    pass Basic
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass Noise
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NONISE();
    }

}