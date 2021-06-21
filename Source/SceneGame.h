#pragma once

#include "Stage.h"
#include "Player.h"
#include "CameraController.h"
#include "EnemyManager.h"
#include "EnemySlime.h"
#include "Scene.h"

// ゲームシーン
class SceneGame : public Scene
{
public:
	SceneGame() {}
	~SceneGame() override {}

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;
private:
	Stage* stage = nullptr;
	Player* player = nullptr;
	// EnemyManager* enemyManager = nullptr;
	// EnemySlime* enemySlime = nullptr;
	CameraController* cameraController = nullptr;
};
