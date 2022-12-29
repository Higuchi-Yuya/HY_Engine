#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include <d3d12.h>
#include <d3dx12.h>
#include <string>

class Material
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:// サブクラス
	// 定数バッファ用データ構造体B1
	struct ConstBufferDataB1
	{
		Vector3 ambient; // アンビエント係数
		float pad1; // パディング
		Vector3 diffuse; // ディフューズ係数
		float pad2; // パディング
		Vector3 specular; // スペキュラー係数
		float alpha; // アルファ
	};
public: // 静的メンバ関数
	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// マテリアル生成
	/// </summary>
	/// <returns>生成されたマテリアル</returns>
	static Material* Create();


private: // 静的メンバ変数
	// デバイス
	static ID3D12Device* device;

public:
	std::string name;// マテリアル名
	Vector3 ambient; // アンビエント影響度
	Vector3 diffuse; // ディフューズ影響度
	Vector3 specular; // スペキュラー影響度
	float alpha; // アルファ
	std::string textureFilename; // テクスチャファイル名

	// テクスチャ番号
	uint32_t textureIndex = 0;
public:
	/// <summary>
	/// 定数バッファの取得
	/// </summary>
	/// <returns></returns>
	ID3D12Resource* GetConstantBuffer() { return constBuff.Get(); }

	/// テクスチャ読み込み
	/// </summary>
	/// <param name="directoryPath">読み込みディレクトリパス</param>

	void LoadTexture(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

private:

	// マテリアル用定数バッファ
	ComPtr<ID3D12Resource> constBuff;

	ConstBufferDataB1* constMap = nullptr;

private:
	// コンストラクタ
	Material() {
		ambient = { 0.3f, 0.3f, 0.3f };
		diffuse = { 0.0f, 0.0f, 0.0f };
		specular = { 0.0f, 0.0f, 0.0f };
		alpha = 1.0f;
	}
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 定数バッファの生成
	/// </summary>
	void CreateConstantBuffer();
};

