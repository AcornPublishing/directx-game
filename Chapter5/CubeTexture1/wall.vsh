float4x4 mat : WORLDVIEWPROJECTION;
float view_position;
float3x3 mat1;
float4 light_pos = {0, 0, -20, 1};
float ViewAngle;

Texture sphereTexture;
Texture wallTexture;
sampler lightsphere = sampler_state {texture = <sphereTexture>; mipfilter = LINEAR; minfilter = LINEAR; magfilter = LINEAR; addressu=clamp; addressv=clamp;};
sampler wall = sampler_state {texture = <wallTexture>; mipfilter = LINEAR; minfilter = LINEAR; magfilter = LINEAR; addressu=wrap; addressv=wrap;};

struct VS_OUTPUT
{
    float4 Position : POSITION;
    float3 tex_coord: TEXCOORD0;
    float3 light_vec: TEXCOORD1;
    float3 view_vec:  TEXCOORD2;
    float3 light_pos:      TEXCOORD3;
};

VS_OUTPUT Vexel_Sh(
    float4 Pos : POSITION, 
    float3 normal : NORMAL
    )
{
   VS_OUTPUT Out = (VS_OUTPUT)0;

   Out.Position = mul(Pos, mat);

   Out.tex_coord.x = lerp(Pos.x, Pos.y, abs(normal.x));
   Out.tex_coord.y = lerp(Pos.z, Pos.y, abs(normal.z));

   Pos.xyz *= -100;

   float3 lightvec = light_pos - Pos;
   Out.light_vec.x = dot(lightvec, normal);
   Out.light_vec.y = dot(lightvec, normal);
   Out.light_vec.z = dot(lightvec, normal);

   float3 viewvec = view_position - Pos;
   Out.view_vec.x = dot(viewvec, normal);
   Out.view_vec.y = dot(viewvec, normal);
   Out.view_vec.z = dot(viewvec, normal);

   Out.light_pos = mul(mat1, lightvec);

   return Out;
}

float4 Pixel_Sh(
   float2 tex_coord : TEXCOORD0, 
   float3 light_vec : TEXCOORD1, 
   float3 view_vec : TEXCOORD2, 
   float3 light_pos : TEXCOORD3
  ) : COLOR0
{
   float4 wallpix = tex2D(wall, tex_coord);

   float4 light = texCUBE(lightsphere, light_pos);

   light_vec = normalize(light_vec);
   view_vec = normalize(view_vec);

   float specular = pow(saturate(dot(-view_vec, light_vec)), 12);

   return (wallpix + specular) * light + 0.3 * wallpix;
}

technique PixelLight
{
    pass P0
    {
        VertexShader = compile vs_1_1 Vexel_Sh();
        PixelShader  = compile ps_2_0 Pixel_Sh();
    }
}