#include "EnemySlime.h"

// コンストラクタ
EnemySlime::EnemySlime()
{
    model = new Model("Data/Model/Body/little_witch.mdl");
    // model->PlayAnimation(0, true, 0.1);

    // モデルが大きいのでスケーリング
    scale.x = scale.y = scale.z = 0.01f;

    // 幅高さ設定
    radius = 0.5f;
    height = 1.0f;
}

// デストラクタ
EnemySlime::~EnemySlime()
{
    delete model;
}

// 更新処理
void EnemySlime::Update(float elapsedTime)
{
    // 速力処理更新
    UpdateVelocity(elapsedTime);
    // 無敵時間更新
    UpdateInvincibleTimer(elapsedTime);
    // オブジェクト行列を更新
    UpdateTransform();
    // モデル行列更新
    // model->UpdateAnimation(elapsedTime);
    model->UpdateTransform(transform);
}

// 死亡した時に呼ばれる
void EnemySlime::OnDead()
{
    Destroy();
}

// 描画処理
void EnemySlime::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);
}