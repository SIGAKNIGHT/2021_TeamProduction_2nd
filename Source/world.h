#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"

// ステージ
class World
{
public:
    World();
    ~World();

    // 更新処理
    void Update(float elapsedTime);

    // 描画処理
    void Render(ID3D11DeviceContext* dc, Shader* shader);

private:
    Model* model = nullptr;
};