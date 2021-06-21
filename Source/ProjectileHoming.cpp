#include "ProjectileHoming.h"


// HACK::�z�[�~���O�e�͓G�����Ȃ��Ȃ�ƓG�̈ʒu�������Ă���Ȃ��Ȃ邽�߃o�O��B
// ���C���ς�(�G�̐����擾���āA�G�̐����O�ɂȂ��straite��ł悤�ɕύX)
// �R���X�g���N�^
ProjectileHoming::ProjectileHoming(ProjectileManager* manager) : Projectile(manager)
{
    model = new Model("Data/Model/Sword/Sword.mdl");
    // ���f�����������̂ŃX�P�[�����O
    scale.x = scale.y = scale.z = 3.0f;
}

//�f�X�g���N�^
ProjectileHoming::~ProjectileHoming()
{
    delete model;
}

// �X�V����
void ProjectileHoming::Update(float elapsedTime)
{
    // ��������
    lifeTimer -= elapsedTime;
    if (lifeTimer <= 0)
    {
        Destroy();
    }

    // �ړ�
    {
        float moveSpeed = this->moveSpeed * elapsedTime;
        position.x += direction.x * moveSpeed;
        position.y += direction.y * moveSpeed;
        position.z += direction.z * moveSpeed;
    }
    // ����
    {
        float turnSpeed = this->turnSpeed * elapsedTime;
        // �^�[�Q�b�g�܂ł̃x�N�g�����Z�o
        DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&position);
        DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&target);
        DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Target, Position);

        // �[���x�N�g���łȂ��Ȃ��]����
        DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
        float lengthSq;
        DirectX::XMStoreFloat(&lengthSq, LengthSq);
        if (lengthSq > 0.00001f)
        {
            // �^�[�Q�b�g�܂ł̃x�N�g����P�ʃx�N�g����
            Vec = DirectX::XMVector3Normalize(Vec);

            // �����Ă�������x�N�g�����Z�o
            DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&direction);

            // �O�����x�N�g���ƃ^�[�Q�b�g�܂ł̃x�N�g���̓���(�p�x)���Z�o
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Direction, Vec);
            float dot;
            DirectX::XMStoreFloat(&dot, Dot);

            // 2�̒P�ʃx�N�g���̊p�x���������ق�
            // 1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����B 0~2�܂ł̒l�ɂȂ�
            float rot = 1.0 - dot; //�@�␳�l
            if (rot > turnSpeed)
            {
                rot = turnSpeed;
            }

            // ��]�p�x������Ȃ��]����������
            if (fabsf(rot) > 0.0001f)
            {
                // ��]�����Z�o
                DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Direction, Vec);

                // ��]���Ɖ�]�ʂ����]�s����Z�o
                // XMMatrixRotationAxis : �C�ӂ̎��𒆐S�ɉ�]����s����\�z
                DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationAxis(Axis, rot);

                // ���݂̍s�����]������
                DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);
                //XMMatrixMultiply :  2�̍s��̐ς��v�Z
                Transform = DirectX::XMMatrixMultiply(Transform, Rotation);

                // ��]��̑O�����������o���A�P�ʃx�N�g��������
                Direction = DirectX::XMVector3Normalize(Transform.r[2]);
                DirectX::XMStoreFloat3(&direction, Direction);
            }
        }
    }
    // �I�u�W�F�N�g�s����X�V
    UpdateTransform();

    // ���f���s��X�V
    model->UpdateTransform(transform);
}

// �`�揈��
void ProjectileHoming::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);
}

// ����
void ProjectileHoming::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& target)
{
    this->direction = direction;
    this->position = position;
    this->target = target;

    UpdateTransform();
}
