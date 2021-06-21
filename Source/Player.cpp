#include <imgui.h>
#include "Player.h"
#include "Camera.h"
#include "Input/Input.h"
#include "Graphics/Graphics.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "ProjectileStraight.h"
#include "ProjectileHoming.h"
#include "CameraController.h"

// �R���X�g���N�^
Player::Player()
{
    //model = new Model("Data/Model/Mr.Incredible/Mr.Incredible.mdl");
    model = new Model("Data/Model/Jummo/Jummo.mdl");
    // model->PlayAnimation(0);

    // ���f�����傫���̂ŃX�P�[�����O
    scale.x = scale.y = scale.z = 0.01f;

    // �q�b�g�G�t�F�N�g�ǂݍ���
    hitEffect = new Effect("Data/Effect/Hit.efk");
    
    // �ҋ@�X�e�[�g�֑J��
    TransitionIdleState();
}

// �f�X�g���N�^
Player::~Player()
{
    delete hitEffect;
    delete model;
}

//�@�X�V����
void Player::Update(float elapsedTime)
{
    // �X�e�[�g���̏���
    switch (state)
    {
    case State::Idle:
        UpdateIdleState(elapsedTime);
        break;
    case State::Move:
        UpdateMoveState(elapsedTime);
        break;
    case State::Jump:
        UpdateJumpState(elapsedTime);
        break;
    case State::Land:
        UpdateLandState(elapsedTime);
        break;
    case State::Attack:
        UpdateAttackState(elapsedTime);
        break;
    }
    // ���͍X�V����
    UpdateVelocity(elapsedTime);

    // �e�ۍX�V����
    projectileManager.Update(elapsedTime);

    // �v���C���[�ƓG�̏Փˏ���
    CollisionPlayerVsEnemies();

    // �e�ۂƓG�̏Փ˔���
    CollisionProjectilesVsEnemies();

    // �I�u�W�F�N�g�s����X�V
    UpdateTransform();
    
    // ���f���A�j���[�V�����X�V����
    model->UpdateAnimation(elapsedTime);

    // ���f���s��̍X�V
    model->UpdateTransform(transform);
}

// �`�揈��
void Player::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);
    projectileManager.Render(dc, shader);
}

// �f�o�b�O�pGUI�`��
void Player::DrawDebugGUI()
{
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
    {
        // �g�����X�t�H�[��
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // �ʒu
            ImGui::InputFloat3("Position", &position.x);
            // ��]
            DirectX::XMFLOAT3 a;
            a.x = DirectX::XMConvertToDegrees(angle.x);
            a.y = DirectX::XMConvertToDegrees(angle.y);
            a.z = DirectX::XMConvertToDegrees(angle.z);
            ImGui::InputFloat3("Angle", &a.x);
            angle.x = DirectX::XMConvertToRadians(a.x);
            angle.y = DirectX::XMConvertToRadians(a.y);
            angle.z = DirectX::XMConvertToRadians(a.z);
            // �X�P�[��
            ImGui::InputFloat3("Scale", &scale.x);
        }
    }
    ImGui::End();
}

// �f�o�b�O�v���~�e�B�u�`��
void Player::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    // �Փ˔���p�̃f�o�b�O����`��
    debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
    projectileManager.DrawDebugPrimitive();


    // �U���Փ˗p�̍���m�[�h�̃f�o�b�O����`��
    if (attackCollisionFlag)
    {
        Model::Node* leftHandBone = model->FindNode("mixamorig:LeftHand");
        debugRenderer->DrawSphere(
            DirectX::XMFLOAT3(
            leftHandBone->worldTransform._41,
            leftHandBone->worldTransform._42,
            leftHandBone->worldTransform._43),
            lehtHandRadius,
            DirectX::XMFLOAT4(1, 0, 0, 1)
        );
    }
}

DirectX::XMFLOAT3 Player::GetMoveVec() const
{
    // ���͏����擾
    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

    //  �J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
    Camera& camera = Camera::Instance();
    const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
    const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

    // �ړ��x�N�g����XZ���ʂɐ����ȃx�N�g���ɂȂ�悤�ɂ���B

    // �J�����E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
    float cameraRightX = cameraRight.x;
    float cameraRightZ = cameraRight.z;
    float cameraRightLenght = sqrtf(powf(cameraRightX, 2) + powf(cameraRightZ, 2));
    if (cameraRightLenght > 0.0f)
    {
        cameraRightX /= cameraRightLenght;
        cameraRightZ /= cameraRightLenght;
    }

    // �J�����O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
    float cameraFrontX = cameraFront.x;
    float cameraFrontZ = cameraFront.z;
    float cameraFrontLenght = sqrtf(powf(cameraFrontX, 2) + powf(cameraFrontZ, 2));
    if (cameraFrontLenght > 0.0f)
    {
        cameraFrontX /= cameraFrontLenght;
        cameraFrontZ /= cameraFrontLenght;
    }

    // �X�e�B�b�N�̐������͒l���J���������ɔ��f���A
    // �X�e�B�b�N�̐������͒l���v�Z����B
    // �i�s�x�N�g�����v�Z����B
    DirectX::XMFLOAT3 vec;
    vec.x = cameraFrontX  + cameraRightX ;
    vec.z = cameraFrontZ  + cameraRightZ ;
    // Y���ɂ͈ړ����Ȃ�
    vec.y = 0.0f;
    return vec;
}

bool Player::InputMove(float elapsedTime)
{
    // �i�s�x�N�g���擾
    DirectX::XMFLOAT3 moveVec = GetMoveVec();

    // �ړ�����
    // Move(moveVec.x, moveVec.z, moveSpeed);

    // ���񏈗�
    // Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

    // �i�s�x�N�g�����[���x�N�g���łȂ��ꍇ�͓��͂��ꂽ // �O�����Z�q(�]���� ? �^ : �U)
    return (sqrt(powf(moveVecX, 2) + powf(moveVecZ, 2)) != 0 ? true : false);
}

// �v���C���[�ƃG�l�~�[�̏Փˏ���
void Player::CollisionPlayerVsEnemies()
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    // �S�Ă̓G�Ƒ�������ŏՓˏ���
    int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy* enemy = enemyManager.GetEnemy(i);

        // �Փˏ���
        DirectX::XMFLOAT3  outPosition;
       //if (Collision::IntersectSphereVsSphere(this->position, this->radius, enemy->GetPosition(), enemy->GetRadius(), outPosition))
       if (Collision::IntersectCylinderVsCylinder
       (this->position, this->radius,this->height, 
           enemy->GetPosition(), enemy->GetRadius(), enemy->GetHeight(), 
           outPosition))
       {
           DirectX::XMFLOAT3 norm;
           DirectX::XMVECTOR pl = DirectX::XMLoadFloat3(&this->position);
           DirectX::XMVECTOR el = DirectX::XMLoadFloat3(&enemy->GetPosition());
           DirectX::XMVECTOR Norm = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(pl, el));
           DirectX::XMStoreFloat3(&norm, Norm);
           if (norm.y > 0.75f)//this->position.y > enemy->GetPosition().y)
           {
               Jump(jumpSpeed * 0.5f);
               // �_���[�W��^����
               enemy->ApplyDamage(1, 0.1);
               continue;
           }
           enemy->SetPosition(outPosition);
       }
    }
}

// �m�[�h�ƓG�̏Փˏ���
void Player::CollisionNodeVsEnemies(const char* nodeName, float nodeRadius)
{
    // �m�[�h�擾
    Model::Node* node = model->FindNode(nodeName);

    // �m�[�h�ʒu�擾
    DirectX::XMFLOAT3 nodePosition;
    nodePosition.y = node->worldTransform._42;
    nodePosition.x = node->worldTransform._41;
    nodePosition.z = node->worldTransform._43;

    // �w��̃m�[�h�ƑS�Ă̓G�𑍓�����ŏՓˏ���
    EnemyManager& enemyManager = EnemyManager::Instance();

    // �S�Ă̓G�Ƒ�������ŏՓˏ���
    int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy* enemy = enemyManager.GetEnemy(i);

        // �Փˏ���
        DirectX::XMFLOAT3  outPosition;
        if (Collision::IntersectSphererVsCylinder
        (nodePosition, lehtHandRadius,
            enemy->GetPosition(), enemy->GetRadius(), enemy->GetHeight(),
            outPosition))
        {
            // �_���[�W��^����
            if (enemy->ApplyDamage(1, 0.5f))
            {
                // �q�b�g�G�t�F�N�g�Đ�
                {
                    DirectX::XMFLOAT3 e = enemy->GetPosition();
                    e.y += enemy->GetHeight() * 0.5f;
                    hitEffect->Play(e);
                }
            }
        }
    }

}

// �W�����v���͏���
bool Player::InputJump()
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (gamePad.GetButtonDown() & GamePad::BTN_A)
    {
        if (jumpCount > 0)
        {
            Jump(jumpSpeed);
            jumpCount--;
            return true;
        }
    }
    return false;
}

// �U�����͏���
bool Player::InputAttack()
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    if (gamePad.GetButtonDown() & GamePad::BTN_B)
    {
        return true;
    }
    return false;
}

// ���n����
void Player::OnLanding()
{
    jumpCount = jumpLimit;
    // �������̑��͂����ȏ�Ȃ璅�n�X�e�[�g��
    if (velocity.y < 0.0f)
    {
        TransitionLandState();
    }
}

// �e�ۓ��͏���
void Player::InputProjectile()
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();

    // ���i�e�۔���
    if (mouse.GetButton() & Mouse::BTN_LEFT)
    {
        projectileSpeed += 1.0f;
        if (projectileSpeed > 40.0f)
        {
            projectileSpeed = 40.0f;
        }
    }
    if (mouse.GetButtonUp() & Mouse::BTN_LEFT)
    {
        // �O����
        DirectX::XMFLOAT3 dir;
        dir.x = sinf(angle.y);
        dir.y = sinf(-angle.x);
        dir.z = cosf(angle.y);
        // ���ˈʒu(�v���C���[�̍�������)
        DirectX::XMFLOAT3 pos;
        pos.x = this->position.x;
        pos.y = this->position.y + height * 0.5f;
        pos.z = this->position.z;
        // ����
        ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);
        projectile->Launch(dir, pos, projectileSpeed);
        projectileSpeed = 10;
    }
}

// �e�ۂƓG�̏Փˏ���
void Player::CollisionProjectilesVsEnemies()
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    // �S�Ă̒e�ۂƑS�Ă̓G�𑍓�����ŏՓ˔���
    int projectileCount = projectileManager.GetProjectileCount();
    int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < projectileCount; ++i)
    {
        Projectile* projectile = projectileManager.GetProjectile(i);
        for (int j = 0; j < enemyCount; ++j)
        {
            Enemy* enemy = enemyManager.GetEnemy(j);
            // �Փ˔���
            DirectX::XMFLOAT3 outPosition;
            if (Collision::IntersectSphererVsCylinder(
                projectile->GetPosition(),
                projectile->GetRadius(),
                enemy->GetPosition(),
                enemy->GetRadius(),
                enemy->GetHeight(),
                outPosition))
            {
                // �_���[�W��^����
                if (enemy->ApplyDamage(1, 0.5f))
                {
                    // ������΂�
                    {
                        DirectX::XMFLOAT3 impluse;
                        float xl = enemy->GetPosition().x - projectile->GetPosition().x;
                        float zl = enemy->GetPosition().z - projectile->GetPosition().z;
                        float xzLength = sqrtf(powf(xl, 2) + powf(zl, 2));
                        xl /= xzLength;
                        zl /= xzLength;
                        impluse.x = xl * projectile->GetImpluse();
                        impluse.y = projectile->GetImpluse();
                        impluse.z = zl * projectile->GetImpluse();
                        enemy->AddImpulse(impluse);
                    }

                    // �q�b�g�G�t�F�N�g�Đ�
                    {
                        DirectX::XMFLOAT3 e = enemy->GetPosition();
                        e.y += enemy->GetHeight() * 0.5f;
                        hitEffect->Play(e);
                    }
                    // �e�۔j��
                    projectile->Destroy();
                }
            }
        }
    }
}

// �ҋ@�X�e�[�g�֑J��
void Player::TransitionIdleState()
{
    state = State::Idle;

    // �ҋ@�A�j���[�V�����Đ�
    model->PlayAnimation(Anim_Idle, true);
}

// �ҋ@�X�e�[�g�X�V����
void Player::UpdateIdleState(float elapsedTime)
{
    // �ړ����͏���
    if (InputMove(elapsedTime))
    {
        TransitionMoveState();
    }

    // �W�����v���͏���
    if (InputJump())
    {
        TransitionJumpState();
    }

    // �e�ۓ��͏���
    InputProjectile();

    // �U�����͏���
    if (InputAttack())
    {
        TransitionAttackState();
    }
}

// �ړ��X�e�[�g�֑J��
void Player::TransitionMoveState()
{
    state = State::Move;

    // ����A�j���[�V�����Đ�
    model->PlayAnimation(Anim_Running, true, 0.1);
}

// �ړ��X�e�[�g�X�V����
void Player::UpdateMoveState(float elapsedTime)
{
    // �ړ����͏���
    if (!InputMove(elapsedTime))
    {
        TransitionIdleState();
    }

    // �W�����v���͏���
    if (InputJump())
    {
        TransitionJumpState();
    }

    // �U�����͏���
    if (InputAttack())
    {
        TransitionAttackState();
    }

    // �e�ۓ��͏���
    InputProjectile();
}

// �W�����v�X�e�[�g�֑J��
void Player::TransitionJumpState()
{
    state = State::Jump;
    // �W�����v�A�j���[�V�����Đ�
    model->PlayAnimation(Anim_Jump, false, 0.1);
}

// �W�����v�X�e�[�g�X�V����
void Player::UpdateJumpState(float elapsedTime)
{
    InputMove(elapsedTime);
    // �W�����v���͏���
    if (InputJump())
    {
        model->PlayAnimation(Anim_Jump_Flip, false, 0.1);
    }
    if (!model->IsPlayAnimation())
    {
        model->PlayAnimation(Anim_Falling, true, 0.1);
    }
    // �e�ۓ��͏���
    InputProjectile();
}

// ���n�X�e�[�g�֑J��
void Player::TransitionLandState()
{
    state = State::Land;

    // ���n�A�j���[�V�����Đ�
    model->PlayAnimation(Anim_Landing, false, 0.1);
}

// ���n�X�e�[�g�̍X�V����
void Player::UpdateLandState(float elapsedTime)
{
    // �ړ����͏���
    if (!model->IsPlayAnimation())
    {
        TransitionIdleState();
    }
}

// �U���X�e�[�g�֑J��
void Player::TransitionAttackState()
{
    state = State::Attack;

    //  �U���A�j���[�V�����Đ�
    model->PlayAnimation(Anim_Attack, false, 0.1);
}

// �U���X�e�[�g�X�V����
void Player::UpdateAttackState(float elapsedTime)
{
    if (!model->IsPlayAnimation())
    {
        TransitionIdleState();
    }

    // �C�ӂ̃A�j���[�V�����Đ���Ԃł̂ݏՓ˔��菈��������
    float animationTime = model->GetanimeSeconds();
    if (animationTime > 0.3f && animationTime < 4.0f)
    {
        attackCollisionFlag = true;
    }
    else
    {
        attackCollisionFlag = false;
    }
    if (attackCollisionFlag)
    {
        // ����m�[�h�ƃG�l�~�[�̏Փˏ���
        CollisionNodeVsEnemies("mixamorig:LeftHand", lehtHandRadius);
    }
}