
/*
 * �w��̈ʒu�ɒP�F�̐}�`��`�悷�邽�߂̃V�F�[�_
*/

/*
 * �R���X�^���g�o�b�t�@(constant buffer)
 * DirectX ���瑗���Ă�����(�|���S�����_������)
*/
cbuffer global
{
    float4 g_color;  // �F
    matrix g_matrixWorld;  // ���_���W�ϊ��s��
};

/*
 * ���_(Vertex)�V�F�[�_�̏o��
*/
struct VSOutput
{
    float4 position : SV_POSITION;
};

VSOutput VS(float4 position : POSITION)
{
    VSOutput output;
    output.position = mul(position, g_matrixWorld);

    // TODO: �J�������W�̌v�Z������
    return output;
}

float4 PS(VSOutput input) : SV_Target
{
    return g_color;
}
