#pragma once

#include <vector>
#include "Enemy.h"

// �G�l�~�[�}�l�[�W���[
class EnemyManager
{
private:
    EnemyManager() {}
    ~EnemyManager() {}
    void CollisionEnemyVsEneies();

public:
    // �B��̃C���X�^���X�擾
    static EnemyManager& Instance()
    {
        static EnemyManager instance;
        return instance;
    }
    // �X�V����
    void Update(float elapsedTime);
    // �`�揈��
    void Render(ID3D11DeviceContext* dc, Shader* shader);
    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive();
    // �G�l�~�[�o�^
    void Register(Enemy* enemy);
    // �G�l�~�[���擾
    int GetEnemyCount() const { return static_cast<int>(enemies.size()); }
    // �G�l�~�[�擾
   //  ��ɂہ@Enemy* GetEnemy(int index) { return enemies.at(index).get(); }
    Enemy* GetEnemy(int index) { return enemies.at(index); }
    // �G�l�~�[�S�폜
    void Clear();   
    // �G�l�~�[�폜
    void Remove(Enemy* enemy);

    // �G�l�~�[�\�[�g
    void SortLengthSq(const DirectX::XMFLOAT3& playerPos);
private:
    //�@��ɂہ@std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<Enemy*> enemies;
    std::vector<Enemy*> removes;
};