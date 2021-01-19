float4x4 mat;
float4x4 worldmat;
float ViewAngle;
float Min = -3.0f;
float Max = 3.0f;
float4 lightDir = {0.40, 0.40, -0.8, 1};

struct VS_OUTPUT 
{
   float4 Pos:     POSITION;
   float3 normal:  TEXCOORD0;
   float3 viewvec: TEXCOORD1;
   float4 basecol: TEXCOORD2;
};

VS_OUTPUT Vexel_Sh(float4 Pos: POSITION, 
  float3 normal: NORMAL)
{
   VS_OUTPUT Out;

   float3 spherePos = normalize(Pos.xyz);

   float t = frac(ViewAngle);
   t = smoothstep(0, 0.5, t) - smoothstep(0.5, 1, t);

   float lrp = Min + (Max - Min) * t;

   Pos.xyz = lerp(spherePos, Pos, lrp);
   normal = lerp(spherePos, normal, lrp);

   Out.basecol = 0.5 + 0.5 * Pos;
   Pos.xyz *= 30;

   Out.Pos = mul(Pos, mat);
   Out.normal = mul(normal, worldmat);
   Out.viewvec = mul(Pos, worldmat);

   return Out;
}

float4 Pixel_Sh(float3 normal: TEXCOORD0, float3 viewvec: TEXCOORD1, float4 basecol: TEXCOORD2) : COLOR 
{
    float3 normalized = normalize(normal);
    float3 lightn = normalize(lightDir);
    float4 diffuse = saturate(dot(lightDir, normalized));
    
    float3 reflection = normalize(2 * diffuse * normalized - lightn);
    float3 eyen = normalize(viewvec);
    float4 specular = pow(saturate(dot(reflection, eyen)), 16);
        
    return basecol * diffuse + specular;
}

technique PixelLight
{
    pass P0
    {
        VertexShader = compile vs_2_0 Vexel_Sh();
        PixelShader  = compile ps_2_0 Pixel_Sh();
    }
}