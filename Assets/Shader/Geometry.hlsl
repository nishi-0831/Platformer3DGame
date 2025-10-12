//�O���[�o��
cbuffer global
{
	matrix g_mWVP; //���[���h����ˉe�܂ł̕ϊ��s��
};

//�o�[�e�b�N�X�o�b�t�@�[�o�͍\����
struct VS_OUTPUT
{	
    float4 Pos : SV_POSITION;
};
//
//�o�[�e�b�N�X�V�F�[�_�[
//
VS_OUTPUT VS( float4 Pos : POSITION )
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.Pos = mul( Pos, g_mWVP );

    return output;
}

//
//�s�N�Z���V�F�[�_�[
//
float4 PS( VS_OUTPUT input ) : SV_Target
{
	return float4( 1.0f, 0.0f, 0.0f, 1.0f );
}

