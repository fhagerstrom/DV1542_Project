#include "Pipeline.h"

bool LoadShader(ID3D11Device* device, ID3D11VertexShader*& vertexShader, ID3D11PixelShader*& pixelShader, std::string& vertexShaderByteCode)
{
    std::string shaderData;
    std::ifstream reader;
    reader.open("Debug/VertexShader.cso", std::ios::binary | std::ios::ate); // ate = At The End

    if (!reader.is_open())
    {
        std::cerr << "Couldn't open Vertex Shader file!" << std::endl;
        return false;
    }

    reader.seekg(0, std::ios::end); // Go to end of file
    shaderData.reserve(static_cast<unsigned int>(reader.tellg())); // Reserve memory
    reader.seekg(0, std::ios::beg); // Go back to beginning

    shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

    if (FAILED(device->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &vertexShader)))
    {
        std::cerr << "Vertex Shader creation failed!" << std::endl;
        return false;
    }

    vertexShaderByteCode = shaderData;
    shaderData.clear();
    reader.close();

    reader.open("Debug/PixelShader.cso", std::ios::binary | std::ios::ate); // ate = At The End

    if (!reader.is_open())
    {
        std::cerr << "Couldn't open Pixel Shader file!" << std::endl;
        return false;
    }

    reader.seekg(0, std::ios::end); // Go to end of file
    shaderData.reserve(static_cast<unsigned int>(reader.tellg())); // Reserve memory
    reader.seekg(0, std::ios::beg); // Go back to beginning

    shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

    if (FAILED(device->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pixelShader)))
    {
        std::cerr << "Pixel Shader creation failed!" << std::endl;
        return false;
    }

    return true;
}

bool CreateSamplerState(ID3D11Device* device, ID3D11SamplerState*& sampler)
{
    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0;
    samplerDesc.MaxAnisotropy = 16;
    samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    HRESULT hr = device->CreateSamplerState(&samplerDesc, &sampler);
    return !FAILED(hr);
}

bool CreateTexture(ID3D11Device* device, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& textureSRV)
{
    return true;
}

bool SetupPipeline(ID3D11Device* device, ID3D11Buffer*& vertexBuffer, ID3D11VertexShader*& vertexShader, ID3D11PixelShader*& pixelShader, ID3D11InputLayout*& inputLayout,
    ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& textureSRV, ID3D11SamplerState*& sampler)
{
    std::string vertexShaderByteCode;

    if (!LoadShader(device, vertexShader, pixelShader, vertexShaderByteCode))
    {
        std::cerr << "Error loading shaders!" << std::endl;
        return false;
    }

    if (!CreateSamplerState(device, sampler))
    {
        std::cerr << "Error creating sampler state" << std::endl;
        return false;
    }

    return true;
}