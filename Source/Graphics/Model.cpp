#include "Graphics/Graphics.h"
#include "Graphics/Model.h"

// �R���X�g���N�^
Model::Model(const char* filename)
{
	// ���\�[�X�ǂݍ���
	resource = std::make_shared<ModelResource>();
	resource->Load(Graphics::Instance().GetDevice(), filename);

	// �m�[�h
	const std::vector<ModelResource::Node>& resNodes = resource->GetNodes();

	nodes.resize(resNodes.size());
	for (size_t nodeIndex = 0; nodeIndex < nodes.size(); ++nodeIndex)
	{
		auto&& src = resNodes.at(nodeIndex);
		auto&& dst = nodes.at(nodeIndex);

		dst.name = src.name.c_str();
		dst.parent = src.parentIndex >= 0 ? &nodes.at(src.parentIndex) : nullptr;
		dst.scale = src.scale;
		dst.rotate = src.rotate;
		dst.translate = src.translate;

		if (dst.parent != nullptr)
		{
			dst.parent->children.emplace_back(&dst);
		}
	}

	// �s��v�Z
	const DirectX::XMFLOAT4X4 transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	UpdateTransform(transform);
}

// �ϊ��s��v�Z
void Model::UpdateTransform(const DirectX::XMFLOAT4X4& transform)
{
	DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);

	for (Node& node : nodes)
	{
		// ���[�J���s��Z�o
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
		DirectX::XMMATRIX LocalTransform = S * R * T;

		// ���[���h�s��Z�o
		DirectX::XMMATRIX ParentTransform;
		if (node.parent != nullptr)
		{
			ParentTransform = DirectX::XMLoadFloat4x4(&node.parent->worldTransform);
		}
		else
		{
			ParentTransform = Transform;
		}
		DirectX::XMMATRIX WorldTransform = LocalTransform * ParentTransform;

		// �v�Z���ʂ��i�[
		DirectX::XMStoreFloat4x4(&node.localTransform, LocalTransform);
		DirectX::XMStoreFloat4x4(&node.worldTransform, WorldTransform);
	}
}

// �A�j���[�V�����X�V����
void Model::UpdateAnimation(float elapsedTime)
{
	// �Đ����łȂ��Ȃ珈�����Ȃ�
	if (!IsPlayAnimation()) return;

	// �u�����h���̌v�Z
	float blendRate = 1.0f;
	//animationBlendTime += elapsedTime;
	//if (animationBlendTime > animationBlendSeconds)
	//{
	//	animationBlendTime = animationBlendSeconds;
	//}
	//blendRate = animationBlendTime / animationBlendSeconds;
	if (animationBlendSeconds > animationBlendTime)
	{
		animationBlendTime += elapsedTime;
		if (animationBlendSeconds <= animationBlendTime)
		{
			animationBlendTime = animationBlendSeconds;
		}
		blendRate = animationBlendTime / animationBlendSeconds;
		blendRate *= blendRate;
	}

	// �w��A�j���[�V�����f�[�^���擾
	const std::vector<ModelResource::Animation>& animations = resource->GetAnimations();
	const ModelResource::Animation& animation = animations.at(currentAnimationIndex);

	// �A�j���[�V�����f�[�^����L�[�t���[���f�[�^���X�g���擾
	const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
	int keyCount = static_cast<int>(keyframes.size());
	for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
	{
		// ���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩�𔻒f����
		const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
		const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
		if (currentAnimationSeconds >= keyframe0.seconds && currentAnimationSeconds < keyframe1.seconds)
		{
			// �Đ����ԂƃL�[�t���[���̎��Ԃ����ԗ����Z�o����B
			float rate = (currentAnimationSeconds - keyframe0.seconds) 
				/ (keyframe1.seconds - keyframe0.seconds);
			int nodeCount = static_cast<int>(nodes.size());
			for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
			{
				// 2�̃L�[�t���[���Ԃ̕�Ԍv�Z
				const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
				const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

				Node& node = nodes[nodeIndex];

				// �u�����h��ԏ���
				if (blendRate < 1.0f)
				{
					// ���݂̎p���Ǝ��̃L�[�t���[���Ƃ̎p���̕��
					DirectX::XMVECTOR key0_S = DirectX::XMLoadFloat3(&node.scale);
					DirectX::XMVECTOR key1_S = DirectX::XMLoadFloat3(&key0.scale);
					DirectX::XMVECTOR key0_R = DirectX::XMLoadFloat4(&node.rotate);
					DirectX::XMVECTOR key1_R = DirectX::XMLoadFloat4(&key0.rotate);
					DirectX::XMVECTOR key0_T = DirectX::XMLoadFloat3(&node.translate);
					DirectX::XMVECTOR key1_T = DirectX::XMLoadFloat3(&key0.translate);

					DirectX::XMVECTOR Scale = DirectX::XMVectorLerp(key0_S, key1_S, blendRate);
					DirectX::XMVECTOR Rotate = DirectX::XMQuaternionSlerp(key0_R, key1_R, blendRate);
					DirectX::XMVECTOR Translate = DirectX::XMVectorLerp(key0_T, key1_T, blendRate);

					DirectX::XMStoreFloat3(&node.scale, Scale);
					DirectX::XMStoreFloat4(&node.rotate, Rotate);
					DirectX::XMStoreFloat3(&node.translate, Translate);
				}
				// �ʏ�̌v�Z
				else
				{
					DirectX::XMVECTOR key0_S = DirectX::XMLoadFloat3(&key0.scale);
					DirectX::XMVECTOR key1_S = DirectX::XMLoadFloat3(&key1.scale);
					DirectX::XMVECTOR key0_R = DirectX::XMLoadFloat4(&key0.rotate);
					DirectX::XMVECTOR key1_R = DirectX::XMLoadFloat4(&key1.rotate);
					DirectX::XMVECTOR key0_T = DirectX::XMLoadFloat3(&key0.translate);
					DirectX::XMVECTOR key1_T = DirectX::XMLoadFloat3(&key1.translate);

					DirectX::XMVECTOR Scale = DirectX::XMVectorLerp(key0_S, key1_S, rate);
					DirectX::XMVECTOR Rotate = DirectX::XMQuaternionSlerp(key0_R, key1_R, rate);
					DirectX::XMVECTOR Translate = DirectX::XMVectorLerp(key0_T, key1_T, rate);

					DirectX::XMStoreFloat3(&node.scale, Scale);
					DirectX::XMStoreFloat4(&node.rotate, Rotate);
					DirectX::XMStoreFloat3(&node.translate, Translate);
				}
			}
			break;
		}
	}

	// �ŏI�t���[������
	if (animationEndFlag)
	{
		animationEndFlag = false;
		currentAnimationIndex = -1;
		return;
	}

	// ���Ԍo��
	currentAnimationSeconds += elapsedTime;

	// �Đ����Ԃ��I�[���Ԃ𒴂�����
	if (currentAnimationSeconds > animation.secondsLength)
	{   
		if (animationLoopFlag)
		{
			currentAnimationSeconds -= animation.secondsLength;
		}
		else
		{
			currentAnimationSeconds = animation.secondsLength;
			animationEndFlag = true;
		}
		// //�@���[�v�t���b�O�������Ă����
		// // �Đ����Ԃ̊����߂�
		// // ���[�v�t���b�O��false�ł����
		// // �G���h�t���b�O��true�ɂ���
		// // 
		// // �Đ����Ԃ������߂�
		// if (animationLoopFlag)
		// {
		// 	currentAnimationSeconds -= animation.secondsLength;
		// }
		// else 
		// {
		// 	currentAnimationSeconds = 0;
		// 	animationEndFlag = true;
		// }
	}
}

// �A�j���[�V�����Đ�
void Model::PlayAnimation(int index, bool loop, float blendSeconds)
{
	currentAnimationIndex = index;
	currentAnimationSeconds = 0.0f;
	animationLoopFlag = loop;
	animationEndFlag = false;
	animationBlendTime = 0.0f;
	animationBlendSeconds = blendSeconds;
}

// �A�j���[�V�����Đ�����
bool Model::IsPlayAnimation() const
{
	if(currentAnimationIndex < 0) return false;
	if (currentAnimationIndex >= resource->GetAnimations().size()) false;
	return true;
}

// �m�[�h����
Model::Node* Model::FindNode(const char* name)
{
	// �S�Ẵm�[�h�𑍓�����Ŗ��O���r����
	for (Node& node : nodes)
	{
		// strcmp�ŕ�����r���s��
		// https://theolizer.com/cpp/useful_cpp/
	    // http://www.cplusplus.com/reference/cstring/strcmp/
		if(strcmp(node.name, name) == 0)
		{
			return &node;
		}
	}
	// ������Ȃ�����
	return nullptr;
}