#pragma once

struct screenQuad
{
    float positions[3];
    float uv[2];
    float normals[3];
    // std::array is a container which encapsulates fixed size arrays
    screenQuad(const std::array<float, 3>& position, const std::array<float, 2>& uvCoords, const std::array<float, 3>& normal)
    {
        for (int i = 0; i < 3; ++i)
        {
            positions[i] = position[i];

        }
        uv[0] = uvCoords[0];
        uv[1] = uvCoords[1];
        for (int j = 0; j < 3; ++j)
        {
            normals[j] = normal[j];
        }
    }
};

bool SetupPipeline(ID3D11Device* device, ID3D11SamplerState*& sampler, ID3D11PixelShader*& pShaderDeferred, ID3D11VertexShader*& vShaderDeferred,
    ID3D11PixelShader*& lightPShaderDeferred, ID3D11VertexShader*& lightVShaderDeferred,
    ID3D11InputLayout*& renderTargetMesh, ID3D11Buffer*& screenQuadMesh, ID3D11RasterizerState*& rasterizerStateWireFrame,
    ID3D11RasterizerState*& rasterizerStateSolid, ID3D11SamplerState*& clampSampler);

