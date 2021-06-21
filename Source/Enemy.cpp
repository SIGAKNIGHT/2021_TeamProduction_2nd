#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "EnemyManager.h"

// デバッグプリミティブ描画
void Enemy::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
    debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

// 破棄
void Enemy::Destroy()
{
    EnemyManager::Instance().Remove(this);
}