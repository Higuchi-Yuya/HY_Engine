#pragma once
#include"Vector3.h"
#include "Vector4.h"
#include "TextureManager.h"
#include<d3d12.h>
#include<wrl.h>

class Dissolve
{
public:// サブクラス
//定数バッファ用データ構造体
	struct ConstBufferDataDissolve {
		Vector4 dissolveColor; // ディゾルブの消える時の色
		bool isActiveDissolve; // フォグを掛けるかどうかのフラグ
		float dissolvePower;	  // ディゾルブの消えるときの色の度合
		float dissolveTime;    // ディゾルブの消えるまでの時間の割合
		float dissolveSmoothMin;// ディゾルブスムースの最小値
	};
public:// メンバ関数
	~Dissolve();

	// 静的初期化
	static void StaticInitialize(ID3D12Device* device);

	static Dissolve* Create();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 行列を更新する
	/// </summary>
	void UpdateMatrix();

	// 描画
	void Draw(ID3D12GraphicsCommandList* cmdList, uint32_t dissolveRootIndex);

	// バッファのゲッター
	ID3D12Resource* GetBuff() { return constBuff_.Get(); }

	// テクスチャのセット
	void SetDissolveTexture(Texture* dissolveTex) { dissolveTex_ = dissolveTex; }

private:// プライベート関数
	/// <summary>
	/// 定数バッファ生成
	/// </summary>
	void CreateConstBuffer();

	/// <summary>
	/// マッピングする
	/// </summary>
	void Map();

public:// 外側から変更可能な値
	bool isActiveDissolve_ = false;
	float dissolvePower_ = 10.0f;
	float dissolveTime_ = 0.0f;
	float dissolveSmoothMin_ = -0.5;
	Vector4 dissolveColor_ = { 1.0f,1.0f,1.0f,1.0f };

private:// メンバ変数
	// デバイス（借りてくる）
	static ID3D12Device* sDevice_;

	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;

	// マッピング済みアドレス
	ConstBufferDataDissolve* constMap_ = nullptr;

	// デフォルトのディゾルブテクスチャのルートパス
	static const std::string sDissolveTexBasePass_;

	// ディゾルブのテクスチャ
	Texture* dissolveTex_;
};

