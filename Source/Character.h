#pragma once

#include <DirectXMath.h>

// キャラクター
class Character
{
public:
    Character() {}
    virtual ~Character() {}

    // 行列更新処理
    void UpdateTransform();
    // 位置取得
    const DirectX::XMFLOAT3& GetPosition() const { return position; }
    // 位置設定
    void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }
    // 回転取得
    const DirectX::XMFLOAT3& GetAngle() const { return angle; }
    // 回転設定
    void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }
    // スケール取得
    const DirectX::XMFLOAT3& GetScale() const { return scale; }
    // スケール設定
    void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
    //半径取得
    float GetRadius() const { return radius; }
    // 地面に接地しているか
    bool IsGround() const { return isGround; }
    // 高さ取得
    float GetHeight() const { return height; }
    // ダメージを与える
    bool ApplyDamage(int damage, float invincibleTime);
    // 衝撃を与える
    void AddImpulse(const DirectX::XMFLOAT3& impulse);

private:
    // 垂直速力更新処理
    void UpdateVerticalVelocity(float elapsedFrame);

    // 垂直移動更新処理
    void UpdateVerticalMove(float elapsedTime);

    // 水平速力更新処理
    void UpdateHorizontalVelocity(float elapsedFrame);
    
    // 水平移動更新処理
    void UpdateHorizontalMove(float elapsedTime);

protected:
    // 移動処理
    void Move(float vx, float vz, float speed);
    // 旋回処理
    void Turn(float elapsedTime, float vx, float vz, float speed);
    // ジャンプ処理
    void Jump(float speed);
    // 速力更新処理
    void UpdateVelocity(float elapsedTime);

    // 着地した時に呼ばれる
    virtual void OnLanding(){}

    // ダメージを受けたときに呼ばれる
    virtual void OnDamage(){}

    // 死亡した時に呼ばれる
    virtual void OnDead(){}

    // 無敵時間更新
    void UpdateInvincibleTimer(float elapsedTime);


protected:
    DirectX::XMFLOAT3 position = { 0,0,0 };
    DirectX::XMFLOAT3 angle = { 0,0,0 };
    DirectX::XMFLOAT3 scale = { 1,1,1 };
    DirectX::XMFLOAT4X4 transform = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1,
    };
    float radius = 0.5f;
    float gravity = -1.0f;
    DirectX::XMFLOAT3 velocity = { 0,0,0 };
    bool isGround = false;
    float height = 2.0f;
    int health = 5;
    float  invincibleTimer = 0.0f;
    float friction = 0.5f;
    float acceleration = 1.0f;
    float maxMoveSpeed = 5.0f;
    float moveVecX = 0.0f;
    float moveVecZ = 0.0f;
    float airControl = 0.3f;
    int maxHealth = 5;
};