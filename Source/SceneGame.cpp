#include "Graphics/Graphics.h"
#include "SceneGame.h"
#include "Camera.h"
#include "EffectManager.h"

// 初期化
void SceneGame::Initialize()
{
	// ステージの初期化
	stage = new Stage();
	// プレイヤーの初期化
	player = new Player();
	// プレイヤーの初期化
	// enemySlime = new EnemySlime();
	// EnemyManager::Instance().Register(enemySlime);
	EnemyManager& enemyManager = EnemyManager::Instance();
	for (int i = 0; i < 2; ++i)
	{
		EnemySlime* slime = new EnemySlime();
		slime->SetPosition(DirectX::XMFLOAT3(i * 3.0f, 0, 5));
		enemyManager.Register(slime);
	}

	// カメラの初期設定
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
	// カメラコントローラーの初期化
	cameraController = new CameraController();
}

// 終了化
void SceneGame::Finalize()
{
	// カメラコントローラーの終了化
	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}
	// ステージの終了化
	if (stage != nullptr)
	{
		delete stage;
		stage = nullptr;
	}
	// プレイヤーの終了化
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}
	EnemyManager::Instance().Clear();
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	// カメラコントローラー更新処理
	DirectX::XMFLOAT3 target = player->GetPosition();
	target.y += 1.0f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);
	// ステージの更新処理
	stage->Update(elapsedTime);
	// エネミー更新処理
	EnemyManager::Instance().Update(elapsedTime);

	EnemyManager::Instance().SortLengthSq(player->GetPosition());
	// プレイヤー更新処理
	player->SetAngle(cameraController->GetAngle());
	player->Update(elapsedTime);

	// エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);
}

// 描画処理
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// 描画処理
	RenderContext rc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };	// ライト方向（下方向）
														
    // // ビュー行列
	// DirectX::XMFLOAT3 eye = { 0,10,-10 };
	// DirectX::XMFLOAT3 focus = { 0,0,0 };
	// DirectX::XMFLOAT3 up = { 0,1,0 };
	// 
	// DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
	// DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
	// DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
	// DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	// DirectX::XMStoreFloat4x4(&rc.view, View);
	// // プロジェクション行列
	// // constexpr
	// float fovY = DirectX::XMConvertToRadians(45);
	// float aspectRatio = graphics.GetScreenWidth() / graphics.GetScreenHeight();
	// float nearZ = 0.1f;
	// float farZ = 1000.0f;
	// DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspectRatio, nearZ, farZ);
	// DirectX::XMStoreFloat4x4(&rc.projection, Projection);


	// カメラパラメータ設定
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	// 3Dモデル描画
	{
		Shader* shader = graphics.GetShader();
		shader->Begin(dc, rc);
		// ステージ描画
		stage->Render(dc, shader);
		// プレイヤー描画
		player->Render(dc, shader);
		EnemyManager::Instance().Render(dc, shader);
	}

	// 3Dエフェクト描画
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// 3Dデバッグ描画
	{
		player->DrawDebugPrimitive();
		EnemyManager::Instance().DrawDebugPrimitive();
		// ラインレンダラ描画実行
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// デバッグレンダラ描画実行
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}

	// 2Dスプライト描画
	{

	}

	// 2DデバッグGUI描画
	{
		player->DrawDebugGUI();
		cameraController->DrawDebugGUI();
	}
}
