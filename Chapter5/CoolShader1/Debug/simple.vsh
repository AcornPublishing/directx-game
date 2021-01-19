float4x4 view_proj_matrix;
float4 color;
float ViewAngle;

const float baseamp=0.10;
const float speed=3.0;
const float rate=15.0;

struct VS_OUTPUT {
  float4 Pos: POSITION;
  float2 xyvec: TEXCOORD;
};

// Vertex Shader
VS_OUTPUT vertsh(float4 Pos: POSITION)
{
   VS_OUTPUT Out;

   Pos.xy = sign(Pos.xy);

   Out.Pos = float4(Pos.xy, 0, 1);
   Out.xyvec = Pos.xy;

   return Out;
}


// Pixel Shader
float4 pixsh(float2 pos: TEXCOORD0) : COLOR 
{
   float x = speed * ViewAngle + rate * pos.x;

   float base = (1 + cos(x*2.5)) * (1 + sin(x*3.5));

   float z = frac(0.05 * x);
   z = max(z, 1 - z);
   z = pow(z, 20);
   float pulse = exp(-10000 * z);

   return pow(saturate(1 - abs(pos.y - (baseamp * base + pulse - 0.5))), 16) * color;
}


technique PixelLight
{
    pass P0
    {
        VertexShader = compile vs_2_0 vertsh();
        PixelShader  = compile ps_2_0 pixsh();
    }
}