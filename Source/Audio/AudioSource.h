#pragma once

#include <memory>
#include <xaudio2.h>
#include "Audio/AudioResource.h"

// �I�[�f�B�I�\�[�X
class AudioSource
{
public:
	AudioSource(IXAudio2* xaudio, std::shared_ptr<AudioResource>& resource, bool loop);
	~AudioSource();

	// �Đ�
	void Play();

	// ��~
	void Stop();

private:
	IXAudio2SourceVoice*			sourceVoice = nullptr;
	std::shared_ptr<AudioResource>	resource;
};
