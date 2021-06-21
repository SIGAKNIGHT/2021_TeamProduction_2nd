#include "Character.h"
#include "CameraController.h"

// �s��X�V����   
void Character::UpdateTransform()
{
    // �X�P�[���s����쐬
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    // ��]�s����쐬
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    // �ʒu�s����쐬
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    // 3�̍s���g�ݍ��킹�A���[���h�s����쐬
    DirectX::XMMATRIX W = S * R * T;
    //�v�Z�������[���h�s������o��
    DirectX::XMStoreFloat4x4(&transform, W);

}

// �_���[�W��^����
bool Character::ApplyDamage(int damage, float invincibleTime)
{
    // �_���[�W��0�̏ꍇ�͌��N��Ԃ�ύX����K�v���Ȃ�
    if (damage == 0) return false;

    // ���S���Ă���ꍇ�͌��N��Ԃ�ύX���Ȃ�
    if (health <= 0) return false;
    
    // �_���[�W����
    if (invincibleTimer > 0.0f) return false;
    invincibleTimer = invincibleTime;

    health -= damage;


    // ���S�ʒm
    if (health <= 0)
    {
        OnDead();
    }
    // �_���[�W�ʒm
    else
    {
        OnDamage();
    }
    // ���N��Ԃ��ύX�����ꍇ��true��Ԃ�
    return true;
}

// ���G���ԍX�V
void Character::UpdateInvincibleTimer(float elapsedTime)
{
    if (invincibleTimer >= 0.0f)
    {
        invincibleTimer -= elapsedTime;
    }
}

void Character::Move(float vx, float vz, float speed)
{
    // �ړ������x�N�g����ݒ�
    moveVecX = vx;
    moveVecZ = vz;

    // �ő呬�x�ݒ�
    maxMoveSpeed = speed;
}

// ���񏈗�
void Character::Turn(float elapsedTime, float vx, float vz, float speed)
{
    speed *= elapsedTime;

    // �i�s�x�N�g�����[���x�N�g���̏ꍇ�͏�������K�v�Ȃ�
    float length = sqrt(pow(vx, 2) + pow(vz, 2));
    if (length < 0.001f)
    {
        return;
    }

    // �i�s�x�N�g����P�ʃx�N�g����
    vx = vx / sqrt(pow(vx, 2) + pow(vz, 2));
    vz = vz / sqrt(pow(vx, 2) + pow(vz, 2));

    // ���g�̉�]�l����O�����x�N�g�������߂�
    float frontX = sinf(angle.y);
    float frontZ = cosf(angle.y);

    // ��]�p�����߂邽�߁A2�̒P�ʃx�N�g���̓��ς��v�Z����
    float dot = (frontX * vx) + (frontZ * vz); // ����

    // ���ϒl��-1.0~1.0�ŕ\������Ă���A2�̒P�ʃx�N�g���̊p�x��
    // �������ق�1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����
    float rot = 1.0 - dot; //�@�␳�l
    if (rot > speed)
    {
        rot = speed;
    }

    // ���E������s�����߂�2�̒P�ʃx�N�g���̊O�ς��v�Z����
    float cross = (frontX * vz) - (frontZ * vx);

    // 2D�̊O�ϒl�����̏ꍇ���{�̏ꍇ�ɂ���č��E���肪�s����
    // ���E������s�����Ƃɂ���č��E��]��I������
    if (cross <= 0.0f) // �O�ς����Ȃ̂ŉE�ɐ���
    {
        angle.y += rot;
    }
    else // ���ɐ���
    {
        angle.y -= rot;
    }
}


// �W�����v����
void Character::Jump(float speed)
{
    // ������̗͂�ݒ�
    velocity.y = speed;
}

// �Ռ���^����
void Character::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
    // ���͂ɗ͂�������B
    velocity.x += impulse.x;
    velocity.y += impulse.y;
    velocity.z += impulse.z;
}

// ���͏����X�V
void Character::UpdateVelocity(float elapsedTime)
{
    // �o�߃t���[��
    float elapsedFrame = 60.0f * elapsedTime;

    // �������͍X�V����
    UpdateVerticalVelocity(elapsedFrame);

    // �������͍X�V����
    UpdateHorizontalVelocity(elapsedFrame);

    // �����ړ��X�V����
    UpdateVerticalMove(elapsedTime);

    // �����ړ��X�V����
    UpdateHorizontalMove(elapsedTime);
}

// �������͍X�V����
void Character::UpdateVerticalVelocity(float elapsedFrame)
{
    // �d�͏���
    velocity.y += gravity * elapsedFrame;
}

// �����ړ��X�V����
void Character::UpdateVerticalMove(float elapsedTime)
{
    // �ړ�����
    this->position.y += velocity.y * elapsedTime;

    // �n�ʔ���
    if (position.y < 0.0f)
    {
        position.y = 0.0f;
        // ���n����
        if (!IsGround())
        {
            OnLanding();
        }

        isGround = true;
        velocity.y = 0.0f;
    }
    else
    {
        isGround = false;
    }
}

// �������͍X�V����
void Character::UpdateHorizontalVelocity(float elapsedFrame)
{
    // XZ���ʂ̑��͌�������
    float length = sqrtf(powf(velocity.x, 2) + powf(velocity.z, 2));
    if (length > 0.0f)
    {
        // ���C��
        float friction = this->friction * elapsedFrame;
        // �󒆂ɂ���Ƃ��͖��C�͂����炷
        if(!IsGround()) friction *= this->airControl;
        // ���C�ɂ�鉡�����̌�������
        if (length > friction)
        {
            velocity.x -= (velocity.x / length) * friction;
            velocity.z -= (velocity.z / length) * friction;
        }
        // �������̑��͂����C�͈ȉ��ɂȂ����̂ő��͂𖳗͉�
        else
        {
            velocity.x = 0.0f;
            velocity.z = 0.0f;
        }
    }
    // XZ���ʂ̑��͂���������
    if (length <= maxMoveSpeed)
    {
        // �ړ��x�N�g�����[���x�N�g���o�Ȃ��Ȃ��������
        float moveVecLength = sqrtf(powf(moveVecX, 2) + powf(moveVecZ, 2));
        if (moveVecLength > 0.0f)
        {
            // ������
            float acceleration = this->acceleration * elapsedFrame;
            // �󒆂ɂ���Ƃ��͉����͂����炷
            if (!IsGround()) acceleration *= this->airControl;
            // �ړ��x�N�g���ɂ���������
            velocity.x += moveVecX * acceleration;
            velocity.z += moveVecZ * acceleration;

            // �ő呬�x����
            float length = sqrtf(powf(velocity.x, 2) + powf(velocity.z, 2));
            if (length > maxMoveSpeed)
            {
                velocity.x = (velocity.x / length) * maxMoveSpeed;
                velocity.z = (velocity.z / length) * maxMoveSpeed;
            }
        }
    }
    // �ړ��x�N�g�������Z�b�g
    moveVecX = 0.0f;
    moveVecZ = 0.0f;
}

// �����ړ��X�V����
void Character::UpdateHorizontalMove(float elapsedTime)
{
    // �ړ�����
    this->position.x += velocity.x * elapsedTime;
    this->position.z += velocity.z * elapsedTime;
}