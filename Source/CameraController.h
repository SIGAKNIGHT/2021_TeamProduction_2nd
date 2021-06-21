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
    void DrawDebugGUI();
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
    DirectX::XMFLOAT3 target = { 0,0,0 };
    DirectX::XMFLOAT3 angle = { 0,0,0 };
    float rollSpeed = DirectX::XMConvertToRadians(10);
    float range = 10.0f;
    float maxAngleX = DirectX::XMConvertToRadians(45);
    float minAngleX = DirectX::XMConvertToRadians(-45);
};