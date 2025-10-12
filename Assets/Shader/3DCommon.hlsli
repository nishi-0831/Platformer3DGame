Texture2D g_texture : register(t0);  // �e�N�X�`��
SamplerState g_sampler : register(s0);  // �e�N�X�`���̃T���v��

cbuffer global : register(b0)
{
    matrix g_matrixWVP;  // ���[���h�E�r���[�E�v���W�F�N�V�����̍����s��
    matrix g_matrixNormalTrans;  // �@���̕ϊ��s�� (��]�s��)
    matrix g_matrixW;    // ���[���h�ϊ��s��
    float4 g_lightDir;  // ������
    float4 g_diffuseColor;  // �}�e���A���̐F
    float4 g_ambientColor;  // �e�̐F
    float4 g_speculerColor;  // �X�y�L�����[����̐F
    float4 g_cameraPosition;  // ���_ (�J�����̍��W)
    float g_shuniness;  // �n�C���C�g�̋���
    bool g_hasTexture;  // �e�N�X�`���������Ă��邩
};

struct VS_OUT
{
    float4 position : SV_POSITION;  // �ʒu
    float4 normal : NORMAL0;  // �@��
    float2 uv : TEXCOORD; // uv���W
    float4 eye : NORMAL1;
};
