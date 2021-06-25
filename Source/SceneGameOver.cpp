#include "Graphics/Graphics.h"
#include "SceneGameOver.h"
#include "SceneGame.h"
#include "SceneLoading.h"
#include "SceneManager.h"
#include "Input/Input.h"

// 初期化
void SceneGameOver::Initialize() {
    // スプライト初期化
    sprite = new Sprite("Data/Sprite/GameOver.png");
}

// 終了化
void SceneGameOver::Finalize() {
    // スプライト終了化
    if (sprite != nullptr) {
        delete sprite;
        sprite = nullptr;
    }
}

// 更新処理
void SceneGameOver::Update(float elapsedTime) {

    GamePad& gamePad = Input::Instance().GetGamePad();

    // なにかボタンを押したらローディングシーンを挟んでゲームシーンへ切り替え
    const GamePadButton anyButton =
        GamePad::BTN_A | GamePad::BTN_B | GamePad::BTN_X | GamePad::BTN_Y; //|
        // GamePad::BTN_BACK | GamePad::BTN_DOWN | GamePad::BTN_LEFT | GamePad::BTN_LEFT_SHOULDER |
        // GamePad::BTN_LEFT_THUMB | GamePad::BTN_LEFT_TRIGGER | GamePad::BTN_RIGHT | GamePad::BTN_RIGHT_SHOULDER |
        // GamePad::BTN_RIGHT_THUMB | GamePad::BTN_RIGHT_TRIGGER | GamePad::BTN_START | GamePad::BTN_UP;
    if (gamePad.GetButton() & anyButton) {
        SceneManager::Instance().ChangeScene(new SceneGameOver)
    }
}

// 描画処理
void SceneGameOver::Render() {
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

    // 画面クリア&レンダーターゲットビュー設定
    FLOAT color[] = { 0.0f,0.0f,0.5f,1.0f };
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    // 2Dスプライト
    {
        float screenWidth = static_cast<float>(graphics.GetScreenWidth());
        float screenHeight = static_cast<float>(graphics.GetScreenHeight());
        float textureWidth = static_cast<float>(sprite->GetTextureWidth());
        float textureHeight = static_cast<float>(sprite->GetTextureHeight());
        // タイトルスプライト描画処理
        sprite->Render(dc, 0, 0, screenWidth, screenHeight, 0, 0, textureWidth, textureHeight, 0, 1, 1, 1, 1);
    }

}