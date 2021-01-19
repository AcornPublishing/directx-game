float4x4 mat : WORLDVIEWPROJECTION;
float4x4 worldmat : WORLD;
float ViewAngle;
float sideFade = 50.0f;
float WidthFactor = 0.05f;
float xWind = 0.05f;

struct VS_OUTPUT 
{
   float4 Pos: POSITION;
   float2 tex_coord: TEXCOORD0;
};

VS_OUTPUT Vexel_Sh(float4 Pos: POSITION)
{
   VS_OUTPUT Out;

   Out.Pos = mul(Pos, mat);

   Out.tex_coord = Pos.xy;

   return Out;
}


Texture meshTexture;
Texture noiceTexture;

sampler Flame = sampler_state {texture = <meshTexture>; mipfilter = LINEAR; minfilter = LINEAR; magfilter = LINEAR; addressu=clamp; addressv=clamp;};
sampler Noise = sampler_state {texture = <noiceTexture>; mipfilter = LINEAR; minfilter = LINEAR; magfilter = LINEAR; addressu=wrap; addressv=wrap;};

float4 Pixel_Sh(float2 tex_coord: TEXCOORD0) : COLOR 
{
   float z = frac(ViewAngle);
   float i = pow(z, 0.5);

   float size = (i * (i-2) + 1) * i * 7;
   float dist = length(tex_coord) / size;

   float n = tex3D(Noise, float3(0.5 * tex_coord, 0.5 * dist));

   float4 flame = tex1D(Flame, size + size * (n - dist));
 
   return flame;
}

technique PixelLight
{
    pass P0
    {
        VertexShader = compile vs_2_0 Vexel_Sh();
        PixelShader  = compile ps_2_0 Pixel_Sh();
    }
}