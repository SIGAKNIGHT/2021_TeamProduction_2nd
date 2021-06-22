#pragma once
#include "Graphics/Shader.h"
#include "Character.h"

class Enemy : public Character
{
public:
    Enemy() {}
    ~Enemy() override {}

    // �X�V����
    virtual void Update(float elapsedTime) = 0;

    // �`�揈��
    virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;
    virtual void DrawDebugPrimitive();

    // �j��
    void Destroy();

    // �\�[�g�p�֐�
    void SetLengthSq(float sq) { lengthSq = sq; }
    float GetLengthSq() const { return lengthSq; }
    DirectX::XMFLOAT3 GetHeadPos() const { return headPos; }
    float GetHeadRadius() const { return headRadius; }

protected:
    float lengthSq;
    float headRadius = 0.2f;
    DirectX::XMFLOAT3 headPos = {0,0,0};

};