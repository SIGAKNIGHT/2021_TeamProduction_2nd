#include "Graphics/Graphics.h"
#include "SceneGame.h"
#include "Camera.h"
#include "EffectManager.h"


// ������
void SceneGame::Initialize()
{
	// HACK
	world = new World();
	// �X�e�[�W�̏�����
	stage = new Stage();
	// �v���C���[�̏�����
	player = new Player();
	// �v���C���[�̏�����
	// enemySlime = new EnemySlime();
	// EnemyManager::Instance().Register(enemySlime);
	EnemyManager& enemyManager = EnemyManager::Instance();
	for (int i = 0; i < 2; ++i)
	{
		EnemySlime* slime = new EnemySlime();
		slime->SetPosition(DirectX::XMFLOAT3(i * 3.0f, 0, 5));
		enemyManager.Register(slime);
	}

	for (int i = 0; i < 5; ++i)
	{
		EnemyGirl* girl = new EnemyGirl();
		girl->SetPosition(DirectX::XMFLOAT3(i * 2.0f - 3, 0, 10));
		enemyManager.Register(girl);
	}

	// �J�����̏����ݒ�
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0)
	);
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),
		0.1f,
		1000.0f
	);
	// �J�����R���g���[���[�̏�����
	cameraController = new CameraController();
	// �X�v���C�g������
	reticle = new Sprite("Data/Sprite/reticle.png");
}

// �I����
void SceneGame::Finalize()
{
	// ���e�B�N���폜
	if (reticle != nullptr)
	{
		delete reticle;
		reticle = nullptr;
	}
	// �J�����R���g���[���[�̏I����
	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}
	// HACK
	if (world != nullptr)
	{
		delete world;
		world = nullptr;
	}

	// �X�e�[�W�̏I����
	if (stage != nullptr)
	{
		delete stage;
		stage = nullptr;
	}
	// �v���C���[�̏I����
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}
	EnemyManager::Instance().Clear();
}

// �X�V����
void SceneGame::Update(float elapsedTime)
{
	

	// �J�����R���g���[���[�X�V����
	DirectX::XMFLOAT3 target = player->GetPosition();
	target.y += 1.0f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);
	// HaCK
	world->Update(elapsedTime);
	// �X�e�[�W�̍X�V����
	stage->Update(elapsedTime);
	// �G�l�~�[�X�V����
	EnemyManager::Instance().Update(elapsedTime);

	EnemyManager::Instance().SortLengthSq(player->GetPosition());
	// �v���C���[�X�V����
	player->SetAngle(cameraController->GetAngle());
	player->Update(elapsedTime);

	// �G�t�F�N�g�X�V����
	EffectManager::Instance().Update(elapsedTime);
	
}

// �`�揈��
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0�`1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// �`�揈��
	RenderContext rc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };	// ���C�g�����i�������j

	// �J�����p�����[�^�ݒ�
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	// 3D���f���`��
	{
		Shader* shader = graphics.GetShader();
		shader->Begin(dc, rc);
		// HACK
		world->Render(dc, shader);
		// �X�e�[�W�`��
		stage->Render(dc, shader);
		// �v���C���[�`��
		player->Render(dc, shader);
		EnemyManager::Instance().Render(dc, shader);
	}

	// 3D�G�t�F�N�g�`��
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// 2D�X�v���C�g�`��
	{
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		float textureWidth = static_cast<float>(reticle->GetTextureWidth());
		float textureHeight = static_cast<float>(reticle->GetTextureHeight());
		float posX = (screenWidth * 0.5f) - (textureWidth * 0.5f);
		float posY = (screenHeight * 0.5f) - (textureHeight * 0.5f);
		// �^�C�g���X�v���C�g�`��
		reticle->Render(dc, posX, posY, textureWidth, textureHeight, 0, 0, textureWidth, textureHeight, 0, 1, 1, 1, 1);
		
	}


	// 3D�f�o�b�O�`��
	{
		player->DrawDebugPrimitive();
		EnemyManager::Instance().DrawDebugPrimitive();
		// ���C�������_���`����s
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// �f�o�b�O�����_���`����s
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}

	// 2D�f�o�b�OGUI�`��
	{
		player->DrawDebugGUI();
		cameraController->DrawDebugGUI();
	}
}
