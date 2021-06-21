#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"

// �X�e�[�W
class Stage
{
public:
    Stage();
    ~Stage();

    // �X�V����
    void Update(float elapsedTime);

    // �`�揈��
    void Render(ID3D11DeviceContext* dc, Shader* shader);

private:
    Model* model = nullptr;
};