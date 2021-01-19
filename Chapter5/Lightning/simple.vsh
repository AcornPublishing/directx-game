float4x4 mat : WORLDVIEWPROJECTION;
float4x4 worldmat : WORLD;
float ViewAngle;

float4 color = {0.7, 0.0, 0.0, 1.0};
float height = 0.44;
float glowFallOff = 0.02;
float sampleDist = 0.007;

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


Texture noiceTexture;

sampler Noise = sampler_state {texture = <noiceTexture>; mipfilter = LINEAR; minfilter = LINEAR; magfilter = LINEAR; addressu=wrap; addressv=wrap; };

float4 Pixel_Sh(float2 texCoord: TEXCOORD0) : COLOR 
{
   float2 t = float2(ViewAngle - abs(texCoord.y), ViewAngle);

   float noise = tex3D(Noise, float3(texCoord.x, t));

   float dist = abs(texCoord.y - height * (noise * 2 - 1) * texCoord.x);

   float factor = 1.0 - pow(dist*3, glowFallOff);
   factor *= factor;

   return (factor * 300) * color;
}

technique PixelLight
{
    pass P0
    {
        VertexShader = compile vs_2_0 Vexel_Sh();
        PixelShader  = compile ps_2_0 Pixel_Sh();
    }
}