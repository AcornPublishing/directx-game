float4x4 world_view_proj_matrix:WORLDVIEWPROJECTION;
float4x4 worldmat : WORLD;
float4 lightPos0;

struct VS_INPUT 
{
   float3 Pos:      POSITION;
   float3 Normal:   NORMAL;
   float2 TexCoord: TEXCOORD;
};

struct VS_OUTPUT {
   float4 Pos:      POSITION;
   float2 TexCoord: TEXCOORD0;
};

VS_OUTPUT Vexel_Sh(VS_INPUT In){
   VS_OUTPUT Out;

   Out.Pos = mul(float4(In.Pos,1.0), world_view_proj_matrix);
   Out.TexCoord = In.TexCoord;
   return Out;
}



sampler2D tText;

struct PS_INPUT 
{
   float4 Pos:      POSITION;
   float2 TexCoord: TEXCOORD;
};

float4 Pixel_Sh(PS_INPUT In): COLOR0{

   float4 color = tex2D(tText,In.TexCoord);
   return color*0.2;
}

technique PixelLight
{
    pass P0
    {
        AlphaBlendEnable = True;
        SrcBlend         = DestAlpha;
        DestBlend        = One;

        ZWriteEnable     = true;
        StencilEnable    = True;
        StencilFunc      = Greater;
        StencilRef       = 0x40;

       VertexShader = compile vs_2_0 Vexel_Sh();
       PixelShader  = compile ps_2_0 Pixel_Sh();
    }
}
