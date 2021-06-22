#include "EnemyGirl.h"
#include "Graphics/Graphics.h"
#include "Mathf.h"
#include "Player.h"
#include "Collision.h"

// �R���X�g���N�^
EnemyGirl::EnemyGirl()
{
    model = new Model("Data/Model/Body/girl.mdl");
    model->PlayAnimation(0, true, 0.1);
    angle.y = DirectX::XM_PI;
    

    // ���f�����傫���̂ŃX�P�[�����O
    scale.x = scale.y = scale.z = 0.01f;

    // �������ݒ�
    radius = 0.5f;
    height = 1.0f;
}

// �f�X�g���N�^
EnemyGirl::~EnemyGirl()
{
    delete model;
}

// �X�V����
void EnemyGirl::Update(float elapsedTime)
{
    // ���͏����X�V
    UpdateVelocity(elapsedTime);
    // ���G���ԍX�V
    UpdateInvincibleTimer(elapsedTime);
    SetHeadPos("head", headRadius);
    // �I�u�W�F�N�g�s����X�V
    UpdateTransform();
    // ���f���s��X�V
    model->UpdateAnimation(elapsedTime);
    model->UpdateTransform(transform);
}

// ���S�������ɌĂ΂��
void EnemyGirl::OnDead()
{
    Destroy();
}

// �`�揈��
void EnemyGirl::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);
}

void EnemyGirl::SetHeadPos(const char* nodeName, float nodeRadius)
{
    // �m�[�h�̈ʒu�Ɠ����蔻����s��
    Model::Node* node = model->FindNode(nodeName);
    if (node != nullptr)
    {
        // �m�[�h�̃��[���h���W
        DirectX::XMFLOAT3 nodePosition(
            node->worldTransform._41,
            node->worldTransform._42,
            node->worldTransform._43
        );
        headPos = nodePosition;
        // �����蔻��\��
        Graphics::Instance().GetDebugRenderer()->DrawSphere(
            nodePosition, nodeRadius, DirectX::XMFLOAT4(0, 0, 1, 1)
        );
    }
}