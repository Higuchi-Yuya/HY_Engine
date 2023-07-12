#include "DirectXCommon.h"
#include <cassert>
#include <d3dx12.h>
#include <thread>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

using namespace Microsoft::WRL;

void DirectXCommon::Initalize(WinApp* winApp)
{
	// NULL���o
	assert(winApp);

	// �����o�ϐ��ɋL�^
	winApp_ = winApp;

	// FPS�Œ菉����
	InitializeFixFPS();

	// �f�o�C�X�̐���
	InitalizeDevice();
	// �R�}���h�֘A�̏�����
	InitialzeCommand();
	// �X���b�v�`�F�[���̏�����
	InitializeSwapchain(winApp);
	// �����_�[�^�[�Q�b�g�r���[�̏�����
	InitializeRenderTargetView();
	// �[�x�o�b�t�@�̏�����
	InitializeDepthBuffer();
	// �t�F���X�̏�����
	InitializeFence();
}

void DirectXCommon::PreDraw()
{
#pragma region �o�b�N�o�b�t�@�̔ԍ��擾
	//�o�b�N�o�b�t�@�̔ԍ����擾(2�Ȃ̂�0�Ԃ�1��)
	uint32_t bbIndex = swapChain_->GetCurrentBackBufferIndex();
#pragma endregion

#pragma region ���\�[�X�o���A�ŏ������݉\�ɕύX
	//���\�[�X�o���A�ŏ������݉\�ɕύX�i�\���p�>�������ݗp�j
	
	barrierDesc_.Transition.pResource = backBuffers_[bbIndex].Get();
	barrierDesc_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrierDesc_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	commandList_->ResourceBarrier(1, &barrierDesc_);
#pragma endregion

#pragma region �`���w��
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += bbIndex * device_->GetDescriptorHandleIncrementSize(rtvHeapDesc_.Type);

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap_->GetCPUDescriptorHandleForHeapStart();
	commandList_->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
#pragma endregion

#pragma region �����_�[�^�[�Q�b�g�N���A
	//��ʃN���A			R	  G	   B	A     �i��ʂ�h��Ԃ��F�j
	FLOAT clearColor[] = { 0.1f,0.25f,0.5f,0.0f };//���ۂ��F
	// �����_�[�^�[�Q�b�g�N���A
	commandList_->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
#pragma endregion

#pragma region �[�x�o�b�t�@�N���A
	commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
#pragma endregion

#pragma region �r���[�|�[�g�̈�̐ݒ�
	// �r���[�|�[�g�ݒ�R�}���h
	D3D12_VIEWPORT viewport{};
	viewport.Width = WinApp::window_width;   //�悱 �ő�1280
	viewport.Height = WinApp::window_height;  //���� �ő�720
	viewport.TopLeftX = 0;  //����X
	viewport.TopLeftY = 0;  //����Y
	viewport.MinDepth = 0.0f; //�ŏ��p�x
	viewport.MaxDepth = 1.0f; //�ő�p�x

	// �r���[�|�[�g�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
	commandList_->RSSetViewports(1, &viewport);

#pragma endregion

#pragma region �V�U�[��`�̐ݒ�
	//�V�U�[��`
	D3D12_RECT scissorRect{};
	scissorRect.left = 0; // �؂蔲�����W��
	scissorRect.right = scissorRect.left + WinApp::window_width; // �؂蔲�����W�E
	scissorRect.top = 0; // �؂蔲�����W��
	scissorRect.bottom = scissorRect.top + WinApp::window_height; // �؂蔲�����W��

	// �V�U�[��`�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
	commandList_->RSSetScissorRects(1, &scissorRect);
#pragma endregion

}

void DirectXCommon::PostDraw()
{
#pragma region �o�b�N�o�b�t�@�̔ԍ����擾
	//�o�b�N�o�b�t�@�̔ԍ����擾(2�Ȃ̂�0�Ԃ�1��)
	uint32_t bbIndex = swapChain_->GetCurrentBackBufferIndex();
#pragma endregion

#pragma region ���\�[�X�o���A�ŕ\����ԂɕύX
	//5.���\�[�X�o���A��߂�
	barrierDesc_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;//�`���Ԃ���
	barrierDesc_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;//�\����Ԃ�
	commandList_->ResourceBarrier(1, &barrierDesc_);
#pragma endregion

#pragma region �O���t�B�b�N�X�R�}���h���N���[�Y
	//���߂̃N���[�Y
	result = commandList_->Close();
	assert(SUCCEEDED(result));
#pragma endregion

#pragma region GPU�R�}���h�̎��s
	//�R�}���h���X�g�̎��s
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);
#pragma endregion
	
#pragma region ��ʃt���b�v
	//��ʂɕ\������o�b�t�@���t���b�v(���\�̓���ւ�)
	result = swapChain_->Present(1, 0);
	assert(SUCCEEDED(result));
#pragma endregion

#pragma region �R�}���h�����҂�
	//�R�}���h�̎��s������҂�
	commandQueue_->Signal(fence_.Get(), ++fenceVal_);
	if (fence_->GetCompletedValue() != fenceVal_) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence_->SetEventOnCompletion(fenceVal_, event);
		if (event != 0)
		{
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}

	}
#pragma endregion

	// FPS�Œ�
	UpdateFixFPS();

#pragma region �R�}���h�A���P�[�^�[�̃��Z�b�g
	//�L���[���N���A
	result = commandAllocator_->Reset();
	assert(SUCCEEDED(result));
#pragma endregion

#pragma region �R�}���h���X�g���Z�b�g
	//�ĂуR�}���h���X�g�𒙂߂鏀��
	result = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(result));
#pragma endregion



}

void DirectXCommon::ClearDepthBuffer()
{
	// �[�x�X�e���V���r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvH =
		CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvHeap_->GetCPUDescriptorHandleForHeapStart());
	// �[�x�o�b�t�@�̃N���A
	commandList_->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void DirectXCommon::InitalizeDevice()
{
	
#ifdef _DEBUG
	//�f�o�b�O���C���[���I����
	ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
	}
#endif

#pragma region �A�_�v�^�[�̗�
	//DXGI�t�@�N�g���[�̐���
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(result));

	//�A�_�v�^�[�̗񋓗p
	std::vector<ComPtr<IDXGIAdapter4>>adapters;
	//�����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	IDXGIAdapter4* tmpAdapter = nullptr;

	//�p�t�H�[�}���X�����������珇�ɁA�S�ẴA�_�v�^�[��񋓂���
	for (uint32_t i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND; i++) {
		//���I�z��ɒǉ�����
		adapters.push_back(tmpAdapter);
	}
#pragma endregion

#pragma region �A�_�v�^�[�̑I��
	//�Ó��ȃA�_�v�^��I�ʂ���
	for (size_t i = 0; i < adapters.size(); i++) {
		DXGI_ADAPTER_DESC3 adapterDesc;
		//�A�_�v�^�[�̏����擾����
		adapters[i]->GetDesc3(&adapterDesc);

		//�\�t�g�E�F�A�f�o�C�X�����
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//�f�o�C�X���̗p���ă��[�v�𔲂���
			tmpAdapter = adapters[i].Get();
			break;
		}
	}
#pragma endregion

#pragma region �f�o�C�X����
	//�Ή����x���̔z��
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	for (size_t i = 0; i < _countof(levels); i++) {
		//�̗p�����A�_�v�^�[�Ńf�o�C�X�𐶐�
		result = D3D12CreateDevice(tmpAdapter, levels[i], IID_PPV_ARGS(&device_));
		if (result == S_OK) {
			//�f�o�C�X�𐶐��ł������_�Ń��[�v�𔲂���
			featureLevel = levels[i];
			break;
		}
	}
#pragma endregion

#pragma region �G���[���f����
	assert(SUCCEEDED(result));

	ComPtr<ID3D12InfoQueue> infoQuene;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQuene))))
	{
		infoQuene->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);    // ��΂��G���[�ꎞ�Ɏ~�܂�
		infoQuene->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);        // �G���[���Ɏ~�܂�
		infoQuene->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);    // ���[�j���O���Ɏ~�܂�
	}
#pragma endregion

}

void DirectXCommon::InitialzeCommand()
{

#pragma region �R�}���h�A���P�[�^�̐���
	//�R�}���h�A���[�P�[�^�𐶐�
	result = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	assert(SUCCEEDED(result));
#pragma endregion

#pragma region �R�}���h���X�g�̐���
	//�R�}���h���X�g�𐶐�
	result = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
	assert(SUCCEEDED(result));
#pragma endregion

#pragma region �R�}���h�L���[�̐���
	//�R�}���h�L���[�̐ݒ�
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

	//�R�}���h�L���[�𐶐�
	result = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(result));
#pragma endregion

}

void DirectXCommon::InitializeSwapchain(WinApp* winApp)
{
#pragma region �X���b�v�`�F�[���̐ݒ�
	//�X���b�v�`�F�[���̐ݒ�
	
	swapChainDesc_.Width = 1280;
	swapChainDesc_.Height = 720;
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;			//�F���̏���
	swapChainDesc_.SampleDesc.Count = 1;							//�}���`�T���v�����Ȃ�
	swapChainDesc_.BufferUsage = DXGI_USAGE_BACK_BUFFER;			//�o�b�N�o�b�t�@�p
	swapChainDesc_.BufferCount = 2;								//�o�b�t�@����2�ɐݒ�
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	//�t���b�v��͔j��
	swapChainDesc_.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
#pragma endregion

#pragma region �X���b�v�`�F�[���̐���
	//�X���b�v�`�F�[���̐���
	ComPtr<IDXGISwapChain1> swapChain1;

	result = dxgiFactory_->CreateSwapChainForHwnd(
		commandQueue_.Get(),
		winApp->GetHwnd(),
		&swapChainDesc_,
		nullptr,
		nullptr,
		&swapChain1);
#pragma endregion
	
#pragma region IDXGISwapChain1�̃I�u�W�F�N�g��IDXGISwapChain4�ɕϊ�����
	//������IDXGISwapChain1�̃I�u�W�F�N�g��IDXGISwapChain4�ɕϊ�����
	swapChain1.As(&swapChain_);

	assert(SUCCEEDED(result));
#pragma endregion

}

void DirectXCommon::InitializeRenderTargetView()
{

#pragma region RTV�p�f�X�N���v�^�q�[�v����
	//�f�X�N���v�^�q�[�v�̐ݒ�
	
	rtvHeapDesc_.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;			//�����_�[�^�[�Q�b�g�r���[
	rtvHeapDesc_.NumDescriptors = swapChainDesc_.BufferCount;		//���\��2��

	//�f�X�N���v�^�q�[�v�̐���
	device_->CreateDescriptorHeap(&rtvHeapDesc_, IID_PPV_ARGS(&rtvHeap_));

	//�o�b�N�o�b�t�@
	backBuffers_.resize(swapChainDesc_.BufferCount);

	
#pragma endregion

#pragma region �����_�[�^�[�Q�b�g�r���[�̐���
	// ���\�̂Q��
	//�X���b�v�`�F�[���̑S�Ẵo�b�t�@�ɂ��ď�������
	for (size_t i = 0; i < backBuffers_.size(); i++) {
		//�X���b�v�`�F�[������o�b�t�@���擾
		swapChain_->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffers_[i]));
		//�f�X�N���v�^�q�[�v�̃n���h�����擾
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
		//�����\���ŃA�h���X�������
		rtvHandle.ptr += i * device_->GetDescriptorHandleIncrementSize(rtvHeapDesc_.Type);
		//�����_�[�^�[�Q�b�g�r���[�̐ݒ�
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		//�V�F�[�_�[�̌v�Z���ʂ�SRGB�ɕϊ����ď�������
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		//�����_�[�^�[�Q�b�g�r���[�̐���
		device_->CreateRenderTargetView(backBuffers_[i].Get(), &rtvDesc, rtvHandle);
	}
#pragma endregion

}

void DirectXCommon::InitializeDepthBuffer()
{
#pragma region �[�x�o�b�t�@���\�[�X�ݒ�
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC depthResoureDesc{};
	depthResoureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResoureDesc.Width = WinApp::window_width;
	depthResoureDesc.Height = WinApp::window_height;
	depthResoureDesc.DepthOrArraySize = 1;
	depthResoureDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthResoureDesc.SampleDesc.Count = 1;
	depthResoureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	//�[�x�l�p�q�[�v�v���p�e�B
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//�[�x�l�̃N���A�ݒ�
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;
#pragma endregion

#pragma region �[�x�o�b�t�@�̐���
	//���\�[�X����
	depthBuff_ = nullptr;
	result = device_->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResoureDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&depthBuff_));
#pragma endregion

#pragma region DSV�p�f�X�N���v�^�q�[�v�̐���
	//�[�x�r���[�p�f�X�N���v�^�q�[�v�쐬
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	
	result = device_->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap_));
#pragma endregion

#pragma region �[�x�o�b�t�@�r���[�iDSV�j�̐���
	//�[�x�r���[�쐬
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device_->CreateDepthStencilView(
		depthBuff_.Get(),
		&dsvDesc,
		dsvHeap_->GetCPUDescriptorHandleForHeapStart()
	);
#pragma endregion

}

void DirectXCommon::InitializeFence()
{
	//�t�F���X�̐���
	result = device_->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
}

void DirectXCommon::InitializeFixFPS()
{
	// ���ݎ��Ԃ��L�^����
	reference_ = std::chrono::steady_clock::now();

}

void DirectXCommon::UpdateFixFPS()
{
	// 1/60�b�҂�����̎���
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));

	// 1/60�b���킸���ɒZ������
	const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));

	// ���ݎ��Ԃ��擾����
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	// �O��L�^����̌o�ߎ��Ԃ��擾����
	std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	// 1/60�b�i���킸���ɒZ�����ԁj�o���Ă��Ȃ��ꍇ
	if (elapsed < kMinCheckTime) {
		// 1/60�b�o�߂���܂Ŕ����ȃX���[�v���J��Ԃ�
		while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
			// 1�}�C�N���b�X���[�v
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}

	// ���ݎ��Ԃ��L�^����
	reference_ = std::chrono::steady_clock::now();
}
