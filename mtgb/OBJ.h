#pragma once
#include "ISystem.h"
#include <DirectXMath.h>

#include <string>
#include <vector>

#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

struct ID3D11Buffer;
struct ID3D11InputLayout;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11Buffer;

namespace mtgb
{
    class Transform;
    struct SimpleVertex
    {
        DirectX::XMVECTOR pos;
    };

    struct SimpleConstantBuffer
    {
        DirectX::XMMATRIX mWVP;
    };

    struct SimpleMesh
    {
        int numVert;
        int numFace;
        ComPtr<ID3D11Buffer> pVertexBuffer;
        ComPtr<ID3D11Buffer> pIndexBuffer;
        SimpleMesh() :numVert{ 0 }, numFace{ 0 }, pVertexBuffer{ nullptr }, pIndexBuffer{nullptr} {}
    };

    struct ModelData
    {
        //�t�@�C����
        std::string fileName;

        SimpleMesh* mesh;

    };

    class OBJ : public ISystem
    {
    public:
        
        void Initialize() override;
        void Update() override;
        void Release() override;

        static int Load(const std::string& fileName);
        void Draw(int hModel, const Transform * transform);
    private:
        std::vector<ModelData*> datas_;
        void InitMesh(const std::string& fileName, SimpleMesh* mesh);

        //�����f���̎�ނ���(���f���̍\�����S�ē���Ȃ�A�v���ɂЂƂj
        static ComPtr<ID3D11InputLayout> pInputLayout_;
        static ComPtr<ID3D11VertexShader> pVertexShader_;
        static ComPtr<ID3D11PixelShader> pPixelShader_;
        static ComPtr<ID3D11Buffer> pConstantBuffer_;
        //�����f������	
        //ID3D11Buffer* pVertexBuffer_;
        //SimpleMesh mesh_;
    };
}

