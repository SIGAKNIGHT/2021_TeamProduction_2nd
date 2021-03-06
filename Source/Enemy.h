#pragma once
#include "Graphics/Shader.h"
#include "Character.h"

class Enemy : public Character
{
public:
    Enemy() {}
    ~Enemy() override {}

    // 更新処理
    virtual void Update(float elapsedTime) = 0;

    // 描画処理
    virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;
    virtual void DrawDebugPrimitive();

    // 破棄
    void Destroy();

    // ソート用関数
    void SetLengthSq(float sq) { lengthSq = sq; }
    float GetLengthSq() const { return lengthSq; }
    DirectX::XMFLOAT3 GetHeadPos() const { return headPos; }
    float GetHeadRadius() const { return headRadius; }

protected:
    float lengthSq;
    float headRadius = 0.2f;
    DirectX::XMFLOAT3 headPos = {0,0,0};

};