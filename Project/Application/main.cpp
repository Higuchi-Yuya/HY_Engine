#pragma region 読み込むヘッダー

#include "MyGame.h"
#include <dxgidebug.h>

#pragma endregion

#pragma region デバッグの文字
// @brief コンソール画面にフォーマット付き文字列の表示
// @param format フォーマット(%dとか%fとかの)
// @param 可変長引数
// @remarks この関数はデバック用です。デバッグ時にしか動作しません
void DebugOutputFormatString([[maybe_unused]] const char* format, ...) {
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	vprintf(format, valist);
	va_end(valist);
#endif
}


#pragma endregion

struct D3DResouceLeakChecker
{
	~D3DResouceLeakChecker()
	{
		// リソースリークチェック
		Microsoft::WRL::ComPtr<IDXGIDebug1>debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR,  _In_ int) {
	
	// リソースチェック
	D3DResouceLeakChecker leckCheck;

	// ゲーム
	std::unique_ptr<MyGame> myGame = std::make_unique<MyGame>();

	// 実行
	myGame->Run();

	return 0;
}
