#pragma once
#include "SpriteManager.h"
#include "Matrix4.h"
#include "Vector2.h"
#include "Vector4.h"

class Sprite
{
public:// �T�u�N���X

	enum VertexNumber {
		LB,	//����
		LT,	//����
		RB,	//�E��
		RT,	//�E��
	};

public:// �����o�֐�
	// ������
	void Initialize(SpriteManager *spriteManager);

	// �X�V����
	void Updata();

	// �`��
	void Draw();

	// ���W�ݒ�
	void SetPosition(const Vector3& position) { position_ = position; }

	// ���W�̎擾
	const Vector3& GetPosition()const { return position_; }

	// ��]�̐ݒ�
	void SetRotation(float rotation) { rotation_.z = rotation; }

	// ��]�l�̎擾
	float GetRotation()const { return rotation_.z; }

	// �F�̐ݒ�
	void SetColor(const Vector4& color) { color_ = color; }

	// �F�̎擾
	const Vector4& GetColor() const { return color_; }

	// �T�C�Y�̐ݒ�
	void SetSize(const Vector2& size) { size_ = size; }

	// �T�C�Y�̎擾
	const Vector2& GetSize()const { return size_; }

	// �A���J�[�|�C���g�̐ݒ�
	void SetAnchorPoint(const Vector2& anchorPoint) { anchorPoint_ = anchorPoint;}

	// �A���J�[�|�C���g�̎擾
	const Vector2& GetAnchorPoint()const { return anchorPoint_; }

	// ���E���]�̐ݒ�
	void SetFlipX(const bool& isFlipX) { isFlipX_ = isFlipX; }

	// ���E���]�̎擾
	const bool& GetFlipX()const { return isFlipX_; }

	// �㉺���]�̐ݒ�
	void SetFlipY(const bool& isFlipY) { isFlipY_ = isFlipY; }

	// �㉺���]�̎擾
	const bool& GetFlipY()const { return isFlipY_; }

	// ��\���̐ݒ�
	void SetInvisible(const bool& invisible) { isInvisible_ = invisible; }

	// ��\���̃t���O�̎擾
	const bool& GetInvisible()const { return isInvisible_; }

private:// �����o�ϐ�

	SpriteManager::Vertex vertices[4] = {
	{{  0.0f,100.0f, 0.0f },{0.0f,1.0f}}, // ����
	{{  0.0f,  0.0f, 0.0f },{0.0f,0.0f}}, // ����
	{{100.0f,100.0f, 0.0f },{1.0f,1.0f}}, // �E��
	{{100.0f,  0.0f, 0.0f },{1.0f,0.0f}}, // �E��
	};

	Matrix4 matWorld;	//���[���h�ϊ��s��

	Vector3 rotation_ = { 0.0f,0.0f,0.0f };	//��]�p
	Vector3 position_ = { 100.0f,100.0f,0.0f };	//���W
	Vector4 color_ = { 1,1,1,1 };	//���W
	Vector2 size_ = { 100.0f,100.0f };//�X�v���C�g�T�C�Y
	Vector2 anchorPoint_ = { 0.5f,0.5f };//�A���J�[�|�C���g(���W�ϊ��̊�_)
	bool isFlipX_ = false;	//���E���]�t���O
	bool isFlipY_ = false;	//�㉺���]�t���O
	bool isInvisible_ = false;	//��\���t���O

	SpriteManager* spriteManager_ = nullptr;

	SpriteManager::ConstBufferData* constMap = nullptr;

	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	// ���_�}�b�v
	SpriteManager::Vertex* vertMap = nullptr;

	// ���_�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff = nullptr;

	// �萔�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff = nullptr;

	// ���ʊm�F
	HRESULT result;
};

