#include "LightGroup.h"
#include <cassert>
#include <d3dx12.h>
// �ÓI�����o�ϐ��̎���
ID3D12Device* LightGroup::device = nullptr;

void LightGroup::StaticInititalize(ID3D12Device* device)
{
	// �ď������`�F�b�N
	assert(!LightGroup::device);
	// nullptr�`�F�b�N
	assert(device);
	// �ÓI�����o�ϐ��̃Z�b�g
	LightGroup::device = device;
}

LightGroup* LightGroup::Create()
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	LightGroup* instance = new LightGroup();
	// ������
	instance->Initialize();
	// ���������C���X�^���X��Ԃ�
	return instance;
}

void LightGroup::Initialize()
{
	// ���C�g�̃f�t�H���g�̐ݒ�
	DefaultLightSetting();
	// �o�b�t�@�̐���
	CreateConstBuffer();
	// �o�b�t�@�̓]��
	TransferConstBuffer();
}

void LightGroup::Update()
{
	// �l���X�V���������������萔�o�b�t�@�ɓ]������
	if (dirty) {
		TransferConstBuffer();
		dirty = false;
	}
}

void LightGroup::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// �萔�o�b�t�@�r���[���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(4, constBuff->GetGPUVirtualAddress());

}

void LightGroup::TransferConstBuffer()
{
	// ����
	constMap->ambientColor = ambientColor_;

	// ���s����
	for (int i = 0; i < kDirLightNum; i++) {
		// ���C�g���L���Ȃ�ݒ��]��
		if (dirLights_[i].IsActive()) {
			constMap->dirLights[i].active = 1;
			constMap->dirLights[i].lightv = -dirLights_[i].GetLightDir();
			constMap->dirLights[i].lightcolor = dirLights_[i].GetLightColor();
		}
		// ���C�g�������Ȃ烉�C�g�F��0��
		else {
			constMap->dirLights[i].active = 0;
		}
	}
}

void LightGroup::DefaultLightSetting()
{
	dirLights_[0].SetActive(true);
	dirLights_[0].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights_[0].SetLightDir({ 0.0f, -1.0f, 0.0f });

	dirLights_[1].SetActive(false);
	dirLights_[1].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights_[1].SetLightDir({ +0.5f, +0.1f, +0.2f });

	dirLights_[2].SetActive(false);
	dirLights_[2].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights_[2].SetLightDir({ -0.5f, +0.1f, -0.2f });
}

void LightGroup::SetAmbientColor(const Vector3& color)
{
	ambientColor_ = color;
	dirty = true;
}

void LightGroup::SetDirLightActive(int index, bool active)
{
	assert(0 <= index && index < kDirLightNum);

	dirLights_[index].SetActive(active);
}

void LightGroup::SetDirLightDir(int index, const Vector3& lightdir)
{
	assert(0 <= index && index < kDirLightNum);

	dirLights_[index].SetLightDir(lightdir);
	dirty = true;
}

void LightGroup::SetDirLightColor(int index, const Vector3& lightcolor)
{
	assert(0 <= index && index < kDirLightNum);

	dirLights_[index].SetLightColor(lightcolor);
	dirty = true;
}

void LightGroup::CreateConstBuffer()
{
	HRESULT result;

	// �q�[�v�v���p�e�B
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff);

	// �萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&heapProps, // �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff));
	assert(SUCCEEDED(result));

	// �萔�o�b�t�@�Ƃ̃f�[�^�����N
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	assert(SUCCEEDED(result));
}

