#include "World.h"

// �R���X�g���N�^
World::World()
{
    // �X�e�[�W���f����ǂݍ���
    model = new Model("Data/Model/world/earth.mdl");
}

World::~World()
{
    // �X�e�[�W���f����j��
    delete model;
}

// �X�V����
void World::Update(float elapsedTime)
{
    // ���͓��ɂ�邱�Ƃ͂Ȃ�
}

// �`�揈��
void World::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    // �V�F�[�_�[�Ƀ��f����`�悵�Ă��炤
    shader->Draw(dc, model);
}