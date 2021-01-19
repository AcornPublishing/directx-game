struct VS_OUTPUT
{
    float4 pos  : POSITION;
    float4 col : COLOR0;
};


float3 Light = {0.0f, 0.0f, -1.0f};

float4x4 mat : WORLDVIEWPROJECTION;
float TimeFactor = 1.0f;

VS_OUTPUT ShaderFunc(
 float4 Pos : POSITION, 
 float3 Normal : NORMAL)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    Out.pos = mul(Pos, mat);
    
    Out.col.r = TimeFactor/10;
    Out.col.g = 1-Out.col.rb;
    Out.col.b = 1-Out.col.rb;

    float4 normalmap = mul(Normal, mat);
    Out.col *= dot(normalmap, Light);

    return Out;
}

technique Transform
{
    pass P0
    {
        VertexShader = compile vs_1_1 ShaderFunc();
    }
}

