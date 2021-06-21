#include "EnemyManager.h"
#include "Collision.h"
#include <vector>
#include <algorithm>

// 更新処理
void EnemyManager::Update(float elapsedTime)
{
    for (Enemy* enemy : enemies)
    {
        enemy->Update(elapsedTime);
    }

    // 破棄処理
    for (Enemy* enemy : removes)
    {
        std::vector<Enemy*>::iterator it = std::find(enemies.begin(), enemies.end(), enemy);
        if (it != enemies.end())
        {
            enemies.erase(it);
        }
        // 敵削除
        delete enemy;
    }
    // 破棄リストをクリア
    removes.clear();

    // 敵同士の衝突処理
    CollisionEnemyVsEneies();
}

// 描画処理
void EnemyManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
    for (Enemy* enemy : enemies)
    {
        enemy->Render(context, shader);
    }
}
// デバッグプリミティブ描画
void EnemyManager::DrawDebugPrimitive()
{
    for (Enemy* enemy : enemies)
    {
        enemy->DrawDebugPrimitive();
    }
}

// エネミー登録
void EnemyManager::Register(Enemy* enemy)
{
    enemies.emplace_back(enemy);
}

void EnemyManager::Clear()
{
    for (Enemy* enemy : enemies)
    {
        delete enemy;
    }
}

void EnemyManager::Remove(Enemy* enemy)
{
    // 破棄リストに追加
    // emplace_backとpush_backの違い  
    // https://qiita.com/brackss1/items/e92da6458172397f7225
    removes.emplace_back(enemy);
}

// エネミー同士の衝突処理
void EnemyManager::CollisionEnemyVsEneies()
{
    // 全ての敵と総当たりで衝突処理
    // int enemyCount = GetEnemyCount();
    size_t enemyCount = enemies.size();
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy* enemy1 = enemies.at(i);
        for (int j = i + 1; j < enemyCount; ++j)
        {
            // if (i == j) continue;
            Enemy* enemy2 = enemies.at(j);
            // 衝突処理
            DirectX::XMFLOAT3  outPosition;
            if (Collision::IntersectCylinderVsCylinder(enemy1->GetPosition(), enemy1->GetRadius(), enemy1->GetHeight(), enemy2->GetPosition(), enemy2->GetRadius(), enemy2->GetHeight(), outPosition))
            {
                enemy2->SetPosition(outPosition);
            }
        }
    }
}
void EnemyManager::SortLengthSq(const DirectX::XMFLOAT3& playerPos)
{
    float l;
    // 各敵にプレイヤーとの距離の2乗を保存する
    for (auto& a: enemies)
    {
        l = powf((a->GetPosition().x - playerPos.x), 2) + powf((a->GetPosition().z - playerPos.z), 2);
        a->SetLengthSq(l);
    }
    // プレイヤーとの距離の２乗でソートする。 // ラムダ式活用 https://codezine.jp/article/detail/6020
    // beginとendの２つのイテレータ内でソートする
    //
    std::sort(enemies.begin(), enemies.end(), [](Enemy* x,Enemy* y)->int
        {return x->GetLengthSq() < y->GetLengthSq();});
}