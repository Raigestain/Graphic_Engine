#include "ColorShader.h"



ColorShader::ColorShader()
{
	m_matrixBuffer = nullptr;
	m_pixelShader = nullptr;
	m_vertexShader = nullptr;
	m_inputLayout = nullptr;
}

ColorShader::ColorShader(const ColorShader& object)
{

}


ColorShader::~ColorShader()
{
}

bool ColorShader::Initialize(ID3D11Device * device, HWND hwnd)
{
	bool bResult;

	bResult = InitializeShader(device, hwnd, L"../Graphic_Engine_v2/ColorVS.hlsl", L"../Graphic_Engine_v2/ColorPS.hlsl");
	if (!bResult)
	{
		return false;
	}
	return true;
}

void ColorShader::Shutdown()
{
	//Shutdown the pixel and vertex shaders.
	ShutdownShader();
}

bool ColorShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix,
						 XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	bool bResult;

	//Set the shader parameters that will be used for rendering.
	bResult = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!bResult)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}

bool ColorShader::InitializeShader(ID3D11Device *device, HWND hwnd, WCHAR *vsFilename, WCHAR *psFilename)
{
	HRESULT hResult;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	
	//Initialize the pointers this function will use to null.
	errorMessage = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	//Compile the vertex shader code.
	hResult = D3DCompileFromFile(vsFilename,
								 NULL,
								 NULL,
								 "ColorVertexShader",
								 "vs_5_0",
								 D3D10_SHADER_ENABLE_STRICTNESS,
								 0,
								 &vertexShaderBuffer,
								 &errorMessage);
	
	//We catch the error in case that the compilation failed. If the errorMessage has no message, then it couldn't find the file.
	if (FAILED(hResult))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Vertex Shader File", MB_OK);
		}

		return false;
	}

	//Compile the pixel shader.
	hResult = D3DCompileFromFile(psFilename,
								 NULL,
								 NULL,
								 "ColorPixelShader",
								 "ps_5_0",
								 D3D10_SHADER_ENABLE_STRICTNESS,
								 0,
								 &pixelShaderBuffer,
								 &errorMessage);
	if (FAILED(hResult))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Pixel Shader File", MB_OK);
		}
		return false;
	}

	//After compiling the vertex and pixel shader code, it's time to create the shader objects using the buffers.
	
	//Create the vertex shader using the buffer.
	hResult = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
										 NULL, &m_vertexShader);
	if (FAILED(hResult))
	{
		return false;
	}

	//Create the pixel shader using the buffer.
	hResult = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(),
									    NULL, &m_pixelShader);
	if (FAILED(hResult))
	{
		return false;
	}

	//Create the input layout description.
	//This setup needs to match the VertexType structure in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	//Get count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//Create the vertex input layout.
	hResult = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
										vertexShaderBuffer->GetBufferSize(), &m_inputLayout);
	if (FAILED(hResult))
	{
		return false;
	}

	//Release the vertex and pixel shader buffers since they're not longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;
	
	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hResult = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(hResult))
	{
		return false;
	}

	return true;
}

void ColorShader::ShutdownShader()
{
	// Release the matrix constant buffer.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = nullptr;
	}

	// Release the layout.
	if (m_inputLayout)
	{
		m_inputLayout->Release();
		m_inputLayout = nullptr;
	}

	// Release the pixel shader.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = nullptr;
	}

	// Release the vertex shader.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = nullptr;
	}
}

void ColorShader::OutputShaderErrorMessage(ID3D10Blob * errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (unsigned long long i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);
}

bool ColorShader::SetShaderParameters(ID3D11DeviceContext * deviceContext, XMMATRIX worldMatrix, 
									  XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	HRESULT hResult;
	D3D11_MAPPED_SUBRESOURCE mappedSubresourse;
	MatrixBufferType* constantBuffer;
	unsigned int bufferNumber;

	//Transpose matrices to prepare them to the shader. This is required by DirectX.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	//Lock the m_matrixBuffer, set the new matrices inside it, and then unlock it.

	//Lock the constant buffer so it can be written to.
	hResult = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresourse);
	if (FAILED(hResult))
	{
		return false;
	}

	//Get the pointer to the data in the constant buffer.
	constantBuffer = (MatrixBufferType*)mappedSubresourse.pData;

	//Copy the matrices into the constant buffer.
	constantBuffer->world = worldMatrix;
	constantBuffer->view = viewMatrix;
	constantBuffer->projection = projectionMatrix;

	//Unlock the constant buffer.
	deviceContext->Unmap(m_matrixBuffer, 0);

	//Now set the updated matrix buffer in the HLSL vertex shader.

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finally set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	return true;
}

void ColorShader::RenderShader(ID3D11DeviceContext * deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_inputLayout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);
}

