#pragma once
#include <imgui.h>
#include "WinApp.h"
#include "DirectXCommon.h"
class ImGuiManager
{
public:
	ImGuiManager();
	~ImGuiManager();

	// 初期化
	void Initialize(WinApp *winApp, DirectXCommon *dxCommon);

	// ImGui受付開始
	void Begin();

	// ImGui受付終了
	void End();

	// 画面への描画
	void Draw();

	// 後始末
	void Finalize();




private:
	static DirectXCommon* dxCommon;

	// SRV用のデスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;
};