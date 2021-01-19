float4x4 mat : WORLDVIEWPROJECTION;
float4x4 worldmat : WORLD;
float ViewAngle;
float sideFade = 20.0f;
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
   float3 coord;

   coord.x = tex_coord.x;
   coord.y = tex_coord.y - ViewAngle;
   coord.z = 1;

   float noisy = tex2D(Noise, coord);

   float shape = WidthFactor * (0.5 - sideFade * tex_coord.x * tex_coord.x);

   float heat = saturate(shape + noisy - tex_coord.y);

   float4 flame = tex2D(Flame, heat);

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