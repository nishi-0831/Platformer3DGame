#include "OBJ.h"
#include "MTAssert.h"
#include "DirectX11Draw.h"

#include "ReleaseUtility.h"
#include "Matrix4x4.h"
#include "Transform.h"
#include "Vector2Int.h"
#include "Screen.h"
#include "ISystem.h"
#include "SceneSystem.h"
#include "CameraSystem.h"
#include <stdio.h>
#include "IncludingWindows.h"
#include <d3d11.h>
#include <d3dCompiler.h>

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dCompiler.lib")
#pragma comment(lib,"winmm.lib")

ComPtr<ID3D11InputLayout> mtgb::OBJ::pInputLayout_{ nullptr };
ComPtr<ID3D11VertexShader> mtgb::OBJ::pVertexShader_{ nullptr };
ComPtr<ID3D11PixelShader> mtgb::OBJ::pPixelShader_{ nullptr };
ComPtr<ID3D11Buffer> mtgb::OBJ::pConstantBuffer_{ nullptr };


void mtgb::OBJ::Initialize()
{
	HRESULT hResult;

	//hlsl�t�@�C���ǂݍ��� �u���u�쐬�@�u���u�Ƃ̓V�F�[�_�[�̉�݂����Ȃ��́BXX�V�F�[�_�[�Ƃ��ē����������Ȃ��B��Ŋe��V�F�[�_�[�ɐ��蓾��B
	ID3DBlob* pCompiledShader = NULL;
	hResult = D3DCompileFromFile(L"Shader/Geometry.hlsl", NULL, NULL, "VS", "vs_5_0", 0, 0, &pCompiledShader, NULL);

	massert(SUCCEEDED(hResult)
		&& "���_�V�F�[�_�̃R���p�C���Ɏ��s @OBJ::Initialize");

	hResult = DirectX11Draw::pDevice_->CreateVertexShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), NULL, pVertexShader_.ReleaseAndGetAddressOf());
	massert(SUCCEEDED(hResult)
		&& "���_�V�F�[�_�̍쐬�Ɏ��s @OBJ::Initialize");

	//���_�C���v�b�g���C�A�E�g���`
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	int numElements = sizeof(layout) / sizeof(layout[0]);

	//���_�C���v�b�g���C�A�E�g���쐬
	hResult = DirectX11Draw::pDevice_->CreateInputLayout(layout, numElements, pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), pInputLayout_.ReleaseAndGetAddressOf());
	massert(SUCCEEDED(hResult)
		&& "���_�C���v�b�g���C�A�E�g�̍쐬�Ɏ��s @OBJ::Initialize");

	//�s�N�Z���V�F�[�_�쐬
	hResult = D3DCompileFromFile(L"Shader/Geometry.hlsl", NULL, NULL, "PS", "ps_5_0", 0, 0, &pCompiledShader, NULL);
	massert(SUCCEEDED(hResult)
		&& "�s�N�Z���V�F�[�_�̍쐬�Ɏ��s @OBJ::Initialize");

	hResult = DirectX11Draw::pDevice_->CreatePixelShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), NULL, pPixelShader_.ReleaseAndGetAddressOf());
	massert(SUCCEEDED(hResult)
		&& "�s�N�Z���V�F�[�_�̍쐬�Ɏ��s @OBJ::Initialize");

	SAFE_RELEASE(pCompiledShader);

	//�R���X�^���g�o�b�t�@�[�쐬
	D3D11_BUFFER_DESC cb;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(SimpleConstantBuffer);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	hResult = DirectX11Draw::pDevice_->CreateBuffer(&cb, NULL, pConstantBuffer_.ReleaseAndGetAddressOf());
		massert(SUCCEEDED(hResult)
			&& "�R���X�^���g�o�b�t�@�̍쐬�Ɏ��s @OBJ::Initialize");
}

int mtgb::OBJ::Load(const std::string& fileName)
{
	ModelData* pData = new ModelData;

	OBJ& pInstance{ Game::System<OBJ>() };

	//�J�����t�@�C���ꗗ���瓯���t�@�C�����̂��̂��������T��
	bool isExist = false;
	for (int i = 0; i < pInstance.datas_.size(); i++)
	{
		//���ɊJ���Ă���ꍇ
		if (pInstance.datas_[i] != nullptr && pInstance.datas_[i]->fileName == fileName)
		{
			pData->mesh = pInstance.datas_[i]->mesh;
			isExist = true;
			break;
		}
	}

	//�V���Ƀt�@�C�����J��
	if (isExist == false)
	{
		pData->mesh = new SimpleMesh;
		pInstance.InitMesh(fileName, pData->mesh);
	}

	//�g���ĂȂ��ԍ����������T��
	for (int i = 0; i < pInstance.datas_.size(); i++)
	{
		if (pInstance.datas_[i] == nullptr)
		{
			pInstance.datas_[i] = pData;
			return i;
		}
	}

	//�V���ɒǉ�
	pInstance.datas_.push_back(pData);
	return (int)pInstance.datas_.size() - 1;
}

void mtgb::OBJ::Update()
{
}

void mtgb::OBJ::Release()
{

	SAFE_CLEAR_CONTAINER_DELETE(datas_);

	pInputLayout_.Reset();
	pVertexShader_.Reset();
	pPixelShader_.Reset();
	pConstantBuffer_.Reset();

}

void mtgb::OBJ::Draw(int hModel, const Transform* transform)
{
	DirectX11Draw::SetIsWriteToDepthBuffer(true);
	//DirectX::XMMATRIX mWorld;
	Matrix4x4 mWorld;
	transform->GenerateWorldMatrix(&mWorld);

	Matrix4x4 mView;
	// �r���[�g�����X�t�H�[���i���_���W�ϊ��j
	
	const Transform& cameraTransform = Game::System<CameraSystem>().GetTransform();
	Game::System<CameraSystem>().GetViewMatrix(&mView);

	Matrix4x4 mProj;
	Game::System<CameraSystem>().GetProjMatrix(&mProj);
	static const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };

	ID3D11DeviceContext* tmpContext = DirectX11Draw::pContext_.Get();
	tmpContext->VSSetShader(pVertexShader_.Get(), NULL, 0);
	tmpContext->PSSetShader(pPixelShader_.Get(), NULL, 0);

	//�V�F�[�_�[�̃R���X�^���g�o�b�t�@�[�Ɋe��f�[�^��n��	
	D3D11_MAPPED_SUBRESOURCE pData;
	SimpleConstantBuffer cb;

	HRESULT hResult = DirectX11Draw::pContext_->Map(pConstantBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &pData);
	massert(SUCCEEDED(hResult)
		&& "Map�Ɏ��s @OBJ::Draw");
	if (SUCCEEDED(hResult))
	{
		DirectX::XMMATRIX mWVP = (mWorld) * mView * mProj;
		//cb.mWVP = DirectX::XMMatrixTranspose(*mWorld);
		cb.mWVP = DirectX::XMMatrixTranspose(mWVP);

		memcpy_s(pData.pData, pData.RowPitch, (void*)&cb, sizeof(cb));

		DirectX11Draw::pContext_->Unmap(pConstantBuffer_.Get(), 0);

		int slot = 0;
		tmpContext->VSSetConstantBuffers(slot, 1, pConstantBuffer_.GetAddressOf());
		tmpContext->PSSetConstantBuffers(slot, 1, pConstantBuffer_.GetAddressOf());

		tmpContext->IASetInputLayout(pInputLayout_.Get());

		tmpContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//���_�o�b�t�@�Z�b�g
		UINT stride = sizeof(SimpleVertex);
		UINT offset = 0;
		tmpContext->IASetVertexBuffers(slot, 1, datas_[hModel]->mesh->pVertexBuffer.GetAddressOf(), &stride, &offset);

		//�C���f�b�N�X�o�b�t�@�Z�b�g
		tmpContext->IASetIndexBuffer(datas_[hModel]->mesh->pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		//�v���~�e�B�u�������_�����O
		tmpContext->DrawIndexed(datas_[hModel]->mesh->numFace * 3, 0, 0);

		
		//DirectX11Draw::End();
	}
}



void mtgb::OBJ::InitMesh(const std::string& fileName, SimpleMesh* mesh)
{
	float x, y, z;
	int v1 = 0, v2 = 0, v3 = 0;
	char key[190] = { 0 };
	
	//�t�@�C�����J���ē��e��ǂݍ���
	FILE* fp = NULL;
	fopen_s(&fp, fileName.c_str(), "rt");
	
	// �t�@�C���I�[�v���̊m�F
	/*if (fp == NULL) {
		massert(false && "OBJ�t�@�C���̃I�[�v���Ɏ��s @OBJ::InitMesh");
		return;
	}*/

	// ������
	mesh->numVert = 0;
	mesh->numFace = 0;

	//�܂��͒��_���A�|���S�����𒲂ׂ�
	while (!feof(fp))
	{
		//�L�[���[�h�ǂݍ���
		fscanf_s(fp, "%s ", key, (unsigned int)sizeof(key));
		//���_
		if (strcmp(key, "v") == 0)
		{
			mesh->numVert++;
		}
		//�t�F�C�X�i�|���S���j
		if (strcmp(key, "f") == 0)
		{
			mesh->numFace++;
		}
	}

	//�ꎞ�I�ȃ������m�ہi���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@�j
	SimpleVertex* pVertexBuffer = new SimpleVertex[mesh->numVert];
	int* pIndexBuffer = new int[mesh->numFace * 3];

	//�{�ǂݍ���	
	fseek(fp, SEEK_SET, 0);
	int vertCount = 0;//�ǂݍ��݃J�E���^�[
	int faceCount = 0;//�ǂݍ��݃J�E���^�[

	while (!feof(fp))
	{
		//�L�[���[�h�ǂݍ���
		ZeroMemory(key, sizeof(key));
		fscanf_s(fp, "%s ", key, (unsigned int)sizeof(key));
		//���_ �ǂݍ���
		if (strcmp(key, "v") == 0)
		{
			fscanf_s(fp, "%f %f %f", &x, &y, &z);
			//OBJ�͉E����W�n�Ȃ̂�x���邢�͂��𔽓]
			pVertexBuffer[vertCount].pos = DirectX::XMVectorSet(-x, y, z, 1);
			vertCount++;
		}
		//�t�F�C�X�i�|���S���j �ǂݍ���
		if (strcmp(key, "f") == 0)
		{
			fscanf_s(fp, "%d %d %d", &v1, &v2, &v3);
			pIndexBuffer[faceCount * 3] = v1 - 1;
			pIndexBuffer[faceCount * 3 + 1] = v2 - 1;
			pIndexBuffer[faceCount * 3 + 2] = v3 - 1;
			faceCount++;
		}
	}

	fclose(fp);

	//���_�o�b�t�@�쐬
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * mesh->numVert;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = pVertexBuffer;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	HRESULT hResult = DirectX11Draw::pDevice_->CreateBuffer(&bd, &initData, mesh->pVertexBuffer.ReleaseAndGetAddressOf());
	massert(SUCCEEDED(hResult)
		&& "���_�o�b�t�@�̍쐬�Ɏ��s���܂��� @OBJ::InitStaticMesh");

	//�C���f�b�N�X�o�b�t�@�쐬
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(int) * mesh->numFace * 3;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = pIndexBuffer;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;
	hResult = DirectX11Draw::pDevice_->CreateBuffer(&bd, &initData, mesh->pIndexBuffer.ReleaseAndGetAddressOf());
	massert(SUCCEEDED(hResult)
	&& "�C���f�b�N�X�o�b�t�@�̍쐬�Ɏ��s���܂��� @OBJ::InitStaticMesh");

	delete[] pVertexBuffer;
	delete[] pIndexBuffer;
}
