#pragma once
#include <DirectXMath.h>
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>

// エフェクトマネージャー
class EffectManager
{
private:
    EffectManager() {}
    ~EffectManager() {}

public:
    // 唯一のインスタンス生成
    static EffectManager& Instance()
    {
        static EffectManager instance;
        return instance;
    }

    // 初期化
    void Initialize();

    // 終了化
    void Finalize();

    // 更新処理
    void Update(float elapsedTime);

    // 描画処理
    void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

    // Effeckseerマネージャーの取得
    Effekseer::Manager* GetEffekseerManager() { return effekseerManager; }

private:
    Effekseer::Manager* effekseerManager = nullptr;
    EffekseerRenderer::Renderer* effekseerRenderer = nullptr;
};