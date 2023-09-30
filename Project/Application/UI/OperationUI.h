#pragma once
#include "Sprite.h"

class OperationUI
{
private:

	// �X�v���C�g�̃^�C�v
	enum Type
	{
		AButtonUp,
		AButtonDown,

		LstickUnder,
		LstickCover,

		AttackText,
		MoveText,

		Size,
	};
private:
	// ����X�v���C�g
	std::vector<std::unique_ptr<Sprite>> sprites_;

	// A�{�^���̃e�N�X�`��
	std::unique_ptr<Texture> AbuttonTex_;

	// L�X�e�B�b�N�̃e�N�X�`��
	std::unique_ptr<Texture> LstickTex_;

	// �ړ��̃e�L�X�g�e�N�X�`��
	std::unique_ptr<Texture> moveTextTex_;

	// �U���̃e�L�X�g�e�N�X�`��
	std::unique_ptr<Texture> attackTextTex_;

	// L�X�e�B�b�N�̃f�t�H���g�|�W�V����
	Vector2 LstickDefuPos_;

public:
	OperationUI();

	/// <summary>
	/// ������
	/// </summary>
	void Init();

	/// <summary>
	/// �X�V����
	/// </summary>
	void Update();

	/// <summary>
	/// �X�v���C�g�̕`��
	/// </summary>
	void DrawFrontSprite();

};

