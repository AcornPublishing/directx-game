float4x4 mat : WORLDVIEWPROJECTION;
float4x4 worldmat : WORLD;
float ViewAngle;

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
   coord.xy = tex_coord;
   coord.z = 1;

   float noiseX = tex3D(Noise, coord);
   float noiseY = tex3D(Noise, coord);

   coord.z = ViewAngle / 2;
   coord.x = tex_coord.x + noiseX * 0.1+coord.z;
   coord.y = tex_coord.y + noiseY * 0.1+coord.z;

   float n = tex3D(Noise, coord);

   float4 flame = tex1D(Flame, n-0.15);

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