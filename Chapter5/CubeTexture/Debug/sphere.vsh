float4x4 mat : WORLDVIEWPROJECTION;
Texture sphereTexture;
float ViewAngle;
sampler lightsphere  = sampler_state {texture = <sphereTexture>; mipfilter = LINEAR; minfilter = LINEAR; magfilter = LINEAR; addressu=clamp; addressv=clamp;};

struct VS_OUTPUT
{
    float4 Position : POSITION;
    float3 Normal : TEXCOORD0;
};

VS_OUTPUT Vexel_Sh(
    float4 Pos : POSITION, 
    float3 nor : NORMAL
    )
{
   VS_OUTPUT Out = (VS_OUTPUT)0;

   Out.Position = mul(Pos, mat);

   Out.Normal = nor;

   return Out;
}

float4 Pixel_Sh(
    float3 normal : TEXCOORD0
  ) : COLOR0
{
 return texCUBE(lightsphere, normal);
}

technique PixelLight
{
    pass P0
    {
        VertexShader = compile vs_1_1 Vexel_Sh();
        PixelShader  = compile ps_2_0 Pixel_Sh();
    }
}