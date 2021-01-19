float4x4 world_view_proj_matrix;
float3 lightPos0;
float ExtrudeDistance = 1000;
sampler2D tText;

struct VS_INPUT 
{
   float3 Pos:      POSITION;
   float3 Normal:   NORMAL;
   float2 TexCoord: TEXCOORD0;
};

struct VS_OUTPUT 
{
   float4 Pos:        POSITION;
   float2 TexCoord:   TEXCOORD0;
   float  Brightness: TEXCOORD1;
};

VS_OUTPUT Vexel_Sh( VS_INPUT In )
{
   VS_OUTPUT Out;

   Out.Pos = mul(float4(In.Pos,1.0), world_view_proj_matrix);
   Out.TexCoord = In.TexCoord;
   
   float Brightness = dot(-normalize(lightPos0),In.Normal);
   Brightness = max(Brightness,0.5); // Don't make it too dark
   
   Out.Brightness = Brightness;

   return Out;
}

struct PS_INPUT 
{
   float4 Pos:        POSITION;
   float2 TexCoord:   TEXCOORD0;
   float  Brightness: TEXCOORD1;
};

float4 Pixel_Sh( PS_INPUT In ) : COLOR0
{
   //  Output constant color:
   float4 color = tex2D(tText,In.TexCoord) *
                    In.Brightness;
   return color;
}


technique PixelLight
{
    pass P0
    {
        ZFunc = Always;
        StencilEnable = True;
        StencilFunc = Always;
        StencilPass = Replace;
        StencilRef = 0x49;

        VertexShader = compile vs_2_0 Vexel_Sh();
        PixelShader  = compile ps_2_0 Pixel_Sh();
    }
}