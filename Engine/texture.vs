cbuffer MatrixBuffer {
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};
struct VertexInputType {
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType {
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

PixelInputType TextureVertexShader(VertexInputType input){
    input.position.w = 1.0f;    // Change the position vector to be 4 units for proper matrix calculations.

    // Calculate the position of the vertex against the world, view, and projection matrices.
    PixelInputType output;
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.tex = input.tex;     // Store the texture coordinates for the pixel shader.

    return output;
}