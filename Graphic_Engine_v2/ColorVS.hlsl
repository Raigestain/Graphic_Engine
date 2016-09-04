/********************************/
/*   GLOBALS                    */
/********************************/
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

/********************************/
/*   TYPEDEFS                   */
/********************************/
struct VertexInputType
{
    float4 position : POSITION;
    float4 color    : COLOR;
};

struct PixelInputType
{
    float4 position : SV_Position;
    float4 color    : COLOR;
};

/*
*   ColorVertexShader()
*   brief: This shader will put the vertex in a position relative to the world, view and projection matrices.
*   param VertexInputType input: Is the input with the position of each vertex and its color.
*   output PixelInputType: The information with the calculated position of the vertex and the color.
*/
PixelInputType ColorVertexShader( VertexInputType input )
{
    PixelInputType output;

    //Change the position vector to be 4 unitos for proper matrix calculations.
    input.position.w = 1.0f;

    //Calculate the vertex position agains world, view and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    //Store the input color for the pixel shader to use.
    output.color = input.color;
	
    return output;
}