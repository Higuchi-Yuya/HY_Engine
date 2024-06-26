#include "DirectXCommon.h"
#include <cassert>
#include <d3dx12.h>
#include <thread>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")


using namespace Microsoft::WRL;

void DirectXCommon::Initalize(WinApp* winApp)
{
	// NULL検出
	assert(winApp);

	// メンバ変数に記録
	winApp_ = winApp;

	// FPS固定初期化
	InitializeFixFPS();

	// デバイスの生成
	InitalizeDevice();
	// コマンド関連の初期化
	InitialzeCommand();
	// スワップチェーンの初期化
	InitializeSwapchain(winApp);
	// レンダーターゲットビューの初期化
	InitializeRenderTargetView();
	// 深度バッファの初期化
	InitializeDepthBuffer();
	// フェンスの初期化
	InitializeFence();
}

void DirectXCommon::PreDraw()
{
#pragma region バックバッファの番号取得
	//バックバッファの番号を取得(2つなので0番か1番)
	uint32_t bbIndex = swapChain_->GetCurrentBackBufferIndex();
#pragma endregion

#pragma region リソースバリアで書き込み可能に変更
	//リソースバリアで書き込み可能に変更（表示用ｰ>書き込み用）
	
	barrierDesc_.Transition.pResource = backBuffers_[bbIndex].Get();
	barrierDesc_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrierDesc_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	commandList_->ResourceBarrier(1, &barrierDesc_);
#pragma endregion

#pragma region 描画先指定
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += bbIndex * device_->GetDescriptorHandleIncrementSize(rtvHeapDesc_.Type);

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap_->GetCPUDescriptorHandleForHeapStart();
	commandList_->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
#pragma endregion

#pragma region レンダーターゲットクリア
	//画面クリア			R	  G	   B	A     （画面を塗りつぶす色）
	FLOAT clearColor[] = { 0.1f,0.25f,0.5f,0.0f };//青っぽい色
	// レンダーターゲットクリア
	commandList_->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
#pragma endregion

#pragma region 深度バッファクリア
	commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
#pragma endregion

#pragma region ビューポート領域の設定
	// ビューポート設定コマンド
	D3D12_VIEWPORT viewport{};
	viewport.Width = WinApp::window_width;   //よこ 最大1280
	viewport.Height = WinApp::window_height;  //たて 最大720
	viewport.TopLeftX = 0;  //左上X
	viewport.TopLeftY = 0;  //左上Y
	viewport.MinDepth = 0.0f; //最小頻度
	viewport.MaxDepth = 1.0f; //最大頻度

	// ビューポート設定コマンドを、コマンドリストに積む
	commandList_->RSSetViewports(1, &viewport);

#pragma endregion

#pragma region シザー矩形の設定
	//シザー矩形
	D3D12_RECT scissorRect{};
	scissorRect.left = 0; // 切り抜き座標左
	scissorRect.right = scissorRect.left + WinApp::window_width; // 切り抜き座標右
	scissorRect.top = 0; // 切り抜き座標上
	scissorRect.bottom = scissorRect.top + WinApp::window_height; // 切り抜き座標下

	// シザー矩形設定コマンドを、コマンドリストに積む
	commandList_->RSSetScissorRects(1, &scissorRect);
#pragma endregion

}

void DirectXCommon::PostDraw()
{
#pragma region バックバッファの番号を取得
	//バックバッファの番号を取得(2つなので0番か1番)
	//uint32_t bbIndex = swapChain_->GetCurrentBackBufferIndex();
#pragma endregion

#pragma region リソースバリアで表示状態に変更
	//5.リソースバリアを戻す
	barrierDesc_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;//描画状態から
	barrierDesc_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;//表示状態へ
	commandList_->ResourceBarrier(1, &barrierDesc_);
#pragma endregion

#pragma region グラフィックスコマンドをクローズ
	//命令のクローズ
	result = commandList_->Close();
	assert(SUCCEEDED(result));
#pragma endregion

#pragma region GPUコマンドの実行
	//コマンドリストの実行
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);
#pragma endregion
	
#pragma region 画面フリップ
	//画面に表示するバッファをフリップ(裏表の入れ替え)
	result = swapChain_->Present(1, 0);
	assert(SUCCEEDED(result));
#pragma endregion

#pragma region コマンド完了待ち
	//コマンドの実行完了を待つ
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

	// FPS固定
	UpdateFixFPS();

#pragma region コマンドアロケーターのリセット
	//キューをクリア
	result = commandAllocator_->Reset();
	assert(SUCCEEDED(result));
#pragma endregion

#pragma region コマンドリストリセット
	//再びコマンドリストを貯める準備
	result = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(result));
#pragma endregion



}

void DirectXCommon::ClearDepthBuffer()
{
	// 深度ステンシルビュー用デスクリプタヒープのハンドルを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvH =
		CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvHeap_->GetCPUDescriptorHandleForHeapStart());
	// 深度バッファのクリア
	commandList_->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void DirectXCommon::InitalizeDevice()
{
	
#ifdef _DEBUG
	//デバッグレイヤーをオンに
	ComPtr<ID3D12Debug1> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(false);
	}
#endif

#pragma region アダプターの列挙
	//DXGIファクトリーの生成
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(result));

	//アダプターの列挙用
	std::vector<ComPtr<IDXGIAdapter4>>adapters;
	//ここに特定の名前を持つアダプターオブジェクトが入る
	IDXGIAdapter4* tmpAdapter = nullptr;

	//パフォーマンスが高い物から順に、全てのアダプターを列挙する
	for (uint32_t i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND; i++) {
		//動的配列に追加する
		adapters.push_back(tmpAdapter);
	}
#pragma endregion

#pragma region アダプターの選別
	//妥当なアダプタを選別する
	for (size_t i = 0; i < adapters.size(); i++) {
		DXGI_ADAPTER_DESC3 adapterDesc;
		//アダプターの情報を取得する
		adapters[i]->GetDesc3(&adapterDesc);

		//ソフトウェアデバイスを回避
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//デバイスを採用してループを抜ける
			tmpAdapter = adapters[i].Get();
			break;
		}
	}
#pragma endregion

#pragma region デバイス生成
	//対応レベルの配列
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	for (size_t i = 0; i < _countof(levels); i++) {
		//採用したアダプターでデバイスを生成
		result = D3D12CreateDevice(tmpAdapter, levels[i], IID_PPV_ARGS(&device_));
		if (result == S_OK) {
			//デバイスを生成できた時点でループを抜ける
			featureLevel = levels[i];
			break;
		}
	}
#pragma endregion

#pragma region エラー判断処理
	assert(SUCCEEDED(result));

	ComPtr<ID3D12InfoQueue> infoQuene;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQuene))))
	{
		infoQuene->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);    // やばいエラー一時に止まる
		infoQuene->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);        // エラー時に止まる
		infoQuene->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);    // ワーニング時に止まる
	}
#pragma endregion

}

void DirectXCommon::InitialzeCommand()
{

#pragma region コマンドアロケータの生成
	//コマンドアローケータを生成
	result = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	assert(SUCCEEDED(result));
#pragma endregion

#pragma region コマンドリストの生成
	//コマンドリストを生成
	result = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
	assert(SUCCEEDED(result));
#pragma endregion

#pragma region コマンドキューの生成
	//コマンドキューの設定
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

	//コマンドキューを生成
	result = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(result));
#pragma endregion

}

void DirectXCommon::InitializeSwapchain(WinApp* winApp)
{
#pragma region スワップチェーンの設定
	//スワップチェーンの設定
	
	swapChainDesc_.Width = 1280;
	swapChainDesc_.Height = 720;
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;			//色情報の書式
	swapChainDesc_.SampleDesc.Count = 1;							//マルチサンプルしない
	swapChainDesc_.BufferUsage = DXGI_USAGE_BACK_BUFFER;			//バックバッファ用
	swapChainDesc_.BufferCount = 2;								//バッファ数を2つに設定
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	//フリップ後は破壊
	swapChainDesc_.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
#pragma endregion

#pragma region スワップチェーンの生成
	//スワップチェーンの生成
	ComPtr<IDXGISwapChain1> swapChain1;

	result = dxgiFactory_->CreateSwapChainForHwnd(
		commandQueue_.Get(),
		winApp->GetHwnd(),
		&swapChainDesc_,
		nullptr,
		nullptr,
		&swapChain1);
#pragma endregion
	
#pragma region IDXGISwapChain1のオブジェクトをIDXGISwapChain4に変換する
	//生成下IDXGISwapChain1のオブジェクトをIDXGISwapChain4に変換する
	swapChain1.As(&swapChain_);

	assert(SUCCEEDED(result));
#pragma endregion

}

void DirectXCommon::InitializeRenderTargetView()
{

#pragma region RTV用デスクリプタヒープ生成
	//デスクリプタヒープの設定
	
	rtvHeapDesc_.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;			//レンダーターゲットビュー
	rtvHeapDesc_.NumDescriptors = swapChainDesc_.BufferCount;		//裏表の2つ

	//デスクリプタヒープの生成
	device_->CreateDescriptorHeap(&rtvHeapDesc_, IID_PPV_ARGS(&rtvHeap_));

	//バックバッファ
	backBuffers_.resize(swapChainDesc_.BufferCount);

	
#pragma endregion

#pragma region レンダーターゲットビューの生成
	// 裏表の２つ分
	//スワップチェーンの全てのバッファについて処理する
	for (size_t i = 0; i < backBuffers_.size(); i++) {
		//スワップチェーンからバッファを取得
		swapChain_->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffers_[i]));
		//デスクリプタヒープのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
		//裏か表かでアドレスがずれる
		rtvHandle.ptr += i * device_->GetDescriptorHandleIncrementSize(rtvHeapDesc_.Type);
		//レンダーターゲットビューの設定
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		//シェーダーの計算結果をSRGBに変換して書き込む
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		//レンダーターゲットビューの生成
		device_->CreateRenderTargetView(backBuffers_[i].Get(), &rtvDesc, rtvHandle);
	}
#pragma endregion

}

void DirectXCommon::InitializeDepthBuffer()
{
#pragma region 深度バッファリソース設定
	//リソース設定
	D3D12_RESOURCE_DESC depthResoureDesc{};
	depthResoureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResoureDesc.Width = WinApp::window_width;
	depthResoureDesc.Height = WinApp::window_height;
	depthResoureDesc.DepthOrArraySize = 1;
	depthResoureDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	depthResoureDesc.SampleDesc.Count = 1;
	depthResoureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	//深度値用ヒーププロパティ
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.DepthStencil.Stencil = 0;
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
#pragma endregion

#pragma region 深度バッファの生成
	//リソース生成
	depthBuff_ = nullptr;
	
	result = device_->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResoureDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&depthBuff_));
	depthBuff_->SetName(L"dxCommonDepth");
#pragma endregion

#pragma region DSV用デスクリプタヒープの生成
	//深度ビュー用デスクリプタヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	
	result = device_->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap_));
#pragma endregion

#pragma region 深度バッファビュー（DSV）の生成
	//深度ビュー作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
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
	//フェンスの生成
	result = device_->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
}

void DirectXCommon::InitializeFixFPS()
{
	// 現在時間を記録する
	reference_ = std::chrono::steady_clock::now();

}

void DirectXCommon::UpdateFixFPS()
{
	// 1/60秒ぴったりの時間
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));

	// 1/60秒よりわずかに短い時間
	const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));

	// 現在時間を取得する
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	// 前回記録からの経過時間を取得する
	std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	// 1/60秒（よりわずかに短い時間）経っていない場合
	if (elapsed < kMinCheckTime) {
		// 1/60秒経過するまで微小なスリープを繰り返す
		while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
			// 1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}

	// 現在時間を記録する
	reference_ = std::chrono::steady_clock::now();
}
