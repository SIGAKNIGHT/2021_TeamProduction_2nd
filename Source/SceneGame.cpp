#include "Graphics/Graphics.h"
#include "SceneGame.h"
#include "Camera.h"
#include "EffectManager.h"

// ������
void SceneGame::Initialize()
{
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
}

// �I����
void SceneGame::Finalize()
{
	// �J�����R���g���[���[�̏I����
	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
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
														
    // // �r���[�s��
	// DirectX::XMFLOAT3 eye = { 0,10,-10 };
	// DirectX::XMFLOAT3 focus = { 0,0,0 };
	// DirectX::XMFLOAT3 up = { 0,1,0 };
	// 
	// DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
	// DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
	// DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
	// DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	// DirectX::XMStoreFloat4x4(&rc.view, View);
	// // �v���W�F�N�V�����s��
	// // constexpr
	// float fovY = DirectX::XMConvertToRadians(45);
	// float aspectRatio = graphics.GetScreenWidth() / graphics.GetScreenHeight();
	// float nearZ = 0.1f;
	// float farZ = 1000.0f;
	// DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspectRatio, nearZ, farZ);
	// DirectX::XMStoreFloat4x4(&rc.projection, Projection);


	// �J�����p�����[�^�ݒ�
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	// 3D���f���`��
	{
		Shader* shader = graphics.GetShader();
		shader->Begin(dc, rc);
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

	// 3D�f�o�b�O�`��
	{
		player->DrawDebugPrimitive();
		EnemyManager::Instance().DrawDebugPrimitive();
		// ���C�������_���`����s
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// �f�o�b�O�����_���`����s
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}

	// 2D�X�v���C�g�`��
	{

	}

	// 2D�f�o�b�OGUI�`��
	{
		player->DrawDebugGUI();
		cameraController->DrawDebugGUI();
	}
}
