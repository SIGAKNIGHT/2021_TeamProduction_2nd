#pragma once

#include <DirectXMath.h>

// カメラ
class Camera
{
private:
    Camera() {}
    ~Camera(){}
public:
    // 唯一のインスタンス取得
    static Camera& Instance()
    {
        static Camera camera;
        return camera;
    }
    // 指定方向を向く
    void SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up);

    // パースペクティブ設定
    void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

    // ビュー行列取得
    const DirectX::XMFLOAT4X4& GetView() const { return view; }

    // プロジェクション行列取得
    const DirectX::XMFLOAT4X4& GetProjection() const { return projection; }
    // 視点取得
    const DirectX::XMFLOAT3& GetEye() const { return eye; }
    // 注視点取得
    const DirectX::XMFLOAT3& GetFocus() const { return focus; }
    // 上方向取得
    const DirectX::XMFLOAT3& GetUp() const { return up; }
    // 前方向取得
    const DirectX::XMFLOAT3& GetFront() const { return front; }
    // 右方向取得
    const DirectX::XMFLOAT3& GetRight() const { return right; }

private:
    DirectX::XMFLOAT4X4 view;
    DirectX::XMFLOAT4X4 projection;
    DirectX::XMFLOAT3 eye;
    DirectX::XMFLOAT3 focus;
    DirectX::XMFLOAT3 up;
    DirectX::XMFLOAT3 front;
    DirectX::XMFLOAT3 right;
};

// class Camera2
// {
// private:
//     Camera2() {}
//     ~Camera2() {}
// public:
//     static Camera2& Instance()
//     {
//         static Camera2 camera2;
//         return camera2;
//     }
//     void setLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up);
//     void setperspectiveFov(float fovY, float aspect, float newrZ, float farZ);
//     const DirectX::XMFLOAT4X4& getView() const { return view };
//     const DirectX::XMFLOAT4X4& getProjection() const { return projection };
// private:
//     DirectX::XMFLOAT4X4 view;
//     DirectX::XMFLOAT4X4 projection;
// };