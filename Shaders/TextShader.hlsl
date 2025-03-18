cbuffer VS_CONSTANT : register(b0)
{
    matrix MVP;
}
cbuffer PS_CONSTANT : register(b1)
{
    float4 CustomColor;
    uint bUseVertexColor;
}
struct VS_INPUT
{
    float4 position : POSITION; // Input position from vertex buffer
    float4 color : COLOR;       // Input color from vertex buffer
    float2 uv : TEXCOORD;
};

struct PS_INPUT
{
    float4 position : SV_POSITION; // Transformed position to pass to the pixel shader
    float4 color : COLOR;          // Color to pass to the pixel shader
    float2 uv : TEXCOORD;
};

struct PS_OUTPUT
{
    float4 color : SV_TARGET;
    float depth : SV_Depth;
};

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;

    output.position = mul(input.position, MVP);
    // output.depthPosition = output.position;

    output.color = bUseVertexColor == 1 ? input.color : CustomColor;
    //output.color = input.color;
    //output.uv = float2(input.u, input.v);
    return output;
}


PS_OUTPUT mainPS(PS_INPUT input) : SV_TARGET
{
    PS_OUTPUT output;

    // 기본 깊이 값 계산 (0.0~1.0)
    // float baseDepth = input.depthPosition.z / input.depthPosition.w;
    //output.color = (input.bUseUV==1) ? sourceTex.Sample(samp, input.uv) : input.color;
    
    output.color = input.color;
    
    // output.color = float4(depth, depth, depth, 1.0f);
    
    return output;
}