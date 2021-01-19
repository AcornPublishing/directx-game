float4x4 world_view_proj_matrix;
float4x4 view_proj_matrix;
float4x4 view_matrix;
float3 lightPos0;
float3 lightPos1;
float ExtrudeDistance = 1000;
float mTime;

struct VS_INPUT
{
   float3 Pos:      POSITION0;
   float3 Normal:   NORMAL0;
   float2 TexCoord: TEXCOORD0;

   float3 Pos1:      POSITION1;
   float3 Normal1:   NORMAL1;
   float2 TexCoord1: TEXCOORD1;
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

   Out.Pos = float4(In.Pos1, 1)*mTime+float4(In.Pos, 1)*(1-mTime);
   Out.Pos = mul(Out.Pos, world_view_proj_matrix);

   Out.TexCoord = In.TexCoord;
   Out.Normal = In.Normal1;

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

   dotP = max(dotP, 0.5); 
   
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