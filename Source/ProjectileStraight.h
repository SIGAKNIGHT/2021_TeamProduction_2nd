#pragma once

#include "Graphics/Model.h"
#include "Projectile.h"

// ���i�e��
class ProjectileStraight : public Projectile
{
public:
    ProjectileStraight(ProjectileManager* manager);
    ~ProjectileStraight() override;

    // �X�V����
    void Update(float elapsedTime) override;

    // �`�揈��
    void Render(ID3D11DeviceContext* dc, Shader* shader) override;

    // ����
    void Launch (const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position, float speed);

private:
    Model* model = nullptr;
    float speed = 75.0f;
    float lifeTimer = 3.0f;
};