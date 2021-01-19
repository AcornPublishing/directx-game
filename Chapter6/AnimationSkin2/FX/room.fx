float4x4 world_view_proj_matrix;
float4x4 view_proj_matrix;
float4x4 view_matrix;
float3 lightPos0;
float3 lightPos1;
float ExtrudeDistance = 1000;

struct VS_INPUT
{
   float3 Pos:      POSITION;
   float3 Normal:   NORMAL;
   float2 TexCoord: TEXCOORD;
};

struct VS_OUTPUT
{
   float4 Pos:      POSITION;
   float2 TexCoord: TEXCOORD0;
   float3 Normal:   TEXCOORD2;
};

VS_OUTPUT Vexel_ShObj(VS_INPUT In)
{
   VS_OUTPUT Out;

   Out.Pos = mul(float4(In.Pos,1.0), world_view_proj_matrix);
   Out.TexCoord = In.TexCoord;
   
   Out.Normal = In.Normal;

   return Out;
}

sampler2D tText;

struct PS_OUTPUT 
{
    float4 Color : COLOR;
};

struct PS_INPUT
{
   float2 TexCoord: TEXCOORD0;
   float3 Normal:   TEXCOORD2;
};

PS_OUTPUT Pixel_ShObj( PS_INPUT In )
{
   PS_OUTPUT Out;
   
   float4 color  = tex2D(tText,In.TexCoord);
        
   float3 normal = normalize(In.Normal);

   float dotP = saturate(dot(-lightPos0, normal));
   float dotP1 = saturate(dot(-lightPos1, normal));
   dotP=max(dotP, dotP1);

   dotP = max(dotP, 0.2); 
   
   Out.Color = color * dotP;
   
   return Out;
}

technique PixelLight
{
    pass P0
    {
        VertexShader = compile vs_2_0 Vexel_ShObj();
        PixelShader  = compile ps_2_0 Pixel_ShObj();
    }
}
