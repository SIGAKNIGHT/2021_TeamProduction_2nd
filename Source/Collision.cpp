#include "Collision.h"
// using namespace DirectX;

// ���Ƌ��̌�������
bool Collision::IntersectSphereVsSphere(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    DirectX::XMFLOAT3& outPositionB
)
{
    // using namespace DirectX;
    // A��B�̒P�ʃx�N�g�����Z�o
    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);
    DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
    float lengthSq;
    DirectX::XMStoreFloat(&lengthSq, LengthSq);


    // ��������
    float range = radiusA + radiusB;
    if (lengthSq > range * range)
    {
        return false;
    }

    // A��B�������o��
    Vec = DirectX::XMVector3Normalize(Vec);
    PositionB = DirectX::XMVectorAdd(PositionA ,DirectX::XMVectorScale(Vec,range));
    DirectX::XMStoreFloat3(&outPositionB, PositionB);


    return true;
}

bool Collision::IntersectCylinderVsCylinder(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    float heightA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    float heightB,
    DirectX::XMFLOAT3& outPositionB
)
{
    // A�̑�����B�̓�����Ȃ瓖�����Ă��Ȃ�
    if (positionA.y > positionB.y + heightB)
    {
        return false;
    }
    // A�̓���B�̑�����艺�Ȃ瓖�����Ă��Ȃ�
    if (positionA.y + heightA < positionB.y)
    {
        return false;
    }
    //XZ���ʂł͈̔̓`�F�b�N
    float length;
    float lx, lz;
    lx = positionB.x - positionA.x;
    lz = positionB.z - positionA.z;
    length = sqrtf(powf(lx, 2) + powf(lz, 2));
    float range = radiusA + radiusB;
    if (length > range)
    {
        return false;
    }

    // A��B�������o��
    lx = (lx / length) * range;
    lz = (lz / length) * range;
    outPositionB.x = positionA.x + lx;
    outPositionB.y = positionB.y;
    outPositionB.z = positionA.z + lz;


    return true;
}
bool Collision::IntersectSphererVsCylinder(
    const DirectX::XMFLOAT3& spherePosition,
    float sphereRadius,
    const DirectX::XMFLOAT3& cylinderPosition,
    float cylinderRadius,
    float cylinderHeight,
    DirectX::XMFLOAT3& outCylinderPosition
)
{
    // A�̑�����B�̓�����Ȃ瓖�����Ă��Ȃ�
    if (spherePosition.y > cylinderPosition.y + cylinderHeight)
    {
        return false;
    }
    // A�̓���B�̑�����艺�Ȃ瓖�����Ă��Ȃ�
    if (spherePosition.y + sphereRadius < cylinderPosition.y)
    {
        return false;
    }
    //XZ���ʂł͈̔̓`�F�b�N
    float length;
    float lx, lz;
    lx = cylinderPosition.x - spherePosition.x;
    lz = cylinderPosition.z - spherePosition.z;
    length = sqrtf(powf(lx, 2) + powf(lz, 2));
    float range = sphereRadius + cylinderRadius;
    if (length > range)
    {
        return false;
    }

    // A��B�������o��
    lx = (lx / length) * range;
    lz = (lz / length) * range;
    outCylinderPosition.x = spherePosition.x + lx;
    outCylinderPosition.y = cylinderPosition.y;
    outCylinderPosition.z = spherePosition.z + lz;
    return true;
}