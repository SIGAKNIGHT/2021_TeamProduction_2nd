#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

// �Q�[���I�[�o�[�V�[��
class SceneGameOver : public Scene
{
public:
    SceneGameOver() {}
    ~SceneGameOver() override{}

    // ������
    void Initialize() override;

    // �I����
    void Finalize() override;

    // �X�V����
    void Update(float elapsedTime) override;

    // �`�揈��
    void Render() override;

private:
    Sprite* sprite = nullptr;

};