#include "ShaderObj.h"
std::string ShaderObj::sDefaultShaderDirectoryPath_ = "Resources/Shaders/";

std::wstring StringToWstring(const std::string& string)
{
	auto const dest_size = ::MultiByteToWideChar(CP_ACP, 0U, string.data(), -1, nullptr, 0U);
	std::vector<wchar_t> dest(static_cast<const uint64_t>(dest_size), L'\0');
	if (::MultiByteToWideChar(CP_ACP, 0U, string.data(), -1, dest.data(), static_cast<int32_t>(dest.size())) == 0)
	{
		throw std::system_error{ static_cast<int32_t>(::GetLastError()), std::system_category() };
	}
	dest.resize(std::char_traits<wchar_t>::length(dest.data()));
	dest.shrink_to_fit();
	return std::wstring(dest.begin(), dest.end());
}

void ShaderObj::Create(const std::string& fileName_, const std::string& entryPoint_, const std::string& target_, const ShaderType& type_)
{
	HRESULT lResult = S_OK;

	// エラーオブジェクト
	Microsoft::WRL::ComPtr<ID3DBlob> lErrorBlob;

	// ディレクトリパスとファイル名を連結してフルパスを得る
	std::string fullPath = sDefaultShaderDirectoryPath_ + fileName_;

	std::wstring lWFileName;
	lWFileName = StringToWstring(fullPath);

	// ピクセルシェーダの読み込みとコンパイル
	lResult = D3DCompileFromFile(
		lWFileName.c_str(), // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		entryPoint_.c_str(), target_.c_str(), // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&blob, &lErrorBlob);

	// エラーなら
	if (FAILED(lResult))
	{
		// errorBlobからエラー内容をstring型にコピー
		std::string lError;
		lError.resize(lErrorBlob->GetBufferSize());
		std::copy_n((char*)lErrorBlob->GetBufferPointer(),
			lErrorBlob->GetBufferSize(),
			lError.begin());
		lError += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(lError.c_str());
		assert(0);
	};

	shaderBytecode.pShaderBytecode = blob->GetBufferPointer();
	shaderBytecode.BytecodeLength = blob->GetBufferSize();

	type = type_;
}
