//
// Simple HLSL 
//

float4x4 mat : WORLDVIEWPROJECTION;
float TimeFactor = 1.0f;

struct VS_OUTPUT
{
    float4 pos  : POSITION;
    float4 col : COLOR0;
};


VS_OUTPUT ShaderFunc(float4 Pos : POSITION)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    // Set position
    Out.pos = mul(Pos, mat);

    // Set color
    Out.col.r = TimeFactor;
    Out.col.g = Out.col.r/10;
    Out.col.b = 1-Out.col.g;

    return Out;
}


technique Transform
{
    pass P0
    {
        VertexShader = compile vs_1_1 ShaderFunc();
    }
}
