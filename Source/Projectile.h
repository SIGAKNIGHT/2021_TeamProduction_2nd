#pragma once

#include "Graphics/Shader.h"

// 前方宣言　宣言される前にクラスを使用したい場合に使用
// (この場合Projectile classでProjectileManagerを使用する)
class ProjectileManager;


// 弾丸
class Projectile
{
public:
    Projectile(ProjectileManager* manager);
    virtual ~Projectile() {}

    // 更新処理
    virtual void Update(float elapsedTime) = 0;
    // 描画処理
    virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;
    // デバッグプリミティブ描画
    virtual void DrawDebugPrimitive();
    // 位置取得
    const DirectX::XMFLOAT3& GetPosition() const { return position; }
    // 方向取得
    const DirectX::XMFLOAT3& GetDirection() const { return direction; }
    // スケール取得
    const DirectX::XMFLOAT3& GetScale() const { return scale; }
    // 半径取得
    float GetRadius() const { return radius; }
    //  HACK衝撃値取得:自分で追加
    float GetImpluse() const { return impluse; }
    // 破棄
    void Destroy();


protected:
    // 行列更新処理
    void UpdateTransform();
    // 速力更新処理
    void UpdateVelocity(float elapsedTime);

private:
    // 垂直速力更新処理
    void UpdateVerticalVelocity(float elapsedFrame);

    // 垂直移動更新処理
    void UpdateVerticalMove(float elapsedTime);

protected:
    DirectX::XMFLOAT3 position = { 0, 0, 0 };
    DirectX::XMFLOAT3 direction = { 0, 0, 1 };
    DirectX::XMFLOAT3 scale = { 1, 1, 1 };
    DirectX::XMFLOAT4X4 transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
    ProjectileManager* manager = nullptr;
    float radius = 0.5f;
    //  HACK衝撃値:自分で追加　各弾ごとに変更する。
    DirectX::XMFLOAT3 velocity = { 0,0,0 };
    float impluse = 5.0f;
    float gravity = -0.05f;
};