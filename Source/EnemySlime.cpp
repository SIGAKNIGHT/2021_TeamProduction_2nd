#include "EnemySlime.h"
#include "Graphics/Graphics.h"
#include "Mathf.h"
#include "Player.h"
#include "Collision.h"

// �R���X�g���N�^
EnemySlime::EnemySlime()
{
    model = new Model("Data/Model/Slime/Slime.mdl");
    // ���f�����傫���̂ŃX�P�[�����O
    scale.x = scale.y = scale.z = 0.01f;

    // �������ݒ�
    radius = 0.5f;
    height = 1.0f;
    // �p�j�X�e�[�g�֑J��
    TransitionWanderState();
}

// �f�X�g���N�^
EnemySlime::~EnemySlime()
{
    delete model;
}

// �X�V����
void EnemySlime::Update(float elapsedTime)
{
    // �X�e�[�g���̍X�V����
    switch (state)
    {
    case State::Wander:
        UpdateWanderState(elapsedTime);
        break;
    case State::Idle:
        UpdateIdleState(elapsedTime);
        break;
    case State::Pursuit:
        UpdatePursuitState(elapsedTime);
        break;
    case State::Attack:
        UpdateAttackState(elapsedTime);
        break;
    case State::IdleBattle:
        UpdateIdleBattleState(elapsedTime);
        break;
    case State::Damage:
        UpdateDamageState(elapsedTime);
        break;
    case State::Death:
        UpdateDeathState(elapsedTime);
        break;
    }
    // ���͏����X�V
    UpdateVelocity(elapsedTime);
    // ���G���ԍX�V
    UpdateInvincibleTimer(elapsedTime);
    // �I�u�W�F�N�g�s����X�V
    UpdateTransform();
    // ���f���A�j���[�V�����X�V
    model->UpdateAnimation(elapsedTime);
    // ���f���s��X�V
    model->UpdateTransform(transform);
}

// ���S�������ɌĂ΂��
void EnemySlime::OnDead()
{
    // ���S�X�e�[�g�֑J��
    TransitionDeathState();
}

// �_���[�W���󂯂��Ƃ��ɌĂ΂��
void EnemySlime::OnDamage()
{
    // �_���[�W�X�e�[�g�֑J��
    TransitionDamageState();
}

// �`�揈��
void EnemySlime::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);
}

// �f�o�b�O�v���~�e�B�u�`��
void EnemySlime::DrawDebugPrimitive()
{
    // ���N���X�̃f�o�b�O�v���~�e�B�u�`��
    Enemy::DrawDebugPrimitive();
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    // �꒣��͈͂��f�o�b�O�~���`��
    debugRenderer->DrawCylinder(territoryOrigin, territoryRange, 1.0f, DirectX::XMFLOAT4(0, 1, 0, 1));

    // �^�[�Q�b�g�ʒu���f�o�b�O���`��
    debugRenderer->DrawSphere(targetPosition, radius, DirectX::XMFLOAT4(1, 1, 0, 1));
    // ���G�͈͂��f�o�b�O�~���`��
    debugRenderer->DrawCylinder(position, searchRange, 1.0f, DirectX::XMFLOAT4(0, 0, 1, 1));

    // �U���͈͂��f�o�b�O�~���`��
    debugRenderer->DrawCylinder(position, attackRange, 1.0f, DirectX::XMFLOAT4(1, 0, 0, 1));
}

// �꒣��ݒ�
void EnemySlime::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
    territoryOrigin = origin;
    territoryRange = range;
}

// �^�[�Q�b�g�ʒu�������_���ݒ�
void EnemySlime::SetRandomTargetposition()
{
    // �킩���
    float area = Mathf::RandomRange(-3.14f, 3.14f);
    float Rann = Mathf::RandomRange(0.0f, territoryRange);
    targetPosition.x = territoryOrigin.x + sinf(area) * Rann;
    targetPosition.y = territoryOrigin.y;
    targetPosition.z = territoryOrigin.z + cosf(area) * Rann;
}

// �ڕW�n�_�ֈړ�
void EnemySlime::MoveToTarget(float elapsedTime, float speedRate)
{
    // �^�[�Q�b�g�����֐i�s�x�N�g�����Z�o
    float vx = targetPosition.x - position.x;
    float vz = targetPosition.z - position.z;
    float dist = sqrtf(vx * vx + vz * vz);
    vx /= dist;
    vz /= dist;
    // �ړ�����
    Move(vx, vz, moveSpeed * speedRate);
    Turn(elapsedTime, vx, vz, turnSpeed * speedRate);
}

// �v���C���[���G
bool EnemySlime::SearchPlayer()
{
    // �v���C���[�Ƃ̍��፷���l������3D�ł̋������������
    const DirectX::XMFLOAT3& playerPosition = Player::Instance().GetPosition();
    float vx = playerPosition.x - position.x;
    float vy = playerPosition.y - position.y;
    float vz = playerPosition.z - position.z;
    float dist = sqrtf(vx * vx + vy * vy + vz * vz);
    if (dist < searchRange)
    {
        float distXZ = sqrtf(vx * vx + vz * vz);
        // �P�ʃx�N�g����
        vx /= distXZ;
        vz /= distXZ;
        // �O���x�N�g��
        float frontX = sinf(angle.y);
        float frontZ = cosf(angle.y);
        // �Q�̃x�N�g���̓��ϒl�őO�㔻��
        float dot = (frontX * vx) + (frontZ * vz);
        if (dot > 0.0f)
        {
            return true;
        }
    }
    return false;
}

// �m�[�h�g�v���C���[�̏Փˏ���
void EnemySlime::CollisionNodeVsPlayer(const char* nodeName, float nodeRadius)
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

        // �����蔻��\��
        Graphics::Instance().GetDebugRenderer()->DrawSphere(
            nodePosition, nodeRadius, DirectX::XMFLOAT4(0, 0, 1, 1)
        );

        // �v���C���[�Ɠ����蔻��
        Player& player = Player::Instance();
        DirectX::XMFLOAT3 outPosition;
        if (Collision::IntersectSphererVsCylinder(
            nodePosition,
            nodeRadius,
            player.GetPosition(),
            player.GetRadius(),
            player.GetHeight(),
            outPosition))
        {
            // �_���[�W��^����
            if (player.ApplyDamage(1, 0.5f))
            {
                // �G�𕉂��Ƃ΂��x�N�g�����Z�o
                DirectX::XMFLOAT3 vec;
                vec.x = outPosition.x - nodePosition.x;
                vec.z = outPosition.z - nodePosition.z;
                float length = sqrt(vec.x * vec.x + vec.z * vec.z);
                vec.x /= length;
                vec.z /= length;

                // XZ���ʂɐ�����΂��͂�������
                float power = 10.0f;
                vec.x *= power;
                vec.z *= power;
                // Y�����ɂ��͂�������
                vec.y = 5.0f;

                //������΂�
                player.AddImpulse(vec);
            }
        }
    }
}

// �p�j�X�e�[�g�֑J��
void EnemySlime::TransitionWanderState()
{
    state = State::Wander;

    // �ڕW�n�_�ݒ�
    SetRandomTargetposition();

    // �����A�j���[�V�����Đ�
    model->PlayAnimation(Anim_WalkFWD, true);
}

// �p�j�X�e�[�g�X�V����
void EnemySlime::UpdateWanderState(float elapsedTime)
{
    // �ڕW�n�_�܂�XZ���ʂł̋�������
    float vx = targetPosition.x - position.x;
    float vz = targetPosition.z - position.z;
    float distSq = vx * vx + vz * vz;
    if (distSq < radius * radius)
    {
        // �ҋ@�X�e�[�g�֑J��
        TransitionIdleState();
    }
    // �ڕW�n�_�ֈړ�
    MoveToTarget(elapsedTime, 0.5f);
    // �v���C���[���G
    if (SearchPlayer())
    {
        // ����������ǐՃX�e�[�g�֑J��
        TransitionPursuitState();
    }
}

// �ҋ@�X�e�[�g�֑J��
void EnemySlime::TransitionIdleState()
{
    state = State::Idle;

    // �^�C�}�[�������_���ݒ�
    stateTimer = Mathf::RandomRange(3.0f, 5.0f);

    // �ҋ@�A�j���[�V�����Đ�
    model->PlayAnimation(Anim_IdleNormal, true);
}

// �ҋ@�X�e�[�g�X�V����
void EnemySlime::UpdateIdleState(float elapsedTime)
{
    // �^�C�}�[����
    stateTimer -= elapsedTime;
    if (stateTimer < 0.0f)
    {
        // �p�j�X�e�[�g�֑J��
        TransitionWanderState();
    }
    // �v���C���[���G
    if (SearchPlayer())
    {
        // ����������ǐՃX�e�[�g�֑J��
        TransitionPursuitState();
    }
}

// �ǐՃX�e�[�g�֑J��
void EnemySlime::TransitionPursuitState()
{
    state = State::Pursuit;
    // ���b�ԒǐՂ���^�C�}�[�������_���ݒ�
    stateTimer = Mathf::RandomRange(3.0f, 5.0f);
    // �����A�j���[�V�����Đ�
    model->PlayAnimation(Anim_RunFWD, true);
}

// �ǐՃX�e�[�g�X�V����
void EnemySlime::UpdatePursuitState(float elapsedTime)
{
    // �ڕW�n�_���v���C���[�ʒu�ɐݒ�
    targetPosition = Player::Instance().GetPosition();

    // �ڕW�n�_�ֈړ�
    MoveToTarget(elapsedTime, 1.0f);

    // �^�C�}�[����
    stateTimer -= elapsedTime;
    if (stateTimer < 0.0f)
    {
        // �ҋ@�X�e�[�g�֑J��
        TransitionIdleState();
    }
    // �v���C���[�ɋ߂Â��ƍU���X�e�[�g�֑J��
    float vx = targetPosition.x - position.x;
    float vy = targetPosition.y - position.y;
    float vz = targetPosition.z - position.z;
    float dist = sqrtf(vx * vx + vy * vy + vz * vz);
    if (dist < attackRange)
    {
        // �U���X�e�[�g�֑J��
        TransitionAttackState();
    }
}

// �U���X�e�[�g�J��
void EnemySlime::TransitionAttackState()
{
    state = State::Attack;

    // �U���A�j���[�V�����Đ�
    model->PlayAnimation(Anim_Attack1, false);
}

// �U���X�e�[�g�X�V����
void EnemySlime::UpdateAttackState(float elapsedTime)
{
    // �C�ӂ̃A�j���[�V�����Đ���Ԃł̂ݏՓ˔��菈��������
    float animationTime = model->GetanimeSeconds();
    if (animationTime >= 0.1f && animationTime <= 0.35f)
    {
        // �ڋʃm�[�h�ƃv���C���[�̏Փˏ���
        CollisionNodeVsPlayer("EyeBall", 0.2f);
    }
    // �U���A�j���[�V�������I�������퓬�ҋ@�X�e�[�g�֑J��
    if (!model->IsPlayAnimation())
    {
        TransitionIdleBattleState();
    }
}

// �퓬�ҋ@�X�e�[�g�֑J��
void EnemySlime::TransitionIdleBattleState()
{
    state = State::IdleBattle;

    // ���b�ԑҋ@����^�C�}�[�������_���ݒ�
    stateTimer = Mathf::RandomRange(2.0f, 3.0f);

    // �퓬�ҋ@�A�j���[�V�����Đ�
    model->PlayAnimation(Anim_IdleBattle, true);
}

// �퓬�ҋ@�X�e�[�g�X�V����
void EnemySlime::UpdateIdleBattleState(float elapsedTime)
{
    // �ڕW�n�_���v���C���[�ʒu�ɐݒ�
    targetPosition = Player::Instance().GetPosition();

    // �^�C�}�[����
    stateTimer -= elapsedTime;
    if (stateTimer < 0.0f)
    {
        // �v���C���[���U���͈͂ɂ����ꍇ�͍U���X�e�[�g�֑J��
        float vx = targetPosition.x - position.x;
        float vy = targetPosition.y - position.y;
        float vz = targetPosition.z - position.z;
        float dist = sqrtf(vx * vx + vy * vy + vz * vz);
        if (dist < attackRange)
        {
            // �U���X�e�[�g�֑J��
            TransitionAttackState();
        }
        else
        {
            // �p�j�X�e�[�g�֑J��
            TransitionWanderState();
        }
    }
    MoveToTarget(elapsedTime, 0.0f);
}

// �_���[�W�X�e�[�g�֑J��
void EnemySlime::TransitionDamageState()
{
    state = State::Damage;

    // �_���[�W�A�j���[�V�����Đ�
    model->PlayAnimation(Anim_GetHit, false);
}

// �_���[�W�X�e�[�g�X�V����
void EnemySlime::UpdateDamageState(float elapsedTime)
{
    // �_���[�W�A�j���[�V�������I�������퓬�ҋ@�X�e�[�g�֑J��
    if (!model->IsPlayAnimation())
    {
        TransitionIdleBattleState();
    }
}

// ���S�X�e�[�g�֑J��
void EnemySlime::TransitionDeathState()
{
    state = State::Death;

    // �_���[�W�A�j���[�V�����Đ�
    model->PlayAnimation(Anim_Die, false);
}

// ���S�X�e�[�g�X�V����
void EnemySlime::UpdateDeathState(float elapsedTime)
{
    // �_���[�W�A�j���[�V�������I������玩����j��
    if (!model->IsPlayAnimation())
    {
        Destroy();
    }
}