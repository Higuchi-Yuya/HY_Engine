#pragma once
#include "SpriteManager.h"

class Sprite
{
	
public:// �����o�֐�
	// ������
	void Initialize(SpriteManager *spriteManager);

	// �`��
	void Draw();

private:// �����o�ϐ�

	DirectX::XMFLOAT3 vertices[3]={
	{ -0.5f, -0.5f, 0.0f }, // ����
	{ -0.5f, +0.5f, 0.0f }, // ����
	{ +0.5f, -0.5f, 0.0f }, // �E��
	};;

	SpriteManager* spriteManager_ = nullptr;

	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	// ���ʊm�F
	HRESULT result;
};

