#pragma once

#include "Graphics/Model.h"
#include "Enemy.h"

// �X���C��
class EnemyGirl : public Enemy
{
public:
    EnemyGirl();
    ~EnemyGirl() override;

    // �X�V����
    void Update(float elapsedTime) override;

    // �X�V����
    void Render(ID3D11DeviceContext* dc, Shader* shader)override;

    // �f�o�b�O�v���~�e�B�u�`��
   //  void DrawDebugPrimitive() override;

    // // �꒣��ݒ�
    // void SetTerritory(const DirectX::XMFLOAT3& origin, float range);

private:
    // // �^�[�Q�b�g�ʒu�������_���ݒ�
    // void SetRandomTargetposition();
    // 
    // // �ڕW�n�_�ֈړ�
    // void MoveToTarget(float elapsedTime, float speedRate);
    // 
    // // �o�~�X�e�[�g�֑J��
    // void TransitionWanderState();
    // // �p�j�X�e�[�g�X�V����
    // void UpdateWanderState(float elapsedTime);
        // �m�[�h�ƃv���C���[�̏Փˏ���
    void SetHeadPos(const char* nodeName, float boneRadius);

protected:
    //  ���S�������ɌĂ΂��
    void OnDead() override;

private:
    Model* model = nullptr;
    // �X�e�[�g
    enum class State
    {
        Wander
    };

    // �A�j���[�V����
    enum Animation
    {

    };
};