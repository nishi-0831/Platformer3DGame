Texture2D g_texture : register(t0);  // �e�N�X�`��
SamplerState g_sampler : register(s0);  // �e�N�X�`���̃T���v��

/*
 * �R���X�^���g�o�b�t�@(constant buffer)
 * DirectX ���瑗���Ă�����(�|���S�����_������)
*/
cbuffer global
{
    matrix g_matrixCameraTranslate; // �J�����ϊ��s��
    matrix g_matrixCameraRotation;  // �J������]�s��
    matrix g_matrixWorldTranslate;  // ���[���h�ϊ��s��
    matrix g_matrixWorldRotation;   // ���[���h��]�s��
    matrix g_matrixTexture;         // �e�N�X�`�����W�ϊ��s��
    float4 g_color;                 // �e�N�X�`���̍����F
    float2 g_angle;                 // �e�N�X�`���̊p�x
};

/*
 * ���_(Vertex)�V�F�[�_�̏o��
*/
struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float2 g_position : TEXCOORD1;
};
