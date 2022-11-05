#include "Line.h"
#include "Render_basic.h"

void Line::Line_Initialize(ID3D12Device* device, Mesh::Vertex* vertices, unsigned short* indices, int vertices_count,int indices_count)
{
	TexMetadata metadata{};
	ScratchImage scratchImg{};

	// WIC�e�N�X�`���̃��[�h
	result = LoadFromWICFile(
		L"Resources/risu.jpg",   //�uResources�v�t�H���_�́utexture.png�vrisu.jpg
		WIC_FLAGS_NONE,
		&metadata, scratchImg);
	assert(SUCCEEDED(result));

	ScratchImage mipChain{};

	// �~�b�v�}�b�v����
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}
	assert(SUCCEEDED(result));


	// �ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	metadata.format = MakeSRGB(metadata.format);

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
	Mesh::Vertex* vertMap = nullptr;
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

	// CBV,SRV,UAV��1���̃T�C�Y���擾
	incrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// �n���h����1�i�߂�iSRV�̈ʒu�j
	srvHandle.ptr += incrementSize;

	// �V�F�[�_���\�[�X�r���[�ݒ�
	



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
	cbResourceDesc.Width = (sizeof(Mesh::ConstBufferDataMaterial) + 0xff) & ~0xff;   // 256�o�C�g�A���C�������g
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

	constMapMaterial->color = { 1.0f,0.0f, 0.0f, 1.0f };

	// �q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES heapProp2{};
	heapProp2.Type = D3D12_HEAP_TYPE_UPLOAD;                   // GPU�ւ̓]���p
	// ���\�[�X�ݒ�
	D3D12_RESOURCE_DESC redesc{};
	redesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	redesc.Width = (sizeof(Mesh::ConstBufferDataTransform) + 0xff) & ~0xff;   // 256�o�C�g�A���C�������g
	redesc.Height = 1;
	redesc.DepthOrArraySize = 1;
	redesc.MipLevels = 1;
	redesc.SampleDesc.Count = 1;
	redesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// �萔�o�b�t�@�̐��� 0��
	result = device->CreateCommittedResource(
		&heapProp2, // �q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&redesc, // ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&object3d.constBuffTransform));
	assert(SUCCEEDED(result));

	// �萔�o�b�t�@�̃}�b�s���O 0��
	result = object3d.constBuffTransform->Map(0, nullptr, (void**)&object3d.constMapTransform); // �}�b�s���O
	assert(SUCCEEDED(result));
#pragma endregion
}

void Line::Line_Updata()
{
	XMMATRIX matScale, matRot, matTrans;

	// �X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = XMMatrixScaling(object3d.scale.x, object3d.scale.y, object3d.scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(object3d.rotation.z);
	matRot *= XMMatrixRotationX(object3d.rotation.x);
	matRot *= XMMatrixRotationY(object3d.rotation.y);
	matTrans = XMMatrixTranslation(object3d.position.x, object3d.position.y, object3d.position.z);

	//���[���h�s��̍���
	object3d.matWorld = XMMatrixIdentity();//�ό`�����Z�b�g
	object3d.matWorld *= matScale;
	object3d.matWorld *= matRot;
	object3d.matWorld *= matTrans;

	//�e�I�u�W�F�N�g�������
	if (object3d.parent != nullptr) {
		//�e�I�u�W�F�N�g�̃��[���h�s���������
		object3d.matWorld *= object3d.parent->matWorld;
	}

	object3d.constMapTransform->mat = object3d.matWorld * Mesh::matView * Mesh::matProjection;
}

void Line::Line_Draw(int indices_count, ID3D12GraphicsCommandList* commandList)
{

	//�p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
	commandList->SetPipelineState(Render_basic::GetInstance()->GetPipelineState2());
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

	// �萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	commandList->SetGraphicsRootConstantBufferView(2, object3d.constBuffTransform->GetGPUVirtualAddress());

	// �`��R�}���h
	commandList->DrawIndexedInstanced(indices_count, 1, 0, 0, 0);
}

