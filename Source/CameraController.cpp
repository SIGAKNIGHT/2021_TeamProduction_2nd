#include <imgui.h>
#include "CameraController.h"
#include "Camera.h"
#include "Input/Input.h"
#include "common.h"
#include <vector>

extern const LONG SCREEN_WIDTH;
extern const LONG SCREEN_HEIGHT;
extern const INT SCREEN_WIDTH_I;
extern const INT SCREEN_HEIGHT_I;

// 更新処理
void CameraController::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();
    // float ax = gamePad.GetAxisRX();
    // float ay = gamePad.GetAxisRY();
    // float ax = mouse.GetPositionX();
    // float ay = mouse.GetPositionY();
    // カメラの回転速度
    float speed = rollSpeed * elapsedTime;

    switch (Camera_View)
    {
    case FirstPerson_View:
        // スティックの入力値に合わせてX軸とY軸を回転
        this->angle.x = (mouse.GetPositionY() - mouse.GetScreenHeight() * 0.5f) * speed;
        this->angle.y = (mouse.GetPositionX() - mouse.GetScreenWidth() * 0.5f) * speed;

        if (this->angle.x < minAngleX)
        {
            this->angle.x = minAngleX;
        }
        else if (this->angle.x > maxAngleX)
        {
            this->angle.x = maxAngleX;
        }
        // Y軸の回転値を-3.14~3.14に収まるようにする
        if (angle.y < -DirectX::XM_PI)
        {
            angle.y += DirectX::XM_2PI;
        }
        if (angle.y > DirectX::XM_PI)
        {
            angle.y -= DirectX::XM_2PI;
        }

        // カメラ回転値を回転行列に変換
        Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

        // 回転行列から前方向ベクトルを取り出す
        Front = Transform.r[2];
        DirectX::XMStoreFloat3(&front, Front);

        // 注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
        //DirectX::XMFLOAT3 eye;
        eye.x = target.x + (front.x * range);//(cos(angle.x) - sin(angle.x)) * range;// (range - target.x) * cos(angle.x) - (double)(range - target.y) * sin(angle.y);
        eye.y = target.y + (front.y * range);//(sin(angle.y) - cos(angle.y)) * range; //  (range - target.x)* sin(angle.x) + (double)(range - target.y) * cos(angle.y);
        eye.z = target.z + (front.z * range);

        // カメラの視点と注視点を設定
        Camera::Instance().SetLookAt(target, eye, DirectX::XMFLOAT3(0, 1, 0));
        break;

    case ThirdPerson_View:
        // スティックの入力値に合わせてX軸とY軸を回転
        this->angle.x = (mouse.GetPositionY() - mouse.GetScreenHeight() * 0.5f) * speed;
        this->angle.y = (mouse.GetPositionX() - mouse.GetScreenWidth() * 0.5f) * speed;

        if (this->angle.x < minAngleX)
        {
            this->angle.x = minAngleX;
        }
        else if (this->angle.x > maxAngleX)
        {
            this->angle.x = maxAngleX;
        }
        // Y軸の回転値を-3.14~3.14に収まるようにする
        if (angle.y < -DirectX::XM_PI)
        {
            angle.y += DirectX::XM_2PI;
        }
        if (angle.y > DirectX::XM_PI)
        {
            angle.y -= DirectX::XM_2PI;
        }

        // カメラ回転値を回転行列に変換
        Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

        // 回転行列から前方向ベクトルを取り出す
        Front = Transform.r[2];
        DirectX::XMStoreFloat3(&front, Front);

        // 注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
        eye.x = target.x - (front.x * range);
        eye.y = target.y - (front.y * range);
        eye.z = target.z - (front.z * range);

        // カメラの視点と注視点を設定
        Camera::Instance().SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));
        break;

    case Side_View:
        this->angle.x = DirectX::XMConvertToRadians(0.0f);
        this->angle.y = (mouse.GetPositionX() - mouse.GetScreenWidth() * 0.5f) * speed;
        // Y軸の回転値を-3.14~3.14に収まるようにする
        if (angle.y < -DirectX::XM_PI)
        {
            angle.y += DirectX::XM_2PI;
        }
        if (angle.y > DirectX::XM_PI)
        {
            angle.y -= DirectX::XM_2PI;
        }

        // カメラ回転値を回転行列に変換
        Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

        // 回転行列から前方向ベクトルを取り出す
        Right = Transform.r[0];
        DirectX::XMStoreFloat3(&right, Right);

        // 注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
        eye.x = target.x - (right.x * range);
        eye.y = target.y - (right.y * range) + 3.0f;
        eye.z = target.z - (right.z * range);

        // カメラの視点と注視点を設定
        Camera::Instance().SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));
        break;
    }
}

void CameraController::DrawCameraViewDebugGUI()
{
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.3f, 0.0f, 0.0f, 1.0f));
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(1280, 65), ImGuiCond_Once);
    if (ImGui::Begin("Camera_View", nullptr, ImGuiWindowFlags_None))
    {        
        ImGui::RadioButton("FirstPerson_View", &Camera_View, FirstPerson_View);
        ImGui::SameLine();
        ImGui::RadioButton("ThirdPerson_View", &Camera_View, ThirdPerson_View);
        ImGui::SameLine();
        ImGui::RadioButton("Side_View", &Camera_View, Side_View);
    }
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
}

void CameraController::DrawCameraDebugGUI()
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    ImGui::SetNextWindowPos(ImVec2(0, 365), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Once);

    if (ImGui::Begin("Camera", nullptr, ImGuiWindowFlags_None))
    {
        // トランスフォーム
        if (ImGui::CollapsingHeader("transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // カメラ距離
            ImGui::SliderFloat("range", &range, 10.0f, 30.0f);
            
            // 回転
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////
}

void CameraController::DrawTestDebugGUI()
{
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.7f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.0f, 0.3f, 0.1f, 1.0f));
    //FLOAT WindowPos_Temp = static_cast<float>(SCREEN_WIDTH_I - 300);
    ImGui::SetNextWindowPos(ImVec2(980, 65), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(300, 720), ImGuiCond_Once);
    if (ImGui::Begin("ImGui_Test", nullptr, ImGuiWindowFlags_MenuBar))
    {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Save")) {

                }
                if (ImGui::MenuItem("Load")) {

                }

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        static float slider1 = 0.0;
        static float slider2 = 0.0;
        static char text1[8] = "";
        static char text2[8] = "";

        ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("TreeNode 1")) {

            //ImGui::Text("fps: %.2f", getFrameRate());
            ImGui::SliderFloat("SliderFloat 1", &slider1, 0.0f, 1.0f);
            ImGui::InputText("InputText 1", text1, sizeof(text1));
            if (ImGui::Button("Button 1")) {
                slider1 = 0.0f;
                //strcpy_s(text1, "Button 1");
            }

            ImGui::TreePop();
        }

        ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("TreeNode 2")) {

            //ImGui::Text("fps: %.2f", getFrameRate());
            ImGui::SliderFloat("SliderFloat 2", &slider2, 0.0f, 1.0f);
            ImGui::InputText("InputText 2", text2, sizeof(text2));
            if (ImGui::Button("Button 2")) {
                slider2 = 0.0f;
                //strcpy_s(text2, "Button 2");
            }

            //if (ImGui::TreeNode("group 3")) {
            //    if (ImGui::Button("button 2")) {

            //    }
            //    ImGui::TreePop();

            //}

            ImGui::TreePop();
        }

        static std::vector<float> items(10);

        if (ImGui::Button("add")) {
            items.push_back(0.0f);
        }
        if (ImGui::Button("remove")) {
            if (items.empty() == false) {
                items.pop_back();
            }
        }

        ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 100), ImGuiWindowFlags_NoTitleBar);
        for (int i = 0; i < items.size(); ++i) {
            char name[16];
            sprintf_s(name, "item %d", i);
            ImGui::SliderFloat(name, &items[i], 0.0f, 10.0f);
        }
        ImGui::EndChild();

        static bool isEnable = false;
        static int mode = 0;

        enum {
            MODE_1,
            MODE_2,
        };

        ImGui::Checkbox("Checkbox", &isEnable);

        ImGui::RadioButton("RadioButton1", &mode, MODE_1);
        ImGui::SameLine();
        ImGui::RadioButton("RadioButton2", &mode, MODE_2);

        ImGui::SetNextTreeNodeOpen(true, ImGuiCond_FirstUseEver);
        if (ImGui::TreeNode("TreeNode")) {
            if (mode == MODE_1) {
                ImGui::Text("Text1");
            }
            else if (mode == MODE_2) {
                ImGui::Text("Text2");
            }

            ImGui::TreePop();
        }
    }
    ImGui::End();

    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
}