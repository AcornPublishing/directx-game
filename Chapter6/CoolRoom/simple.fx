float4x4 mat : WORLDVIEWPROJECTION;
float4x4 worldmat : WORLD;
float4 light = {0.0f, 0.0f, -1.0f}; 
float4 facecolor;

struct VS_OUTPUT
{
    float4 Position : POSITION;
    float3 Normal : TEXCOORD1;
    float3 LightDir : TEXCOORD0;
};

VS_OUTPUT Vexel_Sh(
    float4 pos : POSITION,
    float3 nor : NORMAL
    )
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    Out.Position = mul(pos, mat);

    Out.LightDir = light;

    Out.Normal = normalize(mul(nor, worldmat));

    return Out;
}

float4 Pixel_Sh(
    float3 normal : TEXCOORD1,
    float3 lightdir : TEXCOORD0
  ) : COLOR0
{
    float3 lightn = normalize(lightdir);
    float4 diffuse = saturate(dot(lightn, normal));
        
    return facecolor * diffuse;
}

technique PixelLight
{
    pass P0
    {
        VertexShader = compile vs_1_1 Vexel_Sh();
        PixelShader  = compile ps_2_0 Pixel_Sh();
    }
}
