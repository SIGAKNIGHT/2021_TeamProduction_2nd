#pragma once

#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <imgui.h>

class ImGuiRenderer
{
public:
	ImGuiRenderer(HWND hWnd, ID3D11Device* device);
	~ImGuiRenderer();

public:
	// フレーム開始処理
	void NewFrame();

	// 描画実行
	void Render(ID3D11DeviceContext* context);

	// WIN32メッセージハンドラー
	LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	// マウスカーソル更新
	bool UpdateMouseCursor();

	// マウス座標更新
	void UpdateMousePos();

private:
	struct ConstantBuffer
	{
		DirectX::XMFLOAT4X4		wvp;
	};

	HWND												hWnd;
	INT64												time = 0;
	INT64												ticksPerSecond = 0;
	ImGuiMouseCursor									lastMouseCursor = ImGuiMouseCursor_COUNT;

	Microsoft::WRL::ComPtr<ID3D11Buffer>				vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				constantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>			blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceView;

	int													vertexCount = 0;
	int													indexCount = 0;
};
