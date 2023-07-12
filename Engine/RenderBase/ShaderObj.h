#pragma once
#include<wrl.h>
#include<d3dx12.h>
#include<d3dcompiler.h>
#include<cassert>
#include<filesystem>

class ShaderObj
{
public:// サブクラス
	//タイプ
	enum class ShaderType
	{
		VS,//バーテクス
		PS,//ピクセル
		DS,//ドメイン
		HS,//ハル
		GS,//ジオメトリ
		CS,//コンピュート
	};

public:// メンバ関数

	/// <summary>
	/// シェーダオブジェクトを生成
	/// </summary>
	void Create(const std::string& fileName_, const std::string& entryPoint_, const std::string& target_, const ShaderType& type_);

	/// <summary>
	/// シェーダオブジェクトを取得
	/// </summary>
	D3D12_SHADER_BYTECODE* GetShader() {return &shaderBytecode;}

	/// <summary>
	/// タイプ
	/// </summary>
	const ShaderType& GetType() { return type; }

private:// メンバ変数
	//シェーダオブジェクト
	Microsoft::WRL::ComPtr<ID3DBlob> blob;

	//シェーダバイトコード
	D3D12_SHADER_BYTECODE shaderBytecode{};

	//タイプ
	ShaderType type = ShaderType::VS;

};

