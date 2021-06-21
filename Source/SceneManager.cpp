#include "SceneManager.h"

// 更新処理
void SceneManager::Update(float elapsedTime)
{
    if (currentScene != nullptr)
    {
        currentScene->Update(elapsedTime);
    }
}

// 描画処理
void SceneManager::Render()
{
    if (currentScene != nullptr)
    {
        currentScene->Render();
    }
}

// シーンクリア
void SceneManager::Clear()
{
    if (currentScene != nullptr)
    {
        currentScene->Finalize();
        delete currentScene;
        currentScene = nullptr;
    }
}

void SceneManager::ChangeScene(Scene* scene)
{
    // 古いシーンを終了処理
    Clear();
    // 新しいシーンを設定
    currentScene = scene;
    // 未初期化の場合は初期化処理
    if (!currentScene->IsReady())
    {
      currentScene->Initialize();
    }
}