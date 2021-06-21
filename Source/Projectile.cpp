#include "Projectile.h"
#include "ProjectileManager.h"
#include "Graphics/Graphics.h"

// �R���X�g���N�^
Projectile::Projectile(ProjectileManager* manager) : manager(manager)
{
    // projectile�������Ɏ�����ProjectileManager�ɓo�^����
    manager->Register(this);
}
// �f�o�b�O�v���~�e�B�u�`��
void Projectile::DrawDebugPrimitive()
{
    // DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
    // debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));
}

void Projectile::UpdateVelocity(float elapsedTime)
{
    // �o�߃t���[��
    float elapsedFrame = 60.0f * elapsedTime;

    // �������͍X�V����
    UpdateVerticalVelocity(elapsedFrame);

    // �����ړ��X�V����
    UpdateVerticalMove(elapsedTime);
}

// �������͍X�V����
void Projectile::UpdateVerticalVelocity(float elapsedFrame)
{
    // �d�͏���
    velocity.y += gravity * elapsedFrame;
}

// �����ړ��X�V����
void Projectile::UpdateVerticalMove(float elapsedTime)
{
    // �ړ�����
    direction.y += velocity.y * elapsedTime;
}


// �s��X�V����
void Projectile::UpdateTransform()
{
    // �Ƃ肠�����A���ŉ�]�𖳎������s����쐬����B
   //  transform._11 = scale.x;
   //  transform._12 = 0.0f;
   //  transform._13 = 0.0f;
   //  transform._14 = 0.0f;
   //  transform._21 = 0.0f;
   //  transform._22 = scale.y;
   //  transform._23 = 0.0f;
   //  transform._24 = 0.0f;
   //  transform._31 = 0.0f;
   //  transform._32 = 0.0f;
   //  transform._33 = scale.z;
   //  transform._34 = 0.0f;
   //  transform._41 = position.x;
   //  transform._42 = position.y;
   //  transform._43 = position.z;
   //  transform._44 = 1.0f;
    DirectX::XMVECTOR Front, Up, Right;
    // �O�����x�N�g�� ���K���K�v
    Front = DirectX::XMLoadFloat3(&direction);
    Front = DirectX::XMVector3Normalize(Front);

    // ���̏�x�N�g���Z�o
    // ���̏�x�N�g�������Ƃ��͂�����Ƃ��炷�B2�̃x�N�g�������������������Ă���΁A�O�ς̏o���悤���Ȃ��B�W���o�����b�N
    Up = DirectX::XMVectorSet(0.0001f, 1.0f, 0.0f, 0.0f);
    // Up = DirectX::XMVectorSet(position.x, position.y + 1.0f, position.z, 0.0f);
    Up = DirectX::XMVector3Normalize(Up);

    // �E�x�N�g�����Z�o
    Right = DirectX::XMVector3Cross(Up, Front);
    // ��x�N�g�����Z�o
    Up = DirectX::XMVector3Cross(Front, Right);
    // �v�Z���ʂ����o��
    DirectX::XMFLOAT3 right, up, front;
    DirectX::XMStoreFloat3(&right, Right);
    DirectX::XMStoreFloat3(&up, Up);
    DirectX::XMStoreFloat3(&front, Front);

    // �Z�o�������x�N�g������s����쐬
   transform._11 = scale.x * right.x;
   transform._12 = scale.x * right.y;
   transform._13 = scale.x * right.z;
   transform._14 = 0.0f;
   transform._21 = scale.y * up.x;
   transform._22 = scale.y * up.y;
   transform._23 = scale.y * up.z;
   transform._24 = 0.0f;
   transform._31 = scale.z * front.x;
   transform._32 = scale.z * front.y;
   transform._33 = scale.z * front.z;
   transform._34 = 0.0f;
   transform._41 = position.x;
   transform._42 = position.y;
   transform._43 = position.z;
   transform._44 = 1.0f;

   // ���˕���
   this->direction = front;

}

// �j��
void Projectile::Destroy()
{
    // ProjectileManager���玩�����폜����
    manager->Remove(this);
}
