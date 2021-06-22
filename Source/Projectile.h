#pragma once

#include "Graphics/Shader.h"

// �O���錾�@�錾�����O�ɃN���X���g�p�������ꍇ�Ɏg�p
// (���̏ꍇProjectile class��ProjectileManager���g�p����)
class ProjectileManager;


// �e��
class Projectile
{
public:
    Projectile(ProjectileManager* manager);
    virtual ~Projectile() {}

    // �X�V����
    virtual void Update(float elapsedTime) = 0;
    // �`�揈��
    virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;
    // �f�o�b�O�v���~�e�B�u�`��
    virtual void DrawDebugPrimitive();
    // �ʒu�擾
    const DirectX::XMFLOAT3& GetPosition() const { return position; }
    // �����擾
    const DirectX::XMFLOAT3& GetDirection() const { return direction; }
    // �X�P�[���擾
    const DirectX::XMFLOAT3& GetScale() const { return scale; }
    // ���a�擾
    float GetRadius() const { return radius; }
    //  HACK�Ռ��l�擾:�����Œǉ�
    float GetImpluse() const { return impluse; }
    // �j��
    void Destroy();


protected:
    // �s��X�V����
    void UpdateTransform();
    // ���͍X�V����
    void UpdateVelocity(float elapsedTime);

private:
    // �������͍X�V����
    void UpdateVerticalVelocity(float elapsedFrame);

    // �����ړ��X�V����
    void UpdateVerticalMove(float elapsedTime);

protected:
    DirectX::XMFLOAT3 position = { 0, 0, 0 };
    DirectX::XMFLOAT3 direction = { 0, 0, 1 };
    DirectX::XMFLOAT3 scale = { 1, 1, 1 };
    DirectX::XMFLOAT4X4 transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
    ProjectileManager* manager = nullptr;
    float radius = 0.5f;
    //  HACK�Ռ��l:�����Œǉ��@�e�e���ƂɕύX����B
    DirectX::XMFLOAT3 velocity = { 0,0,0 };
    float impluse = 5.0f;
    float gravity = -0.05f;
};