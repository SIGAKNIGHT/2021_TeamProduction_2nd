#include "ProjectileHoming.h"


// HACK::ホーミング弾は敵がいなくなると敵の位置を持ってこれなくなるためバグる。
// ↑修正済み(敵の数を取得して、敵の数が０になればstraiteを打つように変更)
// コンストラクタ
ProjectileHoming::ProjectileHoming(ProjectileManager* manager) : Projectile(manager)
{
    model = new Model("Data/Model/Sword/Sword.mdl");
    // モデルが小さいのでスケーリング
    scale.x = scale.y = scale.z = 3.0f;
}

//デストラクタ
ProjectileHoming::~ProjectileHoming()
{
    delete model;
}

// 更新処理
void ProjectileHoming::Update(float elapsedTime)
{
    // 寿命処理
    lifeTimer -= elapsedTime;
    if (lifeTimer <= 0)
    {
        Destroy();
    }

    // 移動
    {
        float moveSpeed = this->moveSpeed * elapsedTime;
        position.x += direction.x * moveSpeed;
        position.y += direction.y * moveSpeed;
        position.z += direction.z * moveSpeed;
    }
    // 旋回
    {
        float turnSpeed = this->turnSpeed * elapsedTime;
        // ターゲットまでのベクトルを算出
        DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&position);
        DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&target);
        DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Target, Position);

        // ゼロベクトルでないなら回転処理
        DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
        float lengthSq;
        DirectX::XMStoreFloat(&lengthSq, LengthSq);
        if (lengthSq > 0.00001f)
        {
            // ターゲットまでのベクトルを単位ベクトル化
            Vec = DirectX::XMVector3Normalize(Vec);

            // 向いている方向ベクトルを算出
            DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&direction);

            // 前方向ベクトルとターゲットまでのベクトルの内積(角度)を算出
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Direction, Vec);
            float dot;
            DirectX::XMStoreFloat(&dot, Dot);

            // 2つの単位ベクトルの角度が小さいほど
            // 1.0に近づくという性質を利用して回転速度を調整する。 0~2までの値になる
            float rot = 1.0 - dot; //　補正値
            if (rot > turnSpeed)
            {
                rot = turnSpeed;
            }

            // 回転角度があるなら回転処理をする
            if (fabsf(rot) > 0.0001f)
            {
                // 回転軸を算出
                DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Direction, Vec);

                // 回転軸と回転量から回転行列を算出
                // XMMatrixRotationAxis : 任意の軸を中心に回転する行列を構築
                DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationAxis(Axis, rot);

                // 現在の行列を回転させる
                DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);
                //XMMatrixMultiply :  2つの行列の積を計算
                Transform = DirectX::XMMatrixMultiply(Transform, Rotation);

                // 回転後の前方方向を取り出し、単位ベクトル化する
                Direction = DirectX::XMVector3Normalize(Transform.r[2]);
                DirectX::XMStoreFloat3(&direction, Direction);
            }
        }
    }
    // オブジェクト行列を更新
    UpdateTransform();

    // モデル行列更新
    model->UpdateTransform(transform);
}

// 描画処理
void ProjectileHoming::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);
}

// 発射
void ProjectileHoming::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& target)
{
    this->direction = direction;
    this->position = position;
    this->target = target;

    UpdateTransform();
}
