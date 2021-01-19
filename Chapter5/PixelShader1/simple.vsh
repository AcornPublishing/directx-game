float4x4 mat : WORLDVIEWPROJECTION;
float4x4 worldmat : WORLD;
float4 light;
float4 facecolor;
const float specfactor = 16;

struct VS_OUTPUT
{
    float4 Position : POSITION;
    float3 Normal : TEXCOORD1;
    float3 LightDir : TEXCOORD0;
    float3 eyepos : TEXCOORD2;
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

    float3 worldpos = normalize(mul(pos, worldmat));
    Out.eyepos = light - worldpos;

    return Out;
}

float4 Pixel_Sh(
    float3 normal : TEXCOORD1,
    float3 lightdir : TEXCOORD0,
    float3 eye : TEXCOORD2
  ) : COLOR0
{
    float3 normalized = normalize(normal);
    float3 lightn = normalize(lightdir);
    float4 diffuse = saturate(dot(light, normalized));
    
    float3 reflection = normalize(2 * diffuse * normalized - lightn);
    float3 eyen = normalize(eye);
    float4 specular = pow(saturate(dot(reflection, eyen)), specfactor);
        
    return facecolor * diffuse + specular;
}

technique PixelLight
{
    pass P0
    {
        VertexShader = compile vs_1_1 Vexel_Sh();
        PixelShader  = compile ps_2_0 Pixel_Sh();
    }
}