#pragma once
#include "Sprite.h"
#include "WorldTransform.h"
class TimerUI
{
private:
	enum Unit
	{
		One,
		Ten,
		Hundred,

		Size,
	};

private:
	// �^�C�}�[�p�̃X�v���C�g
	std::array<std::unique_ptr<Sprite>, Unit::Size> mTimerUnit;

	// �e���[���h���
	WorldTransform mParent;

	// �X�v���C�g�̃T�C�Y��Ԋu
	float mTexSize = 128;
	float mTexSpace = mTexSize - 16;

	// �X�v���C�g�̌�
	uint32_t dight = 0;

	// �X�v���C�g�p�̃i���o�[�e�N�X�`��
	std::unique_ptr<Texture> numberTex_;

	// �^�C�}�[
	int32_t gameTimer_ = 0;
	int32_t remainTime_ = 60 * 3;

private:
	void TextureSetting();

public:
	TimerUI();
	void Init();
	void Update();
	void DrawFrontSprite();

	void Reset();
};

