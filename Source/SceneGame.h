#pragma once

#include "world.h"
#include "Stage.h"
#include "Player.h"
#include "CameraController.h"
#include "EnemyManager.h"
#include "EnemySlime.h"
#include "EnemyGirl.h"
#include "Scene.h"
#include "Graphics/Sprite.h"

// �Q�[���V�[��
class SceneGame : public Scene
{
public:
	SceneGame() {}
	~SceneGame() override {}

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render() override;
private:
	Stage* stage = nullptr;

	// HACK �ǉ�
	World* world = nullptr;

	Player* player = nullptr;
	// EnemyManager* enemyManager = nullptr;
	// EnemySlime* enemySlime = nullptr;
	CameraController* cameraController = nullptr;
	Sprite* reticle = nullptr;
};
