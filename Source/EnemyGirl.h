#pragma once

#include "Graphics/Model.h"
#include "Enemy.h"

// スライム
class EnemyGirl : public Enemy
{
public:
    EnemyGirl();
    ~EnemyGirl() override;

    // 更新処理
    void Update(float elapsedTime) override;

    // 更新処理
    void Render(ID3D11DeviceContext* dc, Shader* shader)override;

    // デバッグプリミティブ描画
   //  void DrawDebugPrimitive() override;

    // // 縄張り設定
    // void SetTerritory(const DirectX::XMFLOAT3& origin, float range);

private:
    // // ターゲット位置をランダム設定
    // void SetRandomTargetposition();
    // 
    // // 目標地点へ移動
    // void MoveToTarget(float elapsedTime, float speedRate);
    // 
    // // 俳諧ステートへ遷移
    // void TransitionWanderState();
    // // 徘徊ステート更新処理
    // void UpdateWanderState(float elapsedTime);
        // ノードとプレイヤーの衝突処理
    void SetHeadPos(const char* nodeName, float boneRadius);

protected:
    //  死亡した時に呼ばれる
    void OnDead() override;

private:
    Model* model = nullptr;
    // ステート
    enum class State
    {
        Wander
    };

    // アニメーション
    enum Animation
    {

    };
};