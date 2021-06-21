#include "Projectile.h"
#include "ProjectileManager.h"
#include "Graphics/Graphics.h"

// コンストラクタ
Projectile::Projectile(ProjectileManager* manager) : manager(manager)
{
    // projectile生成時に自分をProjectileManagerに登録する
    manager->Register(this);
}
// デバッグプリミティブ描画
void Projectile::DrawDebugPrimitive()
{
    // DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
    // debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));
}

void Projectile::UpdateVelocity(float elapsedTime)
{
    // 経過フレーム
    float elapsedFrame = 60.0f * elapsedTime;

    // 垂直速力更新処理
    UpdateVerticalVelocity(elapsedFrame);

    // 垂直移動更新処理
    UpdateVerticalMove(elapsedTime);
}

// 垂直速力更新処理
void Projectile::UpdateVerticalVelocity(float elapsedFrame)
{
    // 重力処理
    velocity.y += gravity * elapsedFrame;
}

// 垂直移動更新処理
void Projectile::UpdateVerticalMove(float elapsedTime)
{
    // 移動処理
    direction.y += velocity.y * elapsedTime;
}


// 行列更新処理
void Projectile::UpdateTransform()
{
    // とりあえず、仮で回転を無視した行列を作成する。
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
    // 前方向ベクトル 正規化必要
    Front = DirectX::XMLoadFloat3(&direction);
    Front = DirectX::XMVector3Normalize(Front);

    // 仮の上ベクトル算出
    // 仮の上ベクトルを作るときはちょっとずらす。2つのベクトルが同じ方向を向いていれば、外積の出しようがない。ジンバルロック
    Up = DirectX::XMVectorSet(0.0001f, 1.0f, 0.0f, 0.0f);
    // Up = DirectX::XMVectorSet(position.x, position.y + 1.0f, position.z, 0.0f);
    Up = DirectX::XMVector3Normalize(Up);

    // 右ベクトルを算出
    Right = DirectX::XMVector3Cross(Up, Front);
    // 上ベクトルを算出
    Up = DirectX::XMVector3Cross(Front, Right);
    // 計算結果を取り出す
    DirectX::XMFLOAT3 right, up, front;
    DirectX::XMStoreFloat3(&right, Right);
    DirectX::XMStoreFloat3(&up, Up);
    DirectX::XMStoreFloat3(&front, Front);

    // 算出した軸ベクトルから行列を作成
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

   // 発射方向
   this->direction = front;

}

// 破棄
void Projectile::Destroy()
{
    // ProjectileManagerから自分を削除する
    manager->Remove(this);
}
