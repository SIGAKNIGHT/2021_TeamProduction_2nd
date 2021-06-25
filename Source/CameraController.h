#pragma once
#include <DirectXMath.h>

// �J�����R���g���[���[
class CameraController
{
public:
    // �X�V����
    void Update(float elapsedTime);

    // �^�[�Q�b�g�ʒu�ݒ�
    void SetTarget(const DirectX::XMFLOAT3& target) { this->target = target;}
    void DrawCameraDebugGUI();
    void DrawCameraViewDebugGUI();
    void DrawTestDebugGUI();
    float clamp(float input, float min, float max)
    {
        if (input < min)
        {
            return min;
        }
        else if (input > max)
        {
            return max;
        }
        else
        {
            return input;
        }
    }

    // �A���O���Q�b�^�[
    const DirectX::XMFLOAT3& GetAngle() const { return this->angle; }

private:
    DirectX::XMMATRIX Transform;
    DirectX::XMVECTOR Front;
    DirectX::XMFLOAT3 front;
    DirectX::XMVECTOR Right;
    DirectX::XMFLOAT3 right;
    DirectX::XMFLOAT3 target = { 0,0,0 };
    DirectX::XMFLOAT3 angle = { 0,0,0 };
    float rollSpeed = DirectX::XMConvertToRadians(10);
    float range = 10.0f;
    //float Third_Range = 10.0f;
    float maxAngleX = DirectX::XMConvertToRadians(45);
    float minAngleX = DirectX::XMConvertToRadians(-45);
    DirectX::XMFLOAT3 eye;
    enum {
        FirstPerson_View,
        ThirdPerson_View,
        Side_View,
    };
    int Camera_View = FirstPerson_View;
};