#include "Stage.h"

// �R���X�g���N�^
Stage::Stage()
{
    // �X�e�[�W���f����ǂݍ���
    model = new Model("Data/Model/ExampleStage/ExampleStage.mdl");
}

Stage::~Stage()
{
    // �X�e�[�W���f����j��
    delete model;
}

// �X�V����
void Stage::Update(float elapsedTime)
{
    // ���͓��ɂ�邱�Ƃ͂Ȃ�
}

// �`�揈��
void Stage::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    // �V�F�[�_�[�Ƀ��f����`�悵�Ă��炤
    shader->Draw(dc, model);
}