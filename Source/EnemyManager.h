#pragma once

#include <vector>
#include "Enemy.h"

// エネミーマネージャー
class EnemyManager
{
private:
    EnemyManager() {}
    ~EnemyManager() {}
    void CollisionEnemyVsEneies();

public:
    // 唯一のインスタンス取得
    static EnemyManager& Instance()
    {
        static EnemyManager instance;
        return instance;
    }
    // 更新処理
    void Update(float elapsedTime);
    // 描画処理
    void Render(ID3D11DeviceContext* dc, Shader* shader);
    // デバッグプリミティブ描画
    void DrawDebugPrimitive();
    // エネミー登録
    void Register(Enemy* enemy);
    // エネミー数取得
    int GetEnemyCount() const { return static_cast<int>(enemies.size()); }
    // エネミー取得
   //  ゆにぽ　Enemy* GetEnemy(int index) { return enemies.at(index).get(); }
    Enemy* GetEnemy(int index) { return enemies.at(index); }
    // エネミー全削除
    void Clear();   
    // エネミー削除
    void Remove(Enemy* enemy);

    // エネミーソート
    void SortLengthSq(const DirectX::XMFLOAT3& playerPos);
private:
    //　ゆにぽ　std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<Enemy*> enemies;
    std::vector<Enemy*> removes;
};