#include "Mesh.h"
#include "Render_basic.h"
#include <dinput.h>
#include "Texture.h"


// �E�B���h�E����
static const int window_width = 1280;

// �E�B���h�E�c��
static const int window_height = 720;

XMMATRIX Mesh::matProjection = {};
XMMATRIX Mesh::matView = {};


Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

void Mesh::Mesh_Initialization(ID3D12Device* device, Vertex* vertices, unsigned short* indices, int vertices_count, int indices_count)
{

#pragma region �I�u�W�F�N�g�̕`�揉��������
	//////////////////////////////////////////////////////
	//---------------�`�揉�������� ��������----------------//
	/////////////////////////////////////////////////////


	// ���_�A�C���f�b�N�X�p�J�E���g�錾


	TexMetadata metadata{};
	ScratchImage scratchImg{};

	TexMetadata metadata2{};
	ScratchImage scratchImg2{};

	// WIC�e�N�X�`���̃��[�h
	result = LoadFromWICFile(
		L"Resources/risu.jpg",   //�uResources�v�t�H���_�́utexture.png�vrisu.jpg
		WIC_FLAGS_NONE,
		&metadata, scratchImg);
	assert(SUCCEEDED(result));

	result= LoadFromWICFile(
		L"Resources/reimu.png",   //�uResources�v�t�H���_�́utexture.png�vreimu.png
		WIC_FLAGS_NONE,
		&metadata2, scratchImg2);
	assert(SUCCEEDED(result));

	ScratchImage mipChain;
	ScratchImage mipChain2;

	// �~�b�v�}�b�v����
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}
	assert(SUCCEEDED(result));

	// �~�b�v�}�b�v����
	result = GenerateMipMaps(
		scratchImg2.GetImages(), scratchImg2.GetImageCount(), scratchImg2.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain2);
	if (SUCCEEDED(result)) {
		scratchImg2 = std::move(mipChain2);
		metadata2 = scratchImg2.GetMetadata();
	}
	assert(SUCCEEDED(result));

	// �ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	metadata.format = MakeSRGB(metadata.format);

	// �ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	metadata2.format = MakeSRGB(metadata2.format);

	// �q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = metadata.format;
	textureResourceDesc.Width = metadata.width;
	textureResourceDesc.Height = (UINT)metadata.height;
	textureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	textureResourceDesc.MipLevels = (UINT16)metadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;

	D3D12_RESOURCE_DESC textureResourceDesc2{};
	textureResourceDesc2.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc2.Format = metadata2.format;
	textureResourceDesc2.Width = metadata2.width;
	textureResourceDesc2.Height = (UINT)metadata2.height;
	textureResourceDesc2.DepthOrArraySize = (UINT16)metadata2.arraySize;
	textureResourceDesc2.MipLevels = (UINT16)metadata2.mipLevels;
	textureResourceDesc2.SampleDesc.Count = 1;

	

	// �e�N�X�`���o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff));
	assert(SUCCEEDED(result));

	// �S�~�b�v�}�b�v�ɂ���
	for (size_t i = 0; i < metadata.mipLevels; i++) {
		// �~�b�v�}�b�v���x�����w�肵�ăC���[�W���擾
		const Image* img = scratchImg.GetImage(i, 0, 0);
		// �e�N�X�`���o�b�t�@�Ƀf�[�^�]��
		result = texBuff->WriteToSubresource(
			(UINT)i,
			nullptr,              // �S�̈�փR�s�[
			img->pixels,          // ���f�[�^�A�h���X
			(UINT)img->rowPitch,  // 1���C���T�C�Y
			(UINT)img->slicePitch // 1���T�C�Y
		);
		assert(SUCCEEDED(result));
	}

	// �e�N�X�`���o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc2,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff2));
	assert(SUCCEEDED(result));

	// �S�~�b�v�}�b�v�ɂ���
	for (size_t i = 0; i < metadata2.mipLevels; i++) {
		// �~�b�v�}�b�v���x�����w�肵�ăC���[�W���擾
		const Image* img = scratchImg2.GetImage(i, 0, 0);
		// �e�N�X�`���o�b�t�@�Ƀf�[�^�]��
		result = texBuff2->WriteToSubresource(
			(UINT)i,
			nullptr,              // �S�̈�փR�s�[
			img->pixels,          // ���f�[�^�A�h���X
			(UINT)img->rowPitch,  // 1���C���T�C�Y
			(UINT)img->slicePitch // 1���T�C�Y
		);
		assert(SUCCEEDED(result));
	}

	// SRV�̍ő��
	const size_t kMaxSRVCount = 2056;

	// �f�X�N���v�^�q�[�v�̐ݒ�
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_���猩����悤��
	srvHeapDesc.NumDescriptors = kMaxSRVCount;

	// �ݒ������SRV�p�f�X�N���v�^�q�[�v�𐶐�
	result = device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(srvHeap.GetAddressOf()));
	assert(SUCCEEDED(result));

	
	// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * vertices_count);
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
	
	result = device->CreateCommittedResource(
		&heapProp, // �q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc, // ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	// �C���f�b�N�X�f�[�^�S�̂̃T�C�Y
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * indices_count);

	// ���\�[�X�ݒ�
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB; // �C���f�b�N�X��񂪓��镪�̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// �C���f�b�N�X�o�b�t�@�̐���
	
	result = device->CreateCommittedResource(
		&heapProp, // �q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc, // ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	//�񖇖ڂ̉摜�̃��\�[�X�f�X�N
	D3D12_RESOURCE_DESC resDesc2{};
	resDesc2.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc2.Width = sizeVB; // ���_�f�[�^�S�̂̃T�C�Y
	resDesc2.Height = 1;
	resDesc2.DepthOrArraySize = 1;
	resDesc2.MipLevels = 1;
	resDesc2.SampleDesc.Count = 1;
	resDesc2.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	result = device->CreateCommittedResource(
		&heapProp, // �q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc2, // ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	resDesc2.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc2.Width = sizeIB; // �C���f�b�N�X��񂪓��镪�̃T�C�Y
	resDesc2.Height = 1;
	resDesc2.DepthOrArraySize = 1;
	resDesc2.MipLevels = 1;
	resDesc2.SampleDesc.Count = 1;
	resDesc2.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	result = device->CreateCommittedResource(
		&heapProp, // �q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc2, // ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	// �C���f�b�N�X�o�b�t�@���}�b�s���O
	uint16_t* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	// �S�C���f�b�N�X�ɑ΂���
	for (int i = 0; i < indices_count; i++)
	{
		indexMap[i] = indices[i];   // �C���f�b�N�X���R�s�[
	}
	// �}�b�s���O����
	indexBuff->Unmap(0, nullptr);

	// �C���f�b�N�X�o�b�t�@�r���[�̍쐬
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	// GPU��̃o�b�t�@�ɑΉ��������z������(���C����������)���擾
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));

	// �S���_�ɑ΂���
	for (int i = 0; i < vertices_count; i++) {
		vertMap[i] = vertices[i]; // ���W���R�s�[
	}
	// �q���������
	vertBuff->Unmap(0, nullptr);

	
	// GPU���z�A�h���X
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	// ���_�o�b�t�@�̃T�C�Y
	vbView.SizeInBytes = sizeVB;
	// ���_1���̃f�[�^�T�C�Y
	/*vbView.StrideInBytes = sizeof(XMFLOAT3);*/
	vbView.StrideInBytes = sizeof(vertices[0]);

	//SRV�q�[�v�̐擪�n���h�����擾
	srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
	srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();

	// �V�F�[�_���\�[�X�r���[�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = textureResourceDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = textureResourceDesc.MipLevels;

	// �n���h���̎w���ʒu�ɃV�F�[�_�[���\�[�X�r���[�쐬
	device->CreateShaderResourceView(texBuff.Get(), &srvDesc, srvHandle);

	//�񖇖ڂ̉摜�ɕK�v�Ȑݒ�

	// CBV,SRV,UAV��1���̃T�C�Y���擾
	incrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// �n���h����1�i�߂�iSRV�̈ʒu�j
	srvHandle.ptr += incrementSize;

	// �V�F�[�_���\�[�X�r���[�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.Format = textureResourceDesc2.Format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc2.Texture2D.MipLevels = textureResourceDesc2.MipLevels;

	// �n���h���̎w���ʒu�ɃV�F�[�_�[���\�[�X�r���[�쐬
	device->CreateShaderResourceView(texBuff2.Get(), &srvDesc2, srvHandle);

	srvHandle.ptr += incrementSize;

	// CBV(�R���X�^���g�o�b�t�@�r���[)�̐ݒ�
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	//cbvDesc�̒l�ݒ�i�ȗ��j
	// �萔�o�b�t�@�r���[����
	device->CreateConstantBufferView(&cbvDesc, srvHandle);



	// �萔�o�b�t�@�p�f�[�^�\����Pos�i�}�e���A���j
	struct ConstBufferDataMaterialPos {
		XMFLOAT4 Move; // �ړ�
	};

	//Creation<ConstBufferDataMaterial>();
	// �q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;                   // GPU�ւ̓]���p
	// ���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff;   // 256�o�C�g�A���C�������g
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	
	
	// �萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&cbHeapProp, // �q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc, // ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial));
	assert(SUCCEEDED(result));

	// �萔�o�b�t�@�̃}�b�s���O
	
	result = constBuffMaterial.Get()->Map(0, nullptr, (void**)&constMapMaterial); // �}�b�s���O
	assert(SUCCEEDED(result));

	


	//�s��萔�o�b�t�@
	{
		for (int i = 0; i < _countof(object3ds); i++)
		{
			//������
			InitializeObject3d(& object3ds[i], device);

			//�������火�͐e�q�\���̃T���v��
			//�擪�ȊO�Ȃ�
			if (i > 0) {
				//�ЂƂO�̃I�u�W�F�N�g��e�I�u�W�F�N�g�Ƃ���
				object3ds[i].parent = &object3ds[i - 1];
				//�e�I�u�W�F�N�g�̂X���̑傫��
				object3ds[i].scale = { 0.9f,0.9f,0.9f };
				//�e�I�u�W�F�N�g�ɑ΂���z������ɂR�O�x��]
				object3ds[i].rotation = { 0.0f,0.0f,XMConvertToRadians(30.0f) };
				//�e�I�u�W�F�N�g�ɑ΂���z����-�W�D�O���炷
				object3ds[i].position = { 0.0f,0.0f,-8.0f };
			}
		}

	}
	
	//���[���h�ϊ��s��
	matWorld = XMMatrixIdentity();

	//�ˉe�ϊ��s��
	matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),
		(float)window_width / window_height,
		0.1f, 1000.0f);

	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

#pragma endregion

}

void Mesh::Mesh_Update(Input *input)
{

#pragma region �J�����̏���

	if (input->PushKey(DIK_D) || input->PushKey(DIK_A))
	{
		if (input->PushKey(DIK_D)) { angle.x -= XMConvertToRadians(1.0f); }
		else if (input->PushKey(DIK_A)) { angle.x += XMConvertToRadians(1.0f); }
	}
	if (input->PushKey(DIK_W) || input->PushKey(DIK_S))
	{
		if (input->PushKey(DIK_W)) { angle.y += XMConvertToRadians(1.0f); }
		else if (input->PushKey(DIK_S)) { angle.y -= XMConvertToRadians(1.0f); }
	}

	eye.x = dis * cosf(angle.x) * cosf(angle.y);
	eye.y = -dis * sinf(angle.y);
	eye.z = dis * sinf(angle.x) * cosf(angle.y);

	up.y = cosf(angle.y);

	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

#pragma endregion

#pragma region �ړ��n����

	//�����ꂩ�̃L�[�������Ă�����e��������
	if (input->PushKey(DIK_UP) || input->PushKey(DIK_DOWN) || input->PushKey(DIK_RIGHT) || input->PushKey(DIK_LEFT))
	{

		//���W���ړ����鏈��
		if (input->PushKey(DIK_UP)) { object3ds[0].position.y += 1.0f; }
		else if (input->PushKey(DIK_DOWN)) { object3ds[0].position.y -= 1.0f; }
		if (input->PushKey(DIK_RIGHT)) { object3ds[0].position.x += 1.0f; }
		else if (input->PushKey(DIK_LEFT)) { object3ds[0].position.x -= 1.0f; }
	}

	if (input->PushKey(DIK_R) || input->PushKey(DIK_T))
	{
		// ��]���鏈��Y��
		if (input->PushKey(DIK_R)) { object3ds[0].rotation.y += 0.05f; }
		else if (input->PushKey(DIK_T)) { object3ds[0].rotation.y -= 0.05f; }
	}
	if (input->PushKey(DIK_F) || input->PushKey(DIK_G))
	{
		// ��]���鏈��
		if (input->PushKey(DIK_F)) { object3ds[0].rotation.z += 0.1f; }
		else if (input->PushKey(DIK_G)) { object3ds[0].rotation.z -= 0.1f; }
	}

	if (input->PushKey(DIK_V) || input->PushKey(DIK_B))
	{
		// ��]���鏈��
		if (input->PushKey(DIK_V)) { object3ds[0].rotation.x += 0.05f; }
		else if (input->PushKey(DIK_B)) { object3ds[0].rotation.x -= 0.05f; }
	}

	//�s��̍���
	for (size_t i = 0; i < _countof(object3ds); i++)
	{
		UpdateObject3d(&object3ds[i], matView, matProjection);
	}

#pragma endregion
	
#pragma region �摜�؂�ւ�����
	if (input->PushKey(DIK_1)|| input->PushKey(DIK_2))
	{
		if(input->PushKey(DIK_1)){ srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart(); }
		if(input->PushKey(DIK_2)){ srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
						srvGpuHandle.ptr += incrementSize;}
	}
#pragma endregion

#pragma region �F�̐؂�ւ�����
	float speed = 0.01;

	if (R >= 1 && B <= 0) { R = 1;  B = 0; G += speed; }
	if (G >= 1 && B <= 0) { G = 1;  B = 0; R -= speed; }
	if (R <= 0 && G >= 1) { R = 0;  G = 1; B += speed; }
	if (R <= 0 && B >= 1) { R = 0;  B = 1; G -= speed; }
	if (G <= 0 && B >= 1) { G = 0;  B = 1; R += speed; }
	if (R >= 1 && G <= 0) { R = 1;  G = 0; B -= speed; }

	if (A >= 1) annihilation_flag = true;
	if (A <= 0) annihilation_flag = false;
	if (annihilation_flag == true) A -= speed;
	if (annihilation_flag == false) A += speed;

	constMapMaterial->color = XMFLOAT4(R, G, B, A);
#pragma endregion

}

void Mesh::Mesh_Draw(ID3D12Device* device, int indices_count, ID3D12GraphicsCommandList* commandList)
{
	//�p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
	commandList->SetPipelineState(Render_basic::GetInstance()->GetPipelineState());
	commandList->SetGraphicsRootSignature(Render_basic::GetInstance()->GetRootSignature());

	// �v���~�e�B�u�`��̐ݒ�R�}���h
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // �O�p�`���X�g

	// �萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial.Get()->GetGPUVirtualAddress());

	// SRV�q�[�v�̐ݒ�R�}���h
	commandList->SetDescriptorHeaps(1, srvHeap.GetAddressOf());

	// SRV�q�[�v�̐擪�n���h�����擾�iSRV���w���Ă���͂�


	// SRV�q�[�v�̐擪�ɂ���SRV�����[�g�p�����[�^1�Ԃɐݒ�
	commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

	//�S�I�u�W�F�N�g�ɂ��ď���
	for (int i = 0; i < _countof(object3ds); i++)
	{
		DrawObject3d(&object3ds[i], commandList, vbView, ibView, indices_count);
	}
}

void Mesh::InitializeObject3d(Object3d* object, ID3D12Device* device)
{
	// �q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;                   // GPU�ւ̓]���p
	// ���\�[�X�ݒ�
	D3D12_RESOURCE_DESC redesc{};
	redesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	redesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;   // 256�o�C�g�A���C�������g
	redesc.Height = 1;
	redesc.DepthOrArraySize = 1;
	redesc.MipLevels = 1;
	redesc.SampleDesc.Count = 1;
	redesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// �萔�o�b�t�@�̐��� 0��
	result = device->CreateCommittedResource(
		&heapProp, // �q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&redesc, // ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&object->constBuffTransform));
	assert(SUCCEEDED(result));

	// �萔�o�b�t�@�̃}�b�s���O 0��
	result = object->constBuffTransform->Map(0, nullptr, (void**)&object->constMapTransform); // �}�b�s���O
	assert(SUCCEEDED(result));

}

void Mesh::UpdateObject3d(Object3d* object, XMMATRIX& matView, XMMATRIX& matProjection)
{
	XMMATRIX matScale, matRot, matTrans;

	// �X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = XMMatrixScaling(object->scale.x, object->scale.y, object->scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(object->rotation.z);
	matRot *= XMMatrixRotationX(object->rotation.x);
	matRot *= XMMatrixRotationY(object->rotation.y);
	matTrans = XMMatrixTranslation(object->position.x, object->position.y, object->position.z);

	//���[���h�s��̍���
	object->matWorld = XMMatrixIdentity();//�ό`�����Z�b�g
	object->matWorld *= matScale;
	object->matWorld *= matRot;
	object->matWorld *= matTrans;

	//�e�I�u�W�F�N�g�������
	if (object->parent != nullptr) {
		//�e�I�u�W�F�N�g�̃��[���h�s���������
		object->matWorld *= object->parent->matWorld;
	}

	object->constMapTransform->mat = object->matWorld * matView * matProjection;
}

void Mesh::DrawObject3d(Object3d* object, ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW& vdView, D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices)
{
	//���_�o�b�t�@�̐ݒ�
	commandList->IASetVertexBuffers(0, 1, &vbView);
	// �C���f�b�N�X�o�b�t�@�r���[�̐ݒ�R�}���h
	commandList->IASetIndexBuffer(&ibView);
	// �萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	commandList->SetGraphicsRootConstantBufferView(2, object->constBuffTransform->GetGPUVirtualAddress());

	// �`��R�}���h
	commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
}

void Mesh::Mesh_InitializeLine_Line(ID3D12Device* device, Vertex2* vertices_,int vertices_count)
{
	//���_�f�[�^�S�̂̃T�C�Y�����_�f�[�^����̃T�C�Y�����_�f�[�^�̗v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * vertices_count);


	//���_�o�b�t�@�̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp{};  //�q�[�v�ݒ�
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;  //GPU�ւ̓]���p
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB; //���_�f�[�^�S�̂̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//���_�o�b�t�@�̐���
	ID3D12Resource* vertBuff = nullptr;

	result = device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	assert(SUCCEEDED(result));


	//GPU��̃o�b�t�@�ɑΉ��������z�������i���C����������j���擾....1
	XMFLOAT3* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));

	//�S���_�ɑ΂���
	for (int i = 0; i < vertices_count; i++) {
		vertMap[i] = vertices_[i].pos;  //���W���R�s�[
	}


	//�q���������
	vertBuff->Unmap(0, nullptr);

	//���_�o�b�t�@�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//GPU���z�A�h���X
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//���_�o�b�t�@�̃T�C�Y
	vbView.SizeInBytes = sizeVB;
	//���_����̃f�[�^�T�C�Y
	vbView.StrideInBytes = sizeof(XMFLOAT3);

	ID3DBlob* vsBlob = nullptr; // ���_�V�F�[�_�I�u�W�F�N�g
	ID3DBlob* psBlob = nullptr; // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ID3DBlob* errorBlob = nullptr; // �G���[�I�u�W�F�N�g

	

}

void Mesh::Mesh_Draw_Line(int indices_count, ID3D12GraphicsCommandList* commandList)
{
	//�p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
	commandList->SetPipelineState(Render_basic::GetInstance()->GetPipelineState());
	commandList->SetGraphicsRootSignature(Render_basic::GetInstance()->GetRootSignature());

	// �v���~�e�B�u�`��̐ݒ�R�}���h
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST); // ���C�����X�g

	// �萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial.Get()->GetGPUVirtualAddress());

	// SRV�q�[�v�̐ݒ�R�}���h
	commandList->SetDescriptorHeaps(1, srvHeap.GetAddressOf());

	// SRV�q�[�v�̐擪�n���h�����擾�iSRV���w���Ă���͂�


	// SRV�q�[�v�̐擪�ɂ���SRV�����[�g�p�����[�^1�Ԃɐݒ�
	commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

	//���_�o�b�t�@�̐ݒ�
	commandList->IASetVertexBuffers(0, 1, &vbView);

	// �C���f�b�N�X�o�b�t�@�r���[�̐ݒ�R�}���h
	commandList->IASetIndexBuffer(&ibView);


	// �`��R�}���h
	commandList->DrawIndexedInstanced(indices_count, 1, 0, 0, 0);
}
