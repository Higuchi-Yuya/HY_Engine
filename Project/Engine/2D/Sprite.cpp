#include "Sprite.h"
#include "WinApp.h"

SpriteManager* Sprite::sSpriteManager_ = nullptr;

void Sprite::Initialize(Texture* textureNum, Vector2 position, Vector2 size, Vector4 color)
{
	// ���̉摜�̃T�C�Y�ɓK��
	if (textureNum != nullptr) {
		textureIndex_ = textureNum;
		AbjustTextureSize();
		//�e�N�X�`���T�C�Y���X�v���C�g�̃T�C�Y�ɓK�p
		size_ = textureSize_;
	}

	// �����Ŏ����Ă������̂���
	position_.x = position.x;
	position_.y = position.y;
	size_ = size;
	color_ = color;

	// ���_�f�[�^

	// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	uint32_t sizeVB = static_cast<UINT>(sizeof(vertices_[0]) * _countof(vertices_));

	// ���_�o�b�t�@�̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp{}; // �q�[�v�ݒ�
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPU�ւ̓]���p
	// ���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB; // ���_�f�[�^�S�̂̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// ���_�o�b�t�@�̐���
	result = sSpriteManager_->sDxcommon_->GetDevice()->CreateCommittedResource(
		&heapProp, // �q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc, // ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff_));
	assert(SUCCEEDED(result));

	// GPU��̃o�b�t�@�ɑΉ��������z������(���C����������)���擾

	result = vertBuff_->Map(0, nullptr, (void**)&vertMap_);
	assert(SUCCEEDED(result));
	// �S���_�ɑ΂���
	for (int i = 0; i < _countof(vertices_); i++) {
		vertMap_[i] = vertices_[i]; // ���W���R�s�[
	}
	// �q���������
	vertBuff_->Unmap(0, nullptr);

	// ���_�o�b�t�@�r���[�̍쐬
	
	// GPU���z�A�h���X
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	// ���_�o�b�t�@�̃T�C�Y
	vbView_.SizeInBytes = sizeVB;
	// ���_1���̃f�[�^�T�C�Y
	vbView_.StrideInBytes = sizeof(vertices_[0]);

	//�萔�o�b�t�@�̐ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(SpriteManager::ConstBufferData) + 0xff) & ~0xff;
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//�萔�o�b�t�@����
	result = sSpriteManager_->sDxcommon_->GetDevice()->CreateCommittedResource(
		&cbHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));

	
	result = constBuff_->Map(0, nullptr, (void**)&constMap_);
	assert(SUCCEEDED(result));

	constMap_->color = Vector4(1, 0, 0, 1.0f);
	
	// �P�ʍs�����
	constMap_->mat.identity();

	// ���W�ϊ�
	constMap_->mat.m[0][0] = 2.0f / WinApp::window_width;
	constMap_->mat.m[1][1] = -2.0f / WinApp::window_height;
	constMap_->mat.m[3][0] = -1.0f;
	constMap_->mat.m[3][1] = 1.0f;
}

Sprite::Sprite()
{

}

Sprite::Sprite(Texture textureNum, Vector2 position, Vector2 size, Vector4 color, Vector2 anchorPoint, bool isFlipX, bool isFlipY)
{
	textureIndex_ = &textureNum;
	position_ = position;
	size_ = size;
	color_ = color;
	anchorPoint_ = anchorPoint;
	isFlipX_ = isFlipX;
	isFlipY_ = isFlipY;
}

void Sprite::StaticInitialize(SpriteManager* spriteManager)
{
	sSpriteManager_ = spriteManager;
}

void Sprite::Updata(WorldTransform* parent)
{
	//���_�f�[�^�������o�ϐ��Ōv�Z
	float left = (0.0f - anchorPoint_.x) * size_.x;
	float right = (1.0f - anchorPoint_.x) * size_.x;
	float top = (0.0f - anchorPoint_.y) * size_.y;
	float bottom = (1.0f - anchorPoint_.y) * size_.y;

	// ���E���]
	if (isFlipX_) {
		left = -left;
		right = -right;
	}

	// �㉺���]
	if (isFlipY_) {
		top = -top;
		bottom = -bottom;
	}

	// ���_�f�[�^�̐ݒ�
	vertices_[LB].pos = { left,bottom,0.0f };
	vertices_[LT].pos = { left,top,0.0f };
	vertices_[RB].pos = { right,bottom,0.0f };
	vertices_[RT].pos = { right,top,0.0f };

	//�e�N�X�`���o�b�t�@�擾
	ID3D12Resource* textureBuffer = textureIndex_->buffer.Get();

	//�w��ԍ��̃e�N�X�`�����ǂݍ��ݍς݂Ȃ�
	if (textureBuffer) {
		//�e�N�X�`�����擾
		D3D12_RESOURCE_DESC resDesc = textureBuffer->GetDesc();

		float tex_left = textureLeftTop_.x / resDesc.Width;
		float tex_right = (textureSize_.x) / resDesc.Width;
		float tex_top = textureLeftTop_.y / resDesc.Height;
		float tex_bottom = (textureSize_.y) / resDesc.Height;

		//���_��UV�ɔ��f
		vertices_[LB].uv = { tex_left,tex_bottom };
		vertices_[LT].uv = { tex_left,tex_top };
		vertices_[RB].uv = { tex_right,tex_bottom };
		vertices_[RT].uv = { tex_right,tex_top };
	}

	// ���_�f�[�^��GPU�ɓ]��
	std::copy(std::begin(vertices_), std::end(vertices_), vertMap_);

	// �s��̐ݒ�
	Matrix4 matScale;
	Matrix4 matRot;
	Matrix4 matTrans;

	Matrix4 matResult;

	matScale.identity();
	matRot.identity();
	matTrans.identity();

	matScale.scale(scale_);
	matRot.rotation(rotation_);
	matTrans.translate({ position_.x,position_.y,0.0f });

	Matrix4 matProjection;
	matProjection.identity();
	matProjection.m[0][0] = 2.0f / WinApp::window_width;
	matProjection.m[1][1] = -2.0f / WinApp::window_height;
	matProjection.m[3][0] = -1.0f;
	matProjection.m[3][1] = 1.0f;

	// �s��̊|���Z
	matWorld_.identity();
	matWorld_ *= matScale;
	matWorld_ *= matRot;
	matWorld_ *= matTrans;
	// �e�s�񂪂������ꍇ
	if (parent != nullptr)
	{
		//parent->matWorld_ *= matProjection;
		matWorld_ *= parent->matWorld_;
	}
	matResult = matWorld_ * matProjection;



	// �s��̓]��
	constMap_->mat = matResult;

	// �F��]��
	constMap_->color = color_;
}

void Sprite::Draw()
{
	//��\���Ȃ珈���I��
	if (isInvisible_) {
		return;
	}

	sSpriteManager_->SetTextureCommands(textureIndex_);

	Updata();

	// ���_�o�b�t�@�r���[�̐ݒ�R�}���h
	sSpriteManager_->sDxcommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vbView_);

	// �萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	sSpriteManager_->sDxcommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuff_->GetGPUVirtualAddress());

	// �`��R�}���h
	sSpriteManager_->sDxcommon_->GetCommandList()->DrawInstanced(_countof(vertices_), 1, 0, 0); // �S�Ă̒��_���g���ĕ`��
}

void Sprite::AbjustTextureSize()
{
	ID3D12Resource* textureBuffer = textureIndex_->buffer.Get();
	assert(textureBuffer);
	
	// �e�N�X�`�����擾
	D3D12_RESOURCE_DESC resDesc = textureBuffer->GetDesc();

	textureSize_.x = static_cast<float>(resDesc.Width);
	textureSize_.y = static_cast<float>(resDesc.Height);
}
