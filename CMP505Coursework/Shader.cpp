//
// Shader.cpp
// Copyright © 2019 Diel Barnes. All rights reserved.
//

#include "Shader.h"

#pragma region Init

Shader::Shader(ID3D11Device &device, ID3D11DeviceContext &immediateContext)
{
	m_pDevice = &device;
	m_pImmediateContext = &immediateContext;
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pVertexInputLayout = nullptr;
	m_pMatrixBuffer = nullptr;
}

Shader::~Shader()
{
	SAFE_RELEASE(m_pVertexShader)
	SAFE_RELEASE(m_pPixelShader)
	SAFE_RELEASE(m_pVertexInputLayout)
	SAFE_RELEASE(m_pMatrixBuffer)
}

HRESULT Shader::Initialize(LPCWSTR vertexShaderFilename, LPCSTR vertexShaderEntryPoint, LPCWSTR pixelShaderFilename, LPCSTR pixelShaderEntryPoint, D3D11_INPUT_ELEMENT_DESC vertexInputDesc[], UINT uiElementCount)
{
	HRESULT result = S_OK;

	// Compile the vertex shader
	ID3DBlob* pCompiledVertexShader;
	result = CompileShaderFromFile(vertexShaderFilename, vertexShaderEntryPoint, "vs_5_0", &pCompiledVertexShader);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to compile vertex shader.", result);
		return result;
	}

	const void* ppCompiledVertexShader = pCompiledVertexShader->GetBufferPointer();
	SIZE_T compiledVertexShaderSize = pCompiledVertexShader->GetBufferSize();

	// Create the vertex shader
	result = m_pDevice->CreateVertexShader(ppCompiledVertexShader,
										   compiledVertexShaderSize,
										   nullptr,						// Class linkage interface for dynamic shader linking
										   &m_pVertexShader);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create vertex shader.", result);
		return result;
	}

	// Compile the pixel shader
	ID3DBlob* pCompiledPixelShader;
	result = CompileShaderFromFile(pixelShaderFilename, pixelShaderEntryPoint, "ps_5_0", &pCompiledPixelShader);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to compile pixel shader.", result);
		return result;
	}

	const void* ppCompiledPixelShader = pCompiledPixelShader->GetBufferPointer();
	SIZE_T compiledPixelShaderSize = pCompiledPixelShader->GetBufferSize();

	// Create the pixel shader
	result = m_pDevice->CreatePixelShader(ppCompiledPixelShader,
										  compiledPixelShaderSize,
										  nullptr,						// Class linkage interface for dynamic shader linking
										  &m_pPixelShader);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create pixel shader.", result);
		return result;
	}

	// Create the vertex input layout (should be the same as Vertex struct)
	result = m_pDevice->CreateInputLayout(vertexInputDesc, uiElementCount, ppCompiledVertexShader, compiledVertexShaderSize, &m_pVertexInputLayout);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create vertex input layout.", result);
		return result;
	}

	// Create the matrix constant buffer
	// ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(MatrixBuffer);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;				// Resource is accessible by both the GPU (read only) and the CPU (write only); good choice for a resource that will be updated by the CPU at least once per frame
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// Bind the buffer as a constant buffer to the input assembler stage
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	result = m_pDevice->CreateBuffer(&bufferDesc, nullptr, &m_pMatrixBuffer);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to create matrix buffer.", result);
		return result;
	}

	// Release
	SAFE_RELEASE(pCompiledPixelShader)
	SAFE_RELEASE(pCompiledVertexShader)

	return result;
}

HRESULT Shader::CompileShaderFromFile(LPCWSTR filename, LPCSTR entryPoint, LPCSTR target, ID3DBlob** ppCompiledShader)
{
	HRESULT result = S_OK;

	ID3DBlob* pError = nullptr;
	result = D3DCompileFromFile(filename,
								nullptr,										 // Array of shader macros
								nullptr,										 // Include interface the compiler will use if the shader contains #include
								entryPoint,										 // Name of the shader entry point function where shader execution begins
								target,											 // Target set of shader features / effect type
								D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG, // Disallow deprecated syntax; insert debug information into the output code
								0,												 // Flags for compiling an effect instead of a shader
								ppCompiledShader,
								&pError);
	if (FAILED(result))
	{
		if (pError)
		{
			// Send a string to the debugger for display
			OutputDebugStringA(reinterpret_cast<const char*>(pError->GetBufferPointer()));
			pError->Release();
		}
		// else shader file is missing
	}

	return result;
}

#pragma endregion

#pragma region Render

HRESULT Shader::SetMatrixBuffer(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	HRESULT result = S_OK;

	// Update the matrix constant buffer

	// Lock the matrix buffer so it can be written to
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	result = m_pImmediateContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		Utils::ShowError("Failed to map the matrix buffer.", result);
		return result;
	}

	// Get a pointer to the matrix buffer data
	MatrixBuffer* matrixBufferData = (MatrixBuffer*)mappedResource.pData;

	// Copy the matrices into the matrix buffer
	matrixBufferData->worldMatrix = XMMatrixTranspose(worldMatrix);
	matrixBufferData->viewMatrix = XMMatrixTranspose(viewMatrix);
	matrixBufferData->projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// Unlock the matrix buffer
	m_pImmediateContext->Unmap(m_pMatrixBuffer, 0);

	// Set the constant buffers to be used by the vertex shader
	m_pImmediateContext->VSSetConstantBuffers(0,					// Position of the constant buffer in the vertex shader
											  1,					// Number of buffers to set
											  &m_pMatrixBuffer);	// Array of constant buffers being given to the device

	return result;
}

#pragma endregion
