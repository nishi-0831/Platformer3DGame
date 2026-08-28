#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <vector>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

ID3D11InputLayout* CreateInputLayout(ID3D11Device* _pDevice, ID3DBlob* _pShaderBlob);
