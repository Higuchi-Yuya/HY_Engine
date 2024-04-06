#pragma once

namespace State
{
	enum class AliveState
	{
		Patrol,//巡回
		Back,// 戻る
		Tracking,//追尾
		Dead,
	};
}