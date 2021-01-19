float4x4 world_view_proj_matrix;
float4x4 view_proj_matrix;
float4x4 view_matrix;
float3 lightPos0;
float ExtrudeDistance = 1000;

struct VS_INPUT1
{
   float3 Pos:      POSITION;
   float3 Normal:   NORMAL;
   float2 TexCoord: TEXCOORD;
};

struct VS_OUTPUT1
{
   float4 Pos:      POSITION;
   float2 TexCoord: TEXCOORD0;
   float3 LightDir: TEXCOORD1;
   float3 Normal:   TEXCOORD2;
};

VS_OUTPUT1 Vexel_ShObj(VS_INPUT1 In)
{
   VS_OUTPUT1 Out;

   Out.Pos = mul(float4(In.Pos,1.0), world_view_proj_matrix);
   Out.TexCoord = In.TexCoord;
   
   Out.LightDir = lightPos0;
   Out.Normal = In.Normal;

   return Out;
}

sampler2D tText;

struct PS_OUTPUT 
{
    float4 Color : COLOR;
};

struct PS_INPUT1
{
   float2 TexCoord: TEXCOORD0;
   float3 LightDir: TEXCOORD1;
   float3 Normal:   TEXCOORD2;
};

PS_OUTPUT Pixel_ShObj( PS_INPUT1 In )
{
   PS_OUTPUT Out;
   
   float4 color  = tex2D(tText,In.TexCoord);
        
   float3 normal = normalize(In.Normal);
   float3 lightDir = In.LightDir;
   
   float dotP = saturate(dot(-lightDir, normal));
   dotP = max(dotP, 0.4); 
   
   Out.Color = color * dotP;
   
   return Out;
}


struct VS_INPUT 
{
   float3 Pos:        POSITION;
   float3 Normal:     NORMAL;
};

struct VS_OUTPUT 
{
   float4 Pos:     POSITION;
   float4 Color:   COLOR;
};

VS_OUTPUT Vexel_Sh0( VS_INPUT In )
{
   VS_OUTPUT Out;

   float3 lightDirInView = lightPos0;
   float3 normalInView   = In.Normal;
   float3 posInView      = float4(In.Pos,1.0);


   if (dot(normalInView, -lightDirInView)<0.0)
   {
      posInView += lightDirInView * ExtrudeDistance;
   }

   Out.Pos = mul(float4(posInView, 1.0), world_view_proj_matrix);
   Out.Color = float4(0, 0, 0, 0);

   return Out;
}


technique PixelLight
{
    pass P1
    {
        ColorWriteEnable = 0;
        ZFunc            = Less;
        ZWriteEnable     = false;
        StencilEnable    = True;
        StencilFunc      = Less;
        StencilMask      = 0xffffffff;
        StencilWriteMask = 0xffffffff;

        CullMode = CCW;
	StencilZFail      = IncrSat;

      VertexShader = compile vs_2_0 Vexel_Sh0();
    }

    pass P2
    {
        CullMode = CW;
	StencilZFail      = DecrSat;

        VertexShader = compile vs_2_0 Vexel_Sh0();
    }
}
