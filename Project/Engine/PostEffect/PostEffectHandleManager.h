#pragma once
#include "PostRenderBase.h"

class PostEffectHandleManager
{

public:// ハンドルのセッター
	static void SetPostEffectHandle(const std::string tag,Handles postEffectHandle);

public:// ハンドルのゲッター
	static Handles GetPostEffectHandle(const std::string tag);

private:

	// ポストエフェクトのCPUまたはGPUのハンドルたち
	static std::unordered_map<std::string,Handles> sPostHandles_;
};

