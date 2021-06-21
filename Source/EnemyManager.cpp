#include "EnemyManager.h"
#include "Collision.h"
#include <vector>
#include <algorithm>

// �X�V����
void EnemyManager::Update(float elapsedTime)
{
    for (Enemy* enemy : enemies)
    {
        enemy->Update(elapsedTime);
    }

    // �j������
    for (Enemy* enemy : removes)
    {
        std::vector<Enemy*>::iterator it = std::find(enemies.begin(), enemies.end(), enemy);
        if (it != enemies.end())
        {
            enemies.erase(it);
        }
        // �G�폜
        delete enemy;
    }
    // �j�����X�g���N���A
    removes.clear();

    // �G���m�̏Փˏ���
    CollisionEnemyVsEneies();
}

// �`�揈��
void EnemyManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
    for (Enemy* enemy : enemies)
    {
        enemy->Render(context, shader);
    }
}
// �f�o�b�O�v���~�e�B�u�`��
void EnemyManager::DrawDebugPrimitive()
{
    for (Enemy* enemy : enemies)
    {
        enemy->DrawDebugPrimitive();
    }
}

// �G�l�~�[�o�^
void EnemyManager::Register(Enemy* enemy)
{
    enemies.emplace_back(enemy);
}

void EnemyManager::Clear()
{
    for (Enemy* enemy : enemies)
    {
        delete enemy;
    }
}

void EnemyManager::Remove(Enemy* enemy)
{
    // �j�����X�g�ɒǉ�
    // emplace_back��push_back�̈Ⴂ  
    // https://qiita.com/brackss1/items/e92da6458172397f7225
    removes.emplace_back(enemy);
}

// �G�l�~�[���m�̏Փˏ���
void EnemyManager::CollisionEnemyVsEneies()
{
    // �S�Ă̓G�Ƒ�������ŏՓˏ���
    // int enemyCount = GetEnemyCount();
    size_t enemyCount = enemies.size();
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy* enemy1 = enemies.at(i);
        for (int j = i + 1; j < enemyCount; ++j)
        {
            // if (i == j) continue;
            Enemy* enemy2 = enemies.at(j);
            // �Փˏ���
            DirectX::XMFLOAT3  outPosition;
            if (Collision::IntersectCylinderVsCylinder(enemy1->GetPosition(), enemy1->GetRadius(), enemy1->GetHeight(), enemy2->GetPosition(), enemy2->GetRadius(), enemy2->GetHeight(), outPosition))
            {
                enemy2->SetPosition(outPosition);
            }
        }
    }
}
void EnemyManager::SortLengthSq(const DirectX::XMFLOAT3& playerPos)
{
    float l;
    // �e�G�Ƀv���C���[�Ƃ̋�����2���ۑ�����
    for (auto& a: enemies)
    {
        l = powf((a->GetPosition().x - playerPos.x), 2) + powf((a->GetPosition().z - playerPos.z), 2);
        a->SetLengthSq(l);
    }
    // �v���C���[�Ƃ̋����̂Q��Ń\�[�g����B // �����_�����p https://codezine.jp/article/detail/6020
    // begin��end�̂Q�̃C�e���[�^���Ń\�[�g����
    //
    std::sort(enemies.begin(), enemies.end(), [](Enemy* x,Enemy* y)->int
        {return x->GetLengthSq() < y->GetLengthSq();});
}