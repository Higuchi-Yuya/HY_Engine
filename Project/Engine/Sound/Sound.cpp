#include "Sound.h"

Microsoft::WRL::ComPtr<IXAudio2>Sound::xAudio2_;
IXAudio2MasteringVoice* Sound::masterVoice_;

std::string Sound::kDefaultSoundDirectoryPath = "Resources/Sound/";

Sound::~Sound()
{
	// xaudio2�̉��
	xAudio2_.Reset();
	//�o�b�t�@�̃����������
	delete[] soundData_.pBuffer;

	soundData_.pBuffer = 0;
	soundData_.bufferSize = 0;
	soundData_.wfex = {};
}

void Sound::StaticInitialize()
{

	HRESULT result;
	result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	result = xAudio2_->CreateMasteringVoice(&masterVoice_);

}

//�����f�[�^�̓ǂݍ���
void Sound::SoundLoadWave(const std::string& filename) {

	//-------�@�t�@�C���I�[�v��-------//

	//�t�@�C�����̓X�g���[���̃C���X�^���X
	std::ifstream file;

	std::string fullPath = kDefaultSoundDirectoryPath + filename;

	//.wav�t�@�C�����o�C�i�����[�h�ŊJ��
	file.open(fullPath.c_str(), std::ios_base::binary);

	//�t�@�C���I�[�v�����s�����o����
	assert(file.is_open());

	//-------�A.wav�f�[�^�ǂݍ���-------//

	//RIFF�w�b�_�[�̓ǂݍ���
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));

	//�t�@�C����RIFF���`�F�b�N
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}

	//�^�C�v��WAVE���`�F�b�N
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}

	//Format�`�����N�̓ǂݍ���
	FormatChunk format = {};

	//�`�����N�w�b�_�[�̊m�F
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}

	//�`�����N�{�̂̓ǂݍ���
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	//Data�`�����N�̓ǂݍ���
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));

	//bext�`�����N�����o�����ꍇ
	if (strncmp(data.id, "bext", 4) == 0) {
		//�ǂݍ��݈ʒu��JUNK�`�����N�̏I���܂Ői�߂�
		file.seekg(data.size, std::ios_base::cur);
		//�ēǂݍ���
		file.read((char*)&data, sizeof(data));
	}

	//JUNK�`�����N�����o�����ꍇ
	if (strncmp(data.id, "JUNK", 4) == 0) {
		//�ǂݍ��݈ʒu��JUNK�`�����N�̏I���܂Ői�߂�
		file.seekg(data.size, std::ios_base::cur);
		//�ēǂݍ���
		file.read((char*)&data, sizeof(data));
	}

	//JUNK�`�����N�����o�����ꍇ
	if (strncmp(data.id, "junk", 4) == 0) {
		//�ǂݍ��݈ʒu��JUNK�`�����N�̏I���܂Ői�߂�
		file.seekg(data.size, std::ios_base::cur);
		//�ēǂݍ���
		file.read((char*)&data, sizeof(data));
	}
	//LIST�`�����N�����o�����ꍇ
	if (strncmp(data.id, "LIST", 4) == 0) {
		//�ǂݍ��݈ʒu��JUNK�`�����N�̏I���܂Ői�߂�
		file.seekg(data.size, std::ios_base::cur);
		//�ēǂݍ���
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}

	//Data�`�����N�̃f�[�^��(�g�`�f�[�^)�̓ǂݍ���
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	//Wave�t�@�C�������
	file.close();

	//-------�B�ǂݍ��񂾉����f�[�^��return-------//

	//return����ׂ̉����f�[�^
	SoundData soundData = {};

	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;

	soundData_ = soundData;

}

//-------�����f�[�^�̉��-------//
void Sound::SoundUnload() {
	// xaudio2�̉��
	xAudio2_.Reset();

	//�o�b�t�@�̃����������
	delete[] soundData_.pBuffer;

	soundData_.pBuffer = 0;
	soundData_.bufferSize = 0;
	soundData_.wfex = {};
}

//------�T�E���h�̍Đ�-------//

//�����Đ�
void Sound::SoundPlayWave(bool loop, float volume) {
	HRESULT result;

	//�g�`�t�H�[�}�b�g������SourceVoice�̐���
	result = xAudio2_->CreateSourceVoice(&pSourceVoice_, &soundData_.wfex);
	assert(SUCCEEDED(result));

	//�Đ�����g�`�f�[�^�̐ݒ�
	buf_.pAudioData = soundData_.pBuffer;
	buf_.AudioBytes = soundData_.bufferSize;

	pSourceVoice_->SetVolume(volume);

	if (loop)
	{
		buf_.LoopCount = XAUDIO2_LOOP_INFINITE;
	}
	buf_.Flags = XAUDIO2_END_OF_STREAM;


	//�g�`�f�[�^�̍Đ�
	result = pSourceVoice_->SubmitSourceBuffer(&buf_);
	result = pSourceVoice_->Start();

}

//----------�����X�g�b�v------------//
void Sound::StopWave()
{
	HRESULT result;
	if (pSourceVoice_ != nullptr)
	{
		result = pSourceVoice_->Stop();
		result = pSourceVoice_->FlushSourceBuffers();
		result = pSourceVoice_->SubmitSourceBuffer(&buf_);
	}
}