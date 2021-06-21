#include "Character.h"
#include "CameraController.h"

// 行列更新処理   
void Character::UpdateTransform()
{
    // スケール行列を作成
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    // 回転行列を作成
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    // 位置行列を作成
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    // 3つの行列を組み合わせ、ワールド行列を作成
    DirectX::XMMATRIX W = S * R * T;
    //計算したワールド行列を取り出す
    DirectX::XMStoreFloat4x4(&transform, W);

}

// ダメージを与える
bool Character::ApplyDamage(int damage, float invincibleTime)
{
    // ダメージが0の場合は健康状態を変更する必要がない
    if (damage == 0) return false;

    // 死亡している場合は健康状態を変更しない
    if (health <= 0) return false;
    
    // ダメージ処理
    if (invincibleTimer > 0.0f) return false;
    invincibleTimer = invincibleTime;

    health -= damage;


    // 死亡通知
    if (health <= 0)
    {
        OnDead();
    }
    // ダメージ通知
    else
    {
        OnDamage();
    }
    // 健康状態が変更した場合はtrueを返す
    return true;
}

// 無敵時間更新
void Character::UpdateInvincibleTimer(float elapsedTime)
{
    if (invincibleTimer >= 0.0f)
    {
        invincibleTimer -= elapsedTime;
    }
}

void Character::Move(float vx, float vz, float speed)
{
    // 移動方向ベクトルを設定
    moveVecX = vx;
    moveVecZ = vz;

    // 最大速度設定
    maxMoveSpeed = speed;
}

// 旋回処理
void Character::Turn(float elapsedTime, float vx, float vz, float speed)
{
    speed *= elapsedTime;

    // 進行ベクトルがゼロベクトルの場合は処理する必要なし
    float length = sqrt(pow(vx, 2) + pow(vz, 2));
    if (length < 0.001f)
    {
        return;
    }

    // 進行ベクトルを単位ベクトル化
    vx = vx / sqrt(pow(vx, 2) + pow(vz, 2));
    vz = vz / sqrt(pow(vx, 2) + pow(vz, 2));

    // 自身の回転値から前方向ベクトルを求める
    float frontX = sinf(angle.y);
    float frontZ = cosf(angle.y);

    // 回転角を求めるため、2つの単位ベクトルの内積を計算する
    float dot = (frontX * vx) + (frontZ * vz); // 内積

    // 内積値は-1.0~1.0で表現されており、2つの単位ベクトルの角度が
    // 小さいほど1.0に近づくという性質を利用して回転速度を調整する
    float rot = 1.0 - dot; //　補正値
    if (rot > speed)
    {
        rot = speed;
    }

    // 左右判定を行うために2つの単位ベクトルの外積を計算する
    float cross = (frontX * vz) - (frontZ * vx);

    // 2Dの外積値が生の場合か府の場合によって左右判定が行える
    // 左右判定を行うことによって左右回転を選択する
    if (cross <= 0.0f) // 外積が負なので右に旋回
    {
        angle.y += rot;
    }
    else // 左に旋回
    {
        angle.y -= rot;
    }
}


// ジャンプ処理
void Character::Jump(float speed)
{
    // 上方向の力を設定
    velocity.y = speed;
}

// 衝撃を与える
void Character::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
    // 速力に力を加える。
    velocity.x += impulse.x;
    velocity.y += impulse.y;
    velocity.z += impulse.z;
}

// 速力処理更新
void Character::UpdateVelocity(float elapsedTime)
{
    // 経過フレーム
    float elapsedFrame = 60.0f * elapsedTime;

    // 垂直速力更新処理
    UpdateVerticalVelocity(elapsedFrame);

    // 水平速力更新処理
    UpdateHorizontalVelocity(elapsedFrame);

    // 垂直移動更新処理
    UpdateVerticalMove(elapsedTime);

    // 水平移動更新処理
    UpdateHorizontalMove(elapsedTime);
}

// 垂直速力更新処理
void Character::UpdateVerticalVelocity(float elapsedFrame)
{
    // 重力処理
    velocity.y += gravity * elapsedFrame;
}

// 垂直移動更新処理
void Character::UpdateVerticalMove(float elapsedTime)
{
    // 移動処理
    this->position.y += velocity.y * elapsedTime;

    // 地面判定
    if (position.y < 0.0f)
    {
        position.y = 0.0f;
        // 着地した
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

// 水平速力更新処理
void Character::UpdateHorizontalVelocity(float elapsedFrame)
{
    // XZ平面の速力減速する
    float length = sqrtf(powf(velocity.x, 2) + powf(velocity.z, 2));
    if (length > 0.0f)
    {
        // 摩擦力
        float friction = this->friction * elapsedFrame;
        // 空中にいるときは摩擦力を減らす
        if(!IsGround()) friction *= this->airControl;
        // 摩擦による横方向の減速処理
        if (length > friction)
        {
            velocity.x -= (velocity.x / length) * friction;
            velocity.z -= (velocity.z / length) * friction;
        }
        // 横方向の速力が摩擦力以下になったので速力を無力化
        else
        {
            velocity.x = 0.0f;
            velocity.z = 0.0f;
        }
    }
    // XZ平面の速力を加速する
    if (length <= maxMoveSpeed)
    {
        // 移動ベクトルがゼロベクトル出ないなら加速する
        float moveVecLength = sqrtf(powf(moveVecX, 2) + powf(moveVecZ, 2));
        if (moveVecLength > 0.0f)
        {
            // 加速力
            float acceleration = this->acceleration * elapsedFrame;
            // 空中にいるときは加速力を減らす
            if (!IsGround()) acceleration *= this->airControl;
            // 移動ベクトルによる加速処理
            velocity.x += moveVecX * acceleration;
            velocity.z += moveVecZ * acceleration;

            // 最大速度制限
            float length = sqrtf(powf(velocity.x, 2) + powf(velocity.z, 2));
            if (length > maxMoveSpeed)
            {
                velocity.x = (velocity.x / length) * maxMoveSpeed;
                velocity.z = (velocity.z / length) * maxMoveSpeed;
            }
        }
    }
    // 移動ベクトルをリセット
    moveVecX = 0.0f;
    moveVecZ = 0.0f;
}

// 水平移動更新処理
void Character::UpdateHorizontalMove(float elapsedTime)
{
    // 移動処理
    this->position.x += velocity.x * elapsedTime;
    this->position.z += velocity.z * elapsedTime;
}