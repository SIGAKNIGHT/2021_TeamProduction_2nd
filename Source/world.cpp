#include "World.h"

// コンストラクタ
World::World()
{
    // ステージモデルを読み込み
    model = new Model("Data/Model/world/earth.mdl");
}

World::~World()
{
    // ステージモデルを破棄
    delete model;
}

// 更新処理
void World::Update(float elapsedTime)
{
    // 今は特にやることはない
}

// 描画処理
void World::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    // シェーダーにモデルを描画してもらう
    shader->Draw(dc, model);
}