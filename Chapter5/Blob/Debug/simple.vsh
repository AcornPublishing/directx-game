struct VS_OUTPUT
{
    float4 pos  : POSITION;
    float4 col : COLOR0;
};


float3 Light = {0.0f, 0.0f, -3.0f};

float4x4 mat : WORLDVIEWPROJECTION;
float TimeFactor = 1.0f;

VS_OUTPUT ShaderFunc(
 float4 Pos : POSITION, 
 float3 Normal : NORMAL)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    float4 normalmap = mul(Normal, mat);

    Out.col.r = sin(TimeFactor/10);
    Out.col.g = 1-Out.col.r;
    Out.col.b = 1-Out.col.r;

    Out.col *= dot(normalmap, Light);


    Pos.y += sin(Pos+TimeFactor);
    Out.pos = mul(Pos, mat);

    return Out;
}

VS_OUTPUT ShaderFuncColored(
 float4 Pos : POSITION, 
 float3 Normal : NORMAL)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    float4 normalmap = mul(Normal, mat);
    Out.col = sin(Pos);
    Out.col *= dot(normalmap, Light);

    Pos.y += sin(Pos+TimeFactor);
    Out.pos = mul(Pos, mat);

    return Out;
}

technique Transform
{
    pass P0
    {
        VertexShader = compile vs_1_1 ShaderFunc();
    }
}

technique TransformColored
{
    pass P0
    {
        VertexShader = compile vs_1_1 ShaderFuncColored();
    }
}

