#pragma once

#include "Graphics/Model.h"
#include "Enemy.h"

// スライム
class EnemySlime : public Enemy
{
public:
    EnemySlime();
    ~EnemySlime() override;

    // 更新処理
    void Update(float elapsedTime) override;

    // 更新処理
    void Render(ID3D11DeviceContext* dc, Shader* shader)override;

    // デバッグプリミティブ描画
    void DrawDebugPrimitive() override;

    // 縄張り設定
    void SetTerritory(const DirectX::XMFLOAT3& origin, float range);

private:
    // ターゲット位置をランダム設定
    void SetRandomTargetposition();

    // 目標地点へ移動
    void MoveToTarget(float elapsedTime, float speedRate);

    // プレイヤー索敵
    bool SearchPlayer();

    // ノードとプレイヤーの衝突処理
    void CollisionNodeVsPlayer(const char* nodeName, float boneRadius);

    // 徘徊ステートへ遷移
    void TransitionWanderState();
    // 徘徊ステート更新処理
    void UpdateWanderState(float elapsedTime);

    // 待機ステートへ移動
    void TransitionIdleState();
    // 待機ステート更新処理
    void UpdateIdleState(float elapsedTime);

    // 追跡ステートへ遷移
    void TransitionPursuitState();
    // 追跡ステート更新処理
    void UpdatePursuitState(float elapsedTime);

    // 攻撃ステートへ遷移
    void TransitionAttackState();
    // 攻撃ステート更新処理
    void UpdateAttackState(float elapsedTime);

    // 戦闘待機ステートへ遷移
    void TransitionIdleBattleState();
    // 戦闘待機ステート更新処理
    void UpdateIdleBattleState(float elapsedTime);

    // ダメージステートへ遷移
    void TransitionDamageState();
    // ダメージステート更新処理
    void UpdateDamageState(float elapsedTime);

    // 死亡ステートへ遷移
    void TransitionDeathState();
    // 死亡ステート更新処理
    void UpdateDeathState(float elapsedTime);

protected:
    //  死亡した時に呼ばれる
    void OnDead() override;
    // ダメージをうけた時に呼ばれる
    void OnDamage() override;

private:
    // ステート
    enum class State
    {
        Wander,
        Idle,
        Pursuit,
        Attack,
        IdleBattle,
        Damage,
        Death,
    };

    // アニメーション
    enum Animation
    {
        Anim_IdleNormal,
        Anim_IdleBattle,
        Anim_Attack1,
        Anim_Attack2,
        Anim_WalkFWD,
        Anim_WalkBWD,
        Anim_WalkLeft,
        Anim_WalkRight,
        Anim_RunFWD,
        Anim_SenseSomethingST,
        Anim_SeneSomethingPRT,
        Anim_Taunt,
        Anim_Victory,
        Anim_GetHit,
        Anim_Dizzy,
        Anim_Die,
    };

private:
    Model* model = nullptr;
    State state = State::Wander;
    DirectX::XMFLOAT3 targetPosition = { 0,0,0 };
    DirectX::XMFLOAT3 territoryOrigin = { 0,0,0 };
    float territoryRange = 10.0f;
    float moveSpeed = 3.0f;
    float turnSpeed = DirectX::XMConvertToRadians(360);
    float stateTimer = 0.0f;
    float searchRange = 5.0f;
    float attackRange = 1.5f;
};