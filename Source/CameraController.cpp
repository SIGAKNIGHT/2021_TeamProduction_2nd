#include <imgui.h>
#include "CameraController.h"
#include "Camera.h"
#include "Input/Input.h"
#include "common.h"

// �X�V����
void CameraController::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();
    // float ax = gamePad.GetAxisRX();
    // float ay = gamePad.GetAxisRY();
    // float ax = mouse.GetPositionX();
    // float ay = mouse.GetPositionY();
    // �J�����̉�]���x
    float speed = rollSpeed * elapsedTime;
    // �X�e�B�b�N�̓��͒l�ɍ��킹��X����Y������]
    this->angle.x = (mouse.GetPositionY() - mouse.GetScreenHeight() * 0.5f)* speed;
    this->angle.y = (mouse.GetPositionX() - mouse.GetScreenWidth() * 0.5f) * speed;

    
    if (this->angle.x < minAngleX)
    {
        this->angle.x = minAngleX;
    }
    else if (this->angle.x > maxAngleX)
    {
        this->angle.x = maxAngleX;
    }
    // Y���̉�]�l��-3.14~3.14�Ɏ��܂�悤�ɂ���
    if (angle.y < -DirectX::XM_PI)
    {
        angle.y += DirectX::XM_2PI;
    }
    if (angle.y > DirectX::XM_PI)
    {
        angle.y -= DirectX::XM_2PI;
    }

    // �J������]�l����]�s��ɕϊ�
    DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

    // ��]�s�񂩂�O�����x�N�g�������o��
    DirectX::XMVECTOR Front = Transform.r[2];
    DirectX::XMFLOAT3 front;
    DirectX::XMStoreFloat3(&front, Front);

    // �����_������x�N�g�������Ɉ�苗�����ꂽ�J�������_�����߂�
    DirectX::XMFLOAT3 eye;
    eye.x = target.x + (front.x * range);//(cos(angle.x) - sin(angle.x)) * range;// (range - target.x) * cos(angle.x) - (double)(range - target.y) * sin(angle.y);
    eye.y = target.y + (front.y * range);//(sin(angle.y) - cos(angle.y)) * range; //  (range - target.x)* sin(angle.x) + (double)(range - target.y) * cos(angle.y);
    eye.z = target.z + (front.z * range);
    
    // �J�����̎��_�ƒ����_��ݒ�
    Camera::Instance().SetLookAt(target, eye, DirectX::XMFLOAT3(0, 1, 0));
}

void CameraController::DrawDebugGUI()
{
    ImGui::SetNextWindowPos(ImVec2(10, 320), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(10, 320), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Camera", nullptr, ImGuiWindowFlags_None))
    {
        // �g�����X�t�H�[��
        if (ImGui::CollapsingHeader("Angle", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // ��]
            DirectX::XMFLOAT3 a;
            a.x = DirectX::XMConvertToDegrees(angle.x);
            a.y = DirectX::XMConvertToDegrees(angle.y);
            a.z = DirectX::XMConvertToDegrees(angle.z);
            ImGui::InputFloat3("Angle", &a.x);
            angle.x = DirectX::XMConvertToRadians(a.x);
            angle.y = DirectX::XMConvertToRadians(a.y);
            angle.z = DirectX::XMConvertToRadians(a.z);
        }
    }
    ImGui::End();
}