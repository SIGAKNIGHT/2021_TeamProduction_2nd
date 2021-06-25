#include "Camera.h"

// 指定方向を向く
void Camera::SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{
    // 視点、注視点、上方向空のビュー行列を作成
    DirectX::XMVECTOR Eye   = DirectX::XMLoadFloat3(&eye);      // 視点
    DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);    // 注視点
    DirectX::XMVECTOR Up    = DirectX::XMLoadFloat3(&up);       // 上方向
    DirectX::XMMATRIX View  = DirectX::XMMatrixLookAtLH(Eye, Focus, Up); // XMMatrixLookAtLH LHはLeftHand
    DirectX::XMStoreFloat4x4(&view, View); // XMFLOATにXMVECTORの値を変換
    // ビューを逆行列化し、ワールド行列に戻す
    DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
    DirectX::XMFLOAT4X4 world;
    DirectX::XMStoreFloat4x4(&world, World);

    // カメラの方向を取り出す
    this->right.x = world._11;
    this->right.y = world._12;
    this->right.z = world._13;

    this->up.x = world._21;
    this->up.y = world._22;
    this->up.z = world._23;

    this->front.x = world._31;
    this->front.y = world._32;
    this->front.z = world._33;

    // 視点、注視点を保存
    this->eye = eye;
    this->focus = focus;
}

// パースペクティブ設定
void Camera::SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
    // 画角、画面比率、クリップ距離からプロジェクション行列を作成
    DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
    DirectX::XMStoreFloat4x4(&projection, Projection);
}


// void Camera2::setLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
// {
//     DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
//     DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
//     DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
//     DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
//     DirectX::XMStoreFloat4x4(&view, View);
// }
// 
// void Camera2::setperspectiveFov(float fovY, float aspect, float nearZ, float farZ)
// {
//     DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
//     DirectX::XMStoreFloat4x4(&projection, Projection);
// }