#include "PostEffectHandleManager.h"

std::unordered_map<std::string, Handles> PostEffectHandleManager::sPostHandles_;

void PostEffectHandleManager::SetPostEffectHandle(const std::string tag,Handles postEffectHandle)
{
	sPostHandles_.insert(std::make_pair(tag, std::move(postEffectHandle)));
}

Handles PostEffectHandleManager::GetPostEffectHandle(const std::string tag)
{
	return sPostHandles_[tag];
}


