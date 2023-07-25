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

	// �G���[�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3DBlob> lErrorBlob;

	// �f�B���N�g���p�X�ƃt�@�C������A�����ăt���p�X�𓾂�
	std::string fullPath = sDefaultShaderDirectoryPath_ + fileName_;

	std::wstring lWFileName;
	lWFileName = StringToWstring(fullPath);

	// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	lResult = D3DCompileFromFile(
		lWFileName.c_str(), // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		entryPoint_.c_str(), target_.c_str(), // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&blob, &lErrorBlob);

	// �G���[�Ȃ�
	if (FAILED(lResult))
	{
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string lError;
		lError.resize(lErrorBlob->GetBufferSize());
		std::copy_n((char*)lErrorBlob->GetBufferPointer(),
			lErrorBlob->GetBufferSize(),
			lError.begin());
		lError += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(lError.c_str());
		assert(0);
	};

	shaderBytecode.pShaderBytecode = blob->GetBufferPointer();
	shaderBytecode.BytecodeLength = blob->GetBufferSize();

	type = type_;
}
