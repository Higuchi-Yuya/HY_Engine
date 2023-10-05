#pragma once

#include <cstdint>

namespace Util
{
	// 桁数を取得
	uint32_t GetDight(const uint32_t value);

	// 現在の桁の数を取得
	uint32_t GetDightNumber(const int32_t value, const uint32_t dight);
}

