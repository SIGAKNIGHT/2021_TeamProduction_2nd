#pragma once

#include <xaudio2.h>
#include "Audio/AudioSource.h"

// オーディオ
class Audio
{
public:
	Audio();
	~Audio();

public:
	// インスタンス取得
	static Audio& Instance()
	{
		static Audio instance;
		return instance;
	}

	// オーディオソース読み込み
	std::unique_ptr<AudioSource> LoadAudioSource(const char* filename, bool loop);

private:
	IXAudio2*				xaudio = nullptr;
	IXAudio2MasteringVoice* masteringVoice = nullptr;
};
