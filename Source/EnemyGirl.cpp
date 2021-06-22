#include "EnemyGirl.h"
#include "Graphics/Graphics.h"
#include "Mathf.h"
#include "Player.h"
#include "Collision.h"

// コンストラクタ
EnemyGirl::EnemyGirl()
{
    model = new Model("Data/Model/Body/girl.mdl");
    model->PlayAnimation(0, true, 0.1);
    angle.y = DirectX::XM_PI;
    

    // モデルが大きいのでスケーリング
    scale.x = scale.y = scale.z = 0.01f;

    // 幅高さ設定
    radius = 0.5f;
    height = 1.0f;
}

// デストラクタ
EnemyGirl::~EnemyGirl()
{
    delete model;
}

// 更新処理
void EnemyGirl::Update(float elapsedTime)
{
    // 速力処理更新
    UpdateVelocity(elapsedTime);
    // 無敵時間更新
    UpdateInvincibleTimer(elapsedTime);
    SetHeadPos("head", headRadius);
    // オブジェクト行列を更新
    UpdateTransform();
    // モデル行列更新
    model->UpdateAnimation(elapsedTime);
    model->UpdateTransform(transform);
}

// 死亡した時に呼ばれる
void EnemyGirl::OnDead()
{
    Destroy();
}

// 描画処理
void EnemyGirl::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);
}

void EnemyGirl::SetHeadPos(const char* nodeName, float nodeRadius)
{
    // ノードの位置と当たり判定を行う
    Model::Node* node = model->FindNode(nodeName);
    if (node != nullptr)
    {
        // ノードのワールド座標
        DirectX::XMFLOAT3 nodePosition(
            node->worldTransform._41,
            node->worldTransform._42,
            node->worldTransform._43
        );
        headPos = nodePosition;
        // 当たり判定表示
        Graphics::Instance().GetDebugRenderer()->DrawSphere(
            nodePosition, nodeRadius, DirectX::XMFLOAT4(0, 0, 1, 1)
        );
    }
}