#include "Graphics/Graphics.h"
#include "Graphics/Model.h"

// コンストラクタ
Model::Model(const char* filename)
{
	// リソース読み込み
	resource = std::make_shared<ModelResource>();
	resource->Load(Graphics::Instance().GetDevice(), filename);

	// ノード
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

	// 行列計算
	const DirectX::XMFLOAT4X4 transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	UpdateTransform(transform);
}

// 変換行列計算
void Model::UpdateTransform(const DirectX::XMFLOAT4X4& transform)
{
	DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);

	for (Node& node : nodes)
	{
		// ローカル行列算出
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
		DirectX::XMMATRIX LocalTransform = S * R * T;

		// ワールド行列算出
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

		// 計算結果を格納
		DirectX::XMStoreFloat4x4(&node.localTransform, LocalTransform);
		DirectX::XMStoreFloat4x4(&node.worldTransform, WorldTransform);
	}
}

// アニメーション更新処理
void Model::UpdateAnimation(float elapsedTime)
{
	// 再生中でないなら処理しない
	if (!IsPlayAnimation()) return;

	// ブレンド率の計算
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

	// 指定アニメーションデータを取得
	const std::vector<ModelResource::Animation>& animations = resource->GetAnimations();
	const ModelResource::Animation& animation = animations.at(currentAnimationIndex);

	// アニメーションデータからキーフレームデータリストを取得
	const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
	int keyCount = static_cast<int>(keyframes.size());
	for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
	{
		// 現在の時間がどのキーフレームの間にいるかを判断する
		const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
		const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
		if (currentAnimationSeconds >= keyframe0.seconds && currentAnimationSeconds < keyframe1.seconds)
		{
			// 再生時間とキーフレームの時間から補間率を算出する。
			float rate = (currentAnimationSeconds - keyframe0.seconds) 
				/ (keyframe1.seconds - keyframe0.seconds);
			int nodeCount = static_cast<int>(nodes.size());
			for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
			{
				// 2つのキーフレーム間の補間計算
				const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
				const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

				Node& node = nodes[nodeIndex];

				// ブレンド補間処理
				if (blendRate < 1.0f)
				{
					// 現在の姿勢と次のキーフレームとの姿勢の補間
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
				// 通常の計算
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

	// 最終フレーム処理
	if (animationEndFlag)
	{
		animationEndFlag = false;
		currentAnimationIndex = -1;
		return;
	}

	// 時間経過
	currentAnimationSeconds += elapsedTime;

	// 再生時間が終端時間を超えたら
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
		// //　ループフラッグが立っていれば
		// // 再生時間の巻き戻し
		// // ループフラッグがfalseであれば
		// // エンドフラッグをtrueにする
		// // 
		// // 再生時間を巻き戻す
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

// アニメーション再生
void Model::PlayAnimation(int index, bool loop, float blendSeconds)
{
	currentAnimationIndex = index;
	currentAnimationSeconds = 0.0f;
	animationLoopFlag = loop;
	animationEndFlag = false;
	animationBlendTime = 0.0f;
	animationBlendSeconds = blendSeconds;
}

// アニメーション再生中か
bool Model::IsPlayAnimation() const
{
	if(currentAnimationIndex < 0) return false;
	if (currentAnimationIndex >= resource->GetAnimations().size()) false;
	return true;
}

// ノード検索
Model::Node* Model::FindNode(const char* name)
{
	// 全てのノードを総当たりで名前を比較する
	for (Node& node : nodes)
	{
		// strcmpで文字比較を行う
		// https://theolizer.com/cpp/useful_cpp/
	    // http://www.cplusplus.com/reference/cstring/strcmp/
		if(strcmp(node.name, name) == 0)
		{
			return &node;
		}
	}
	// 見つからなかった
	return nullptr;
}