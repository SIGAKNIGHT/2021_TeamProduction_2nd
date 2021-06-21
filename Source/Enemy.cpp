#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "EnemyManager.h"

// �f�o�b�O�v���~�e�B�u�`��
void Enemy::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
    debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

// �j��
void Enemy::Destroy()
{
    EnemyManager::Instance().Remove(this);
}