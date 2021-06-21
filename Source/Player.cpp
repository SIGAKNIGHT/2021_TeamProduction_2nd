#include <imgui.h>
#include "Player.h"
#include "Camera.h"
#include "Input/Input.h"
#include "Graphics/Graphics.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "ProjectileStraight.h"
#include "ProjectileHoming.h"
#include "CameraController.h"

// コンストラクタ
Player::Player()
{
    //model = new Model("Data/Model/Mr.Incredible/Mr.Incredible.mdl");
    model = new Model("Data/Model/Jummo/Jummo.mdl");
    // model->PlayAnimation(0);

    // モデルが大きいのでスケーリング
    scale.x = scale.y = scale.z = 0.01f;

    // ヒットエフェクト読み込み
    hitEffect = new Effect("Data/Effect/Hit.efk");
    
    // 待機ステートへ遷移
    TransitionIdleState();
}

// デストラクタ
Player::~Player()
{
    delete hitEffect;
    delete model;
}

//　更新処理
void Player::Update(float elapsedTime)
{
    // ステート毎の処理
    switch (state)
    {
    case State::Idle:
        UpdateIdleState(elapsedTime);
        break;
    case State::Move:
        UpdateMoveState(elapsedTime);
        break;
    case State::Jump:
        UpdateJumpState(elapsedTime);
        break;
    case State::Land:
        UpdateLandState(elapsedTime);
        break;
    case State::Attack:
        UpdateAttackState(elapsedTime);
        break;
    }
    // 速力更新処理
    UpdateVelocity(elapsedTime);

    // 弾丸更新処理
    projectileManager.Update(elapsedTime);

    // プレイヤーと敵の衝突処理
    CollisionPlayerVsEnemies();

    // 弾丸と敵の衝突判定
    CollisionProjectilesVsEnemies();

    // オブジェクト行列を更新
    UpdateTransform();
    
    // モデルアニメーション更新処理
    model->UpdateAnimation(elapsedTime);

    // モデル行列の更新
    model->UpdateTransform(transform);
}

// 描画処理
void Player::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);
    projectileManager.Render(dc, shader);
}

// デバッグ用GUI描画
void Player::DrawDebugGUI()
{
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
    {
        // トランスフォーム
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // 位置
            ImGui::InputFloat3("Position", &position.x);
            // 回転
            DirectX::XMFLOAT3 a;
            a.x = DirectX::XMConvertToDegrees(angle.x);
            a.y = DirectX::XMConvertToDegrees(angle.y);
            a.z = DirectX::XMConvertToDegrees(angle.z);
            ImGui::InputFloat3("Angle", &a.x);
            angle.x = DirectX::XMConvertToRadians(a.x);
            angle.y = DirectX::XMConvertToRadians(a.y);
            angle.z = DirectX::XMConvertToRadians(a.z);
            // スケール
            ImGui::InputFloat3("Scale", &scale.x);
        }
    }
    ImGui::End();
}

// デバッグプリミティブ描画
void Player::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    // 衝突判定用のデバッグ球を描画
    debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
    projectileManager.DrawDebugPrimitive();


    // 攻撃衝突用の左手ノードのデバッグ球を描画
    if (attackCollisionFlag)
    {
        Model::Node* leftHandBone = model->FindNode("mixamorig:LeftHand");
        debugRenderer->DrawSphere(
            DirectX::XMFLOAT3(
            leftHandBone->worldTransform._41,
            leftHandBone->worldTransform._42,
            leftHandBone->worldTransform._43),
            lehtHandRadius,
            DirectX::XMFLOAT4(1, 0, 0, 1)
        );
    }
}

DirectX::XMFLOAT3 Player::GetMoveVec() const
{
    // 入力情報を取得
    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

    //  カメラ方向とスティックの入力値によって進行方向を計算する
    Camera& camera = Camera::Instance();
    const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
    const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

    // 移動ベクトルはXZ平面に水平なベクトルになるようにする。

    // カメラ右方向ベクトルをXZ単位ベクトルに変換
    float cameraRightX = cameraRight.x;
    float cameraRightZ = cameraRight.z;
    float cameraRightLenght = sqrtf(powf(cameraRightX, 2) + powf(cameraRightZ, 2));
    if (cameraRightLenght > 0.0f)
    {
        cameraRightX /= cameraRightLenght;
        cameraRightZ /= cameraRightLenght;
    }

    // カメラ前方向ベクトルをXZ単位ベクトルに変換
    float cameraFrontX = cameraFront.x;
    float cameraFrontZ = cameraFront.z;
    float cameraFrontLenght = sqrtf(powf(cameraFrontX, 2) + powf(cameraFrontZ, 2));
    if (cameraFrontLenght > 0.0f)
    {
        cameraFrontX /= cameraFrontLenght;
        cameraFrontZ /= cameraFrontLenght;
    }

    // スティックの水平入力値をカメラ方向に反映し、
    // スティックの垂直入力値を計算する。
    // 進行ベクトルを計算する。
    DirectX::XMFLOAT3 vec;
    vec.x = cameraFrontX  + cameraRightX ;
    vec.z = cameraFrontZ  + cameraRightZ ;
    // Y軸には移動しない
    vec.y = 0.0f;
    return vec;
}

bool Player::InputMove(float elapsedTime)
{
    // 進行ベクトル取得
    DirectX::XMFLOAT3 moveVec = GetMoveVec();

    // 移動処理
    // Move(moveVec.x, moveVec.z, moveSpeed);

    // 旋回処理
    // Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

    // 進行ベクトルがゼロベクトルでない場合は入力された // 三項演算子(評価式 ? 真 : 偽)
    return (sqrt(powf(moveVecX, 2) + powf(moveVecZ, 2)) != 0 ? true : false);
}

// プレイヤーとエネミーの衝突処理
void Player::CollisionPlayerVsEnemies()
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    // 全ての敵と総当たりで衝突処理
    int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy* enemy = enemyManager.GetEnemy(i);

        // 衝突処理
        DirectX::XMFLOAT3  outPosition;
       //if (Collision::IntersectSphereVsSphere(this->position, this->radius, enemy->GetPosition(), enemy->GetRadius(), outPosition))
       if (Collision::IntersectCylinderVsCylinder
       (this->position, this->radius,this->height, 
           enemy->GetPosition(), enemy->GetRadius(), enemy->GetHeight(), 
           outPosition))
       {
           DirectX::XMFLOAT3 norm;
           DirectX::XMVECTOR pl = DirectX::XMLoadFloat3(&this->position);
           DirectX::XMVECTOR el = DirectX::XMLoadFloat3(&enemy->GetPosition());
           DirectX::XMVECTOR Norm = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(pl, el));
           DirectX::XMStoreFloat3(&norm, Norm);
           if (norm.y > 0.75f)//this->position.y > enemy->GetPosition().y)
           {
               Jump(jumpSpeed * 0.5f);
               // ダメージを与える
               enemy->ApplyDamage(1, 0.1);
               continue;
           }
           enemy->SetPosition(outPosition);
       }
    }
}

// ノードと敵の衝突処理
void Player::CollisionNodeVsEnemies(const char* nodeName, float nodeRadius)
{
    // ノード取得
    Model::Node* node = model->FindNode(nodeName);

    // ノード位置取得
    DirectX::XMFLOAT3 nodePosition;
    nodePosition.y = node->worldTransform._42;
    nodePosition.x = node->worldTransform._41;
    nodePosition.z = node->worldTransform._43;

    // 指定のノードと全ての敵を総当たりで衝突処理
    EnemyManager& enemyManager = EnemyManager::Instance();

    // 全ての敵と総当たりで衝突処理
    int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy* enemy = enemyManager.GetEnemy(i);

        // 衝突処理
        DirectX::XMFLOAT3  outPosition;
        if (Collision::IntersectSphererVsCylinder
        (nodePosition, lehtHandRadius,
            enemy->GetPosition(), enemy->GetRadius(), enemy->GetHeight(),
            outPosition))
        {
            // ダメージを与える
            if (enemy->ApplyDamage(1, 0.5f))
            {
                // ヒットエフェクト再生
                {
                    DirectX::XMFLOAT3 e = enemy->GetPosition();
                    e.y += enemy->GetHeight() * 0.5f;
                    hitEffect->Play(e);
                }
            }
        }
    }

}

// ジャンプ入力処理
bool Player::InputJump()
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (gamePad.GetButtonDown() & GamePad::BTN_A)
    {
        if (jumpCount > 0)
        {
            Jump(jumpSpeed);
            jumpCount--;
            return true;
        }
    }
    return false;
}

// 攻撃入力処理
bool Player::InputAttack()
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    if (gamePad.GetButtonDown() & GamePad::BTN_B)
    {
        return true;
    }
    return false;
}

// 着地処理
void Player::OnLanding()
{
    jumpCount = jumpLimit;
    // 下方向の速力が一定以上なら着地ステートへ
    if (velocity.y < 0.0f)
    {
        TransitionLandState();
    }
}

// 弾丸入力処理
void Player::InputProjectile()
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();

    // 直進弾丸発射
    if (mouse.GetButton() & Mouse::BTN_LEFT)
    {
        projectileSpeed += 1.0f;
        if (projectileSpeed > 40.0f)
        {
            projectileSpeed = 40.0f;
        }
    }
    if (mouse.GetButtonUp() & Mouse::BTN_LEFT)
    {
        // 前方向
        DirectX::XMFLOAT3 dir;
        dir.x = sinf(angle.y);
        dir.y = sinf(-angle.x);
        dir.z = cosf(angle.y);
        // 発射位置(プレイヤーの腰あたり)
        DirectX::XMFLOAT3 pos;
        pos.x = this->position.x;
        pos.y = this->position.y + height * 0.5f;
        pos.z = this->position.z;
        // 発射
        ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);
        projectile->Launch(dir, pos, projectileSpeed);
        projectileSpeed = 10;
    }
}

// 弾丸と敵の衝突処理
void Player::CollisionProjectilesVsEnemies()
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    // 全ての弾丸と全ての敵を総当たりで衝突判定
    int projectileCount = projectileManager.GetProjectileCount();
    int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < projectileCount; ++i)
    {
        Projectile* projectile = projectileManager.GetProjectile(i);
        for (int j = 0; j < enemyCount; ++j)
        {
            Enemy* enemy = enemyManager.GetEnemy(j);
            // 衝突判定
            DirectX::XMFLOAT3 outPosition;
            if (Collision::IntersectSphererVsCylinder(
                projectile->GetPosition(),
                projectile->GetRadius(),
                enemy->GetPosition(),
                enemy->GetRadius(),
                enemy->GetHeight(),
                outPosition))
            {
                // ダメージを与える
                if (enemy->ApplyDamage(1, 0.5f))
                {
                    // 吹き飛ばす
                    {
                        DirectX::XMFLOAT3 impluse;
                        float xl = enemy->GetPosition().x - projectile->GetPosition().x;
                        float zl = enemy->GetPosition().z - projectile->GetPosition().z;
                        float xzLength = sqrtf(powf(xl, 2) + powf(zl, 2));
                        xl /= xzLength;
                        zl /= xzLength;
                        impluse.x = xl * projectile->GetImpluse();
                        impluse.y = projectile->GetImpluse();
                        impluse.z = zl * projectile->GetImpluse();
                        enemy->AddImpulse(impluse);
                    }

                    // ヒットエフェクト再生
                    {
                        DirectX::XMFLOAT3 e = enemy->GetPosition();
                        e.y += enemy->GetHeight() * 0.5f;
                        hitEffect->Play(e);
                    }
                    // 弾丸破棄
                    projectile->Destroy();
                }
            }
        }
    }
}

// 待機ステートへ遷移
void Player::TransitionIdleState()
{
    state = State::Idle;

    // 待機アニメーション再生
    model->PlayAnimation(Anim_Idle, true);
}

// 待機ステート更新処理
void Player::UpdateIdleState(float elapsedTime)
{
    // 移動入力処理
    if (InputMove(elapsedTime))
    {
        TransitionMoveState();
    }

    // ジャンプ入力処理
    if (InputJump())
    {
        TransitionJumpState();
    }

    // 弾丸入力処理
    InputProjectile();

    // 攻撃入力処理
    if (InputAttack())
    {
        TransitionAttackState();
    }
}

// 移動ステートへ遷移
void Player::TransitionMoveState()
{
    state = State::Move;

    // 走りアニメーション再生
    model->PlayAnimation(Anim_Running, true, 0.1);
}

// 移動ステート更新処理
void Player::UpdateMoveState(float elapsedTime)
{
    // 移動入力処理
    if (!InputMove(elapsedTime))
    {
        TransitionIdleState();
    }

    // ジャンプ入力処理
    if (InputJump())
    {
        TransitionJumpState();
    }

    // 攻撃入力処理
    if (InputAttack())
    {
        TransitionAttackState();
    }

    // 弾丸入力処理
    InputProjectile();
}

// ジャンプステートへ遷移
void Player::TransitionJumpState()
{
    state = State::Jump;
    // ジャンプアニメーション再生
    model->PlayAnimation(Anim_Jump, false, 0.1);
}

// ジャンプステート更新処理
void Player::UpdateJumpState(float elapsedTime)
{
    InputMove(elapsedTime);
    // ジャンプ入力処理
    if (InputJump())
    {
        model->PlayAnimation(Anim_Jump_Flip, false, 0.1);
    }
    if (!model->IsPlayAnimation())
    {
        model->PlayAnimation(Anim_Falling, true, 0.1);
    }
    // 弾丸入力処理
    InputProjectile();
}

// 着地ステートへ遷移
void Player::TransitionLandState()
{
    state = State::Land;

    // 着地アニメーション再生
    model->PlayAnimation(Anim_Landing, false, 0.1);
}

// 着地ステートの更新処理
void Player::UpdateLandState(float elapsedTime)
{
    // 移動入力処理
    if (!model->IsPlayAnimation())
    {
        TransitionIdleState();
    }
}

// 攻撃ステートへ遷移
void Player::TransitionAttackState()
{
    state = State::Attack;

    //  攻撃アニメーション再生
    model->PlayAnimation(Anim_Attack, false, 0.1);
}

// 攻撃ステート更新処理
void Player::UpdateAttackState(float elapsedTime)
{
    if (!model->IsPlayAnimation())
    {
        TransitionIdleState();
    }

    // 任意のアニメーション再生区間でのみ衝突判定処理をする
    float animationTime = model->GetanimeSeconds();
    if (animationTime > 0.3f && animationTime < 4.0f)
    {
        attackCollisionFlag = true;
    }
    else
    {
        attackCollisionFlag = false;
    }
    if (attackCollisionFlag)
    {
        // 左手ノードとエネミーの衝突処理
        CollisionNodeVsEnemies("mixamorig:LeftHand", lehtHandRadius);
    }
}