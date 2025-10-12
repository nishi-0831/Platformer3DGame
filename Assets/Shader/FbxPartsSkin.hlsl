#include "3DCommon.hlsli"

//��`
#define MAX_BONE_MATRICES 128




cbuffer BoneMatrices : register(b1) //�{�[���̃|�[�Y�s�񂪓���
{
    matrix g_boneMatrices[MAX_BONE_MATRICES];
    bool g_hasSkinnedMesh;
    float padding[3];
};

//�X�L�j���O��̒��_�E�@��������
struct Skin
{
    float4 position;
    float3 normal;
};
//�o�[�e�b�N�X�o�b�t�@�[�̓���
struct VSSkinIn
{
    float3 position : POSITION; //�ʒu   
    float3 normal : NORMAL; //���_�@��
    float2 uv : TEXCOORD; //�e�N�X�`���[���W
    uint4 boneIndex : BONE_INDEX; //�{�[���̃C���f�b�N�X
    float4 boneWeight : BONE_WEIGHT; //�{�[���̏d��
};
//�s�N�Z���V�F�[�_�[�̓��́i�o�[�e�b�N�X�o�b�t�@�[�̏o�́j�@
struct PSSkinIn
{
    float4 position : SV_Position; //�ʒu
    float3 normal : NORMAL; //���_�@��
    float2 uv : TEXCOORD; //�e�N�X�`���[���W
    float4 Color : COLOR0; //�ŏI�J���[�i���_�V�F�[�_�[�ɂ����Ắj
};


//���_���X�L�j���O�i�{�[���ɂ��ړ��j����B�T�u�֐��i�o�[�e�b�N�X�V�F�[�_�[�Ŏg�p�j
Skin SkinVert(VSSkinIn input)
{
    Skin Output = (Skin) 0;
    float4 pos = float4(input.position, 1.0f);
    float3 normal = input.normal;
    
    if(g_hasSkinnedMesh && any(input.boneWeight.xyz > 0.0f))
    {
        float4 skinnedPos = float4(0, 0, 0, 0);
        float3 skinnedNormal = float3(0, 0, 0);
        
        [unroll]
        for (int i = 0; i < 4; i++)
        {
            if(input.boneWeight[i] > 0.0f)
            {
                // i�Ԗڂ̃{�[���̕ϊ��s����擾
                matrix boneMatrix = g_boneMatrices[input.boneIndex[i]];
                
                // ���_���W��ϊ��s��A�E�F�C�g�ŕϊ�
                Output.position += mul(boneMatrix, pos) * input.boneWeight[i];
                
                // �@�����ړ���������菜���ĕϊ�
                Output.normal += mul((float3x3) boneMatrix, normal) * input.boneWeight[i];
            }
        }
    }
    
    ////�{�[��0
    //uint iBone = input.boneIndex.x;
    //float fWeight = input.boneWeight.x;
    //matrix m = FetchBoneMatrix(iBone);
    //Output.position += fWeight * mul(position, m);
    //Output.normal += fWeight * mul(normal, (float3x3) m);
    ////�{�[��1
    //iBone = input.boneIndex.y;
    //fWeight = input.boneWeight.y;
    //m = FetchBoneMatrix(iBone);
    //Output.position += fWeight * mul(position, m);
    //Output.normal += fWeight * mul(normal, (float3x3) m);
    ////�{�[��2
    //iBone = input.boneIndex.z;
    //fWeight = input.boneWeight.z;
    //m = FetchBoneMatrix(iBone);
    //Output.position += fWeight * mul(position, m);
    //Output.normal += fWeight * mul(normal, (float3x3) m);
    ////�{�[��3
    //iBone = input.boneIndex.w;
    //fWeight = input.boneWeight.w;
    //m = FetchBoneMatrix(iBone);
    //Output.position += fWeight * mul(position, m);
    //Output.normal += fWeight * mul(normal, (float3x3) m);
    
    return Output;
}

//
//PSSkinIn VSSkin(VSSkinIn input )
//�o�[�e�b�N�X�V�F�[�_�[
PSSkinIn VS(VSSkinIn input)
{
    PSSkinIn output;
    
    Skin vSkinned = SkinVert(input);

    output.position = mul(vSkinned.position, g_matrixWVP);
    output.normal = normalize(mul(vSkinned.normal, (float3x3) g_matrixW));
    output.uv = input.uv;
    float3 LightDir = normalize(g_lightDir);
    float3 PosWorld = mul(vSkinned.position, g_matrixW);
    float3 ViewDir = normalize(g_cameraPosition - PosWorld);
    float3 Normal = normalize(output.normal);
    float4 NL = saturate(dot(Normal, LightDir));

    float3 Reflect = normalize(2 * NL * Normal - LightDir);
    float4 specular = pow(saturate(dot(Reflect, ViewDir)), g_shuniness);

    output.Color = g_diffuseColor * NL + specular * g_speculerColor;
    
    return output;
}

//
// float4 PSSkin(PSSkinIn input) : SV_Target
//�s�N�Z���V�F�[�_�[
float4 PS(PSSkinIn input) : SV_Target
{
    float4 TexDiffuse = g_texture.Sample(g_sampler, input.uv);

    return input.Color + TexDiffuse;
}