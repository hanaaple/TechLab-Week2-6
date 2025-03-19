// ShaderW0.hlsl

Texture2D sourceTex : register(t0);
SamplerState samp : register(s0);

cbuffer constants : register(b0)
{
    matrix MVP;
}

cbuffer UV : register(b1)
{
    float u;
    float v;
    float width;
    float height;
}

cbuffer UseUv : register(b2)
{
    int useUv;
}

struct VS_INPUT
{
    float4 position : POSITION; // Input position from vertex buffer
    float4 color : COLOR; // Input color from vertex buffer
    float2 uv : TEXCOORD;
};

struct PS_INPUT
{
    float4 position : SV_POSITION; // Transformed position to pass to the pixel shader
    float4 color : COLOR; // Color to pass to the pixel shader
    float2 uv : TEXCOORD0;
    //uint bUseUV : BLENDINDICES;
    // float4 depthPosition : TEXCOORD0;
};

struct PS_OUTPUT
{
    float4 color : SV_TARGET;
};

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;

    output.position = mul(input.position, MVP);
    // output.depthPosition = output.position;

    output.color = input.color;
    output.uv = input.uv;
    return output;
}


PS_OUTPUT mainPS(PS_INPUT input) : SV_TARGET
{
    PS_OUTPUT output;

    // 기본 깊이 값 계산 (0.0~1.0)
    // float baseDepth = input.depthPosition.z / input.depthPosition.w;
    //output.color = (input.bUseUV==1) ? sourceTex.Sample(samp, input.uv) : input.color;

    // 텍스처 샘플링

    if (useUv == 1)
    {
        // (input.u * width, input.v * height + 1 - height);
        // 
        float correctedV = 1.0 - (v + height);
        float4 texColor = sourceTex.Sample(samp, input.uv * float2(width, height) + float2(u, correctedV));
        output.color = texColor;
        //output.color = float4(1, 1, 1, 1);
        //배경이 특정 색(예: 검은색)일 경우 픽셀 버리기
        if (length(texColor.rgb - float3(0, 0, 0)) < 0.05)
        {
            discard;
        }
    }
    else
    {
        output.color = input.color;
    }
    
    return output;
}
