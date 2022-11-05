#pragma region �ǂݍ��ރw�b�_�[

#pragma comment(lib,"d3dcompiler.lib")


#include<iostream>

#include <math.h>
#include "Line.h"
#include "Mesh.h"

#include "Render_basic.h"
#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Sprite.h"
using namespace Microsoft::WRL;
using namespace std;


#pragma endregion
const float PI = 3.141592f;

#pragma region ���܂��Ȃ�
// @brief �R���\�[����ʂɃt�H�[�}�b�g�t��������̕\��
// @param format �t�H�[�}�b�g(%d�Ƃ�%f�Ƃ���)
// @param �ϒ�����
// @remarks ���̊֐��̓f�o�b�N�p�ł��B�f�o�b�O���ɂ������삵�܂���
void DebugOutputFormatString(const char* format, ...) {
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	vprintf(format, valist);
	va_end(valist);
#endif
}


#pragma endregion

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR,  _In_ int) {

#pragma region WindowsAPI����������
	// �|�C���^
	WinApp* winApp = nullptr;

	// WindouwsAPI�̏�����
	winApp = new WinApp();
	winApp->Initialize();

#pragma endregion

#pragma region DirectX����������

	////////////////////////////////////////////////////
	//-------------DirectX12������������������-----------//
	//////////////////////////////////////////////////

	// Common�|�C���^
	DirectXCommon* dxCommon = nullptr;

	// DirectX�̏�����
	dxCommon = new DirectXCommon();
	dxCommon->Initalize(winApp);

	// �|�C���^
	Input* input = nullptr;

	// ���͂̏�����
	input = new Input();
	input->Initialize(winApp);

	// �X�v���C�g�̏�����
	Sprite::StaticInitialize(dxCommon->GetDevice(), WinApp::window_width, WinApp::window_height);
	
	/////////////////////////////////////////////////////////
	//--------------DirectX12�����������@�����܂�-------------//
	///////////////////////////////////////////////////////
#pragma endregion


	Sprite* sprite = nullptr;

	Sprite::LoadTexture(2, L"Resources/texture.png");
	sprite = Sprite::Create(2, { 500,200 }, { 1,1,1,1 }, { 0,0 }, false, false);

	Render_basic::GetInstance()->Initialization(dxCommon->GetDevice());
	
	Render_basic::GetInstance()->Initialization2(dxCommon->GetDevice());

	Mesh mesh[10];

	Line line[20];

	Mesh::Vertex vertices[] = {
		// x      y      z       u      v
		//�O
		{{-5.0f, -5.0f, -5.0f}, {},{0.0f, 1.0f}}, // ����
		{{-5.0f,  5.0f, -5.0f}, {},{0.0f, 0.0f}}, // ����
		{{ 5.0f, -5.0f, -5.0f}, {},{1.0f, 1.0f}}, // �E��
		{{ 5.0f,  5.0f, -5.0f}, {},{1.0f, 0.0f}}, // �E��
	
		//��
		{{-5.0f, -5.0f,  5.0f}, {},{0.0f, 1.0f}}, // ����
		{{-5.0f,  5.0f,  5.0f}, {},{0.0f, 0.0f}}, // ����
		{{ 5.0f, -5.0f,  5.0f}, {},{1.0f, 1.0f}}, // �E��
		{{ 5.0f,  5.0f,  5.0f}, {}, {1.0f, 0.0f}}, // �E��

		//��
		{{-5.0f, -5.0f, -5.0f}, {},{0.0f, 1.0f}}, // ����
		{{-5.0f, -5.0f,  5.0f}, {},{0.0f, 0.0f}}, // ����
		{{-5.0f,  5.0f, -5.0f}, {},{1.0f, 1.0f}}, // �E��
		{{-5.0f,  5.0f,  5.0f}, {},{1.0f, 0.0f}}, // �E��

		//�E
		{{ 5.0f, -5.0f, -5.0f}, {},{0.0f, 1.0f}}, // ����
		{{ 5.0f, -5.0f,  5.0f}, {},{0.0f, 0.0f}}, // ����
		{{ 5.0f,  5.0f, -5.0f}, {},{1.0f, 1.0f}}, // �E��
		{{ 5.0f,  5.0f,  5.0f}, {},{1.0f, 0.0f}}, // �E��

		//��
		{{-5.0f, -5.0f, -5.0f}, {},{0.0f, 1.0f}}, // ����
		{{-5.0f, -5.0f,  5.0f}, {},{0.0f, 0.0f}}, // ����
		{{ 5.0f, -5.0f, -5.0f}, {},{1.0f, 1.0f}}, // �E��
		{{ 5.0f, -5.0f,  5.0f}, {},{1.0f, 0.0f}}, // �E��

		//��
		{{-5.0f,  5.0f, -5.0f}, {},{0.0f, 1.0f}}, // ����
		{{-5.0f,  5.0f,  5.0f}, {},{0.0f, 0.0f}}, // ����
		{{ 5.0f,  5.0f, -5.0f}, {},{1.0f, 1.0f}}, // �E��
		{{ 5.0f,  5.0f,  5.0f}, {},{1.0f, 0.0f}}, // �E��
	};

	Mesh::Vertex line_vertices1[] = {
		{{2.0f,0.0f,18.0f}},
		{{2.0f,0.0f,-18.0f}}
	};
	Mesh::Vertex line_vertices2[] = {
		{{6.0f,0.0f,18.0f}},
		{{6.0f,0.0f,-18.0f}}
	};
	Mesh::Vertex line_vertices3[] = {
		{{10.0f,0.0f,18.0f}},
		{{10.0f,0.0f,-18.0f}},
	};
	Mesh::Vertex line_vertices4[] = {
		{{14.0f,0.0f,18.0f}},
		{{14.0f,0.0f,-18.0f}},
	};
	Mesh::Vertex line_vertices5[] = {
		{{18.0f,0.0f,18.0f}},
		{{18.0f,0.0f,-18.0f}},
	};
	Mesh::Vertex line_vertices6[] = {
		{{-2.0f,0.0f,18.0f}},
		{{-2.0f,0.0f,-18.0f}}
	};
	Mesh::Vertex line_vertices7[] = {
		{{-6.0f,0.0f,18.0f}},
		{{-6.0f,0.0f,-18.0f}},
	};
	Mesh::Vertex line_vertices8[] = {
		{{-10.0f,0.0f,18.0f}},
		{{-10.0f,0.0f,-18.0f}},
	};
	Mesh::Vertex line_vertices9[] = {
		{{-14.0f,0.0f,18.0f}},
		{{-14.0f,0.0f,-18.0f}},
	};
	Mesh::Vertex line_vertices10[] = {
	{{-18.0f,0.0f,18.0f}},
	{{-18.0f,0.0f,-18.0f}},
	};
	Mesh::Vertex line_vertices11[] = {
	{{18.0f,0.0f,2.0f}},
	{{-18.0f,0.0f,2.0f}}

	};
	Mesh::Vertex line_vertices12[] = {
		{{18.0f,0.0f,6.0f}},
		{{-18.0f,0.0f,6.0f}}
	};
	Mesh::Vertex line_vertices13[] = {
		{{18.0f,0.0f,10.0f}},
		{{-18.0f,0.0f,10.0f}},
	};
	Mesh::Vertex line_vertices14[] = {
		{{18.0f,0.0f,14.0f}},
		{{-18.0f,0.0f,14.0f}},
	};
	Mesh::Vertex line_vertices15[] = {
		{{18.0f,0.0f,18.0f}},
		{{-18.0f,0.0f,18.0f}},
	};
	Mesh::Vertex line_vertices16[] = {
		{{18.0f,0.0f,-2.0f}},
		{{-18.0f,0.0f,-2.0f}}
	};
	Mesh::Vertex line_vertices17[] = {
		{{18.0f,0.0f,-6.0f}},
		{{-18.0f,0.0f,-6.0f}},
	};
	Mesh::Vertex line_vertices18[] = {
		{{18.0f,0.0f,-10.0f}},
		{{-18.0f,0.0f,-10.0f}},
	};
	Mesh::Vertex line_vertices19[] = {
		{{18.0f,0.0f,-14.0f}},
		{{-18.0f,0.0f,-14.0f}},
	};
	Mesh::Vertex line_vertices20[] = {
	{{18.0f,0.0f,-18.0f}},
	{{-18.0f,0.0f,-18.0f}},
	};

	unsigned short indices[] = {
		//�O
		0, 1, 2, // �O�p�`1��
		2, 1, 3, // �O�p�`2��

		//��
		6,5,4,
		7,5,6,

		////��
		8,9,10,
		10,9,11,

		//�E
		12,14,13,
		13,14,15,

		//��
		18,17,16,
		19,17,18,

		//��
		20,21,22,
		22,21,23,


	};

	unsigned short indices2[] = {
		0,1
	};

	for (int i = 0; i < _countof(indices) / 3; i++)
	{// �O�p�`����ƂɌv�Z���Ă���
		// �O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɓ����
		unsigned short indices_1 = indices[i * 3 + 0];
		unsigned short indices_2 = indices[i * 3 + 1];
		unsigned short indices_3 = indices[i * 3 + 2];
		//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
		XMVECTOR p0 = XMLoadFloat3(&vertices[indices_1].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[indices_2].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[indices_3].pos);
		// ���O�[�����P�x�N�g���A���O�[�����Q�x�N�g�����v�Z�@�i�x�N�g���̌��Z�j
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);

		//�O�ς͗������琂���ȃx�N�g��
		XMVECTOR normal = XMVector3Cross(v1, v2);

		//���K���i�������P�ɂ���j
		normal = XMVector3Normalize(normal);

		//���߂��@���𒸓_�f�[�^�ɑ��
		XMStoreFloat3(&vertices[indices_1].normal, normal);
		XMStoreFloat3(&vertices[indices_2].normal, normal);
		XMStoreFloat3(&vertices[indices_3].normal, normal);
	}

	int vertices_count;
	int indices_count;

	int vertices_count2;
	int indices_count2;

	vertices_count = _countof(vertices);
	indices_count = _countof(indices);

	vertices_count2 = _countof(line_vertices1);
	indices_count2 = _countof(indices2);

	mesh[0].Mesh_Initialization(dxCommon->GetDevice(), vertices, indices, _countof(vertices), indices_count);

#pragma region �O���b�g���̐ݒ�
	line[0].Line_Initialize(dxCommon->GetDevice(), line_vertices1, indices2, vertices_count2, indices_count2);
	vertices_count2 = _countof(line_vertices2);
	line[1].Line_Initialize(dxCommon->GetDevice(), line_vertices2, indices2, vertices_count2, indices_count2);
	vertices_count2 = _countof(line_vertices3);
	line[2].Line_Initialize(dxCommon->GetDevice(), line_vertices3, indices2, vertices_count2, indices_count2);
	vertices_count2 = _countof(line_vertices4);
	line[3].Line_Initialize(dxCommon->GetDevice(), line_vertices4, indices2, vertices_count2, indices_count2);
	vertices_count2 = _countof(line_vertices5);
	line[4].Line_Initialize(dxCommon->GetDevice(), line_vertices5, indices2, vertices_count2, indices_count2);
	vertices_count2 = _countof(line_vertices6);
	line[5].Line_Initialize(dxCommon->GetDevice(), line_vertices6, indices2, vertices_count2, indices_count2);
	vertices_count2 = _countof(line_vertices7);
	line[6].Line_Initialize(dxCommon->GetDevice(), line_vertices7, indices2, vertices_count2, indices_count2);
	vertices_count2 = _countof(line_vertices8);
	line[7].Line_Initialize(dxCommon->GetDevice(), line_vertices8, indices2, vertices_count2, indices_count2);
	vertices_count2 = _countof(line_vertices9);
	line[8].Line_Initialize(dxCommon->GetDevice(), line_vertices9, indices2, vertices_count2, indices_count2);
	vertices_count2 = _countof(line_vertices10);
	line[9].Line_Initialize(dxCommon->GetDevice(), line_vertices10, indices2, vertices_count2, indices_count2);
	vertices_count2 = _countof(line_vertices11);
	line[10].Line_Initialize(dxCommon->GetDevice(), line_vertices11, indices2, vertices_count2, indices_count2);
	vertices_count2 = _countof(line_vertices12);
	line[11].Line_Initialize(dxCommon->GetDevice(), line_vertices12, indices2, vertices_count2, indices_count2);
	vertices_count2 = _countof(line_vertices13);
	line[12].Line_Initialize(dxCommon->GetDevice(), line_vertices13, indices2, vertices_count2, indices_count2);
	vertices_count2 = _countof(line_vertices14);
	line[13].Line_Initialize(dxCommon->GetDevice(), line_vertices14, indices2, vertices_count2, indices_count2);
	vertices_count2 = _countof(line_vertices15);
	line[14].Line_Initialize(dxCommon->GetDevice(), line_vertices15, indices2, vertices_count2, indices_count2);
	vertices_count2 = _countof(line_vertices16);
	line[15].Line_Initialize(dxCommon->GetDevice(), line_vertices16, indices2, vertices_count2, indices_count2);
	vertices_count2 = _countof(line_vertices17);
	line[16].Line_Initialize(dxCommon->GetDevice(), line_vertices17, indices2, vertices_count2, indices_count2);
	vertices_count2 = _countof(line_vertices18);
	line[17].Line_Initialize(dxCommon->GetDevice(), line_vertices18, indices2, vertices_count2, indices_count2);
	vertices_count2 = _countof(line_vertices19);
	line[18].Line_Initialize(dxCommon->GetDevice(), line_vertices19, indices2, vertices_count2, indices_count2);
	vertices_count2 = _countof(line_vertices20);
	line[19].Line_Initialize(dxCommon->GetDevice(), line_vertices20, indices2, vertices_count2, indices_count2);
#pragma endregion
	// �O���b�g���̏����ݒ�

	
	//�Q�[�����[�v
	while (true) {
#pragma region �E�B���h�E���b�Z�[�W����

		// Windows�̃��b�Z�[�W����
		if (winApp->ProcessMessage()) {
			// �Q�[�����[�v�𔲂���
			break;
		}

#pragma endregion

#pragma region DirectX���t���[������
		/////////////////////////////////////////////////////
		//----------DireceX���t���[�������@��������------------//
		///////////////////////////////////////////////////

		// ���͂̍X�V
		input->Update();
		
		// ������0�L�[��������Ă�����
		if (input->PushKey(DIK_0))
		{
			OutputDebugStringA("Hit 0\n");  // �o�̓E�B���h�E�ɁuHit 0�v�ƕ\��
		}

		

		

		//2.�`���̕ύX
		//�����_�[�^�[�Q�b�g�r���[�̃n���h�����擾
		
		

		

		

		////�g���K�[����
		//bool �L�[����������Ԃ�(uint8_t �L�[�ԍ�);
		//bool �L�[�𗣂�����Ԃ�(uint8_t �L�[�ԍ�);
		//bool �L�[���������u�Ԃ�(uint8_t �L�[�ԍ�);
		//bool �L�[�𗣂����u�Ԃ�(uint8_t �L�[�ԍ�);




		mesh->Mesh_Update(input);

		for (int i = 0; i < 20; i++)
		{
			line[i].Line_Updata();
		}
		
		
		/*transformX = 0.0f;
		transformY = 0.0f;
		rotation = 0.0f;
		scale = 1.0f;*/

		// �L�[����

		////���s�ړ�
		//if (key[DIK_W]) {
		//	transformY += 0.05f;
		//}

		//if (key[DIK_S]) {
		//	transformY -= 0.05f;
		//}

		//if (key[DIK_A]) {
		//	transformX -= 0.05f;
		//}

		//if (key[DIK_D]) {
		//	transformX += 0.05f;
		//}

		//// �g��k��
		//if (key[DIK_Z]) {
		//	scale -= 0.1f;
		//}

		//if (key[DIK_C]) {
		//	scale += 0.1f;
		//}


		//// ��]
		//if (key[DIK_Q]) {
		//	rotation -= PI / 32;
		//}

		//if (key[DIK_E]) {
		//	rotation += PI / 32;
		//}


		//// �A�t�B���s��̐���
		//affin[0][0] = scale * cos(rotation);
		//affin[0][1] = scale * (-sin(rotation));
		//affin[0][2] = transformX;

		//affin[1][0] = scale * sin(rotation);
		//affin[1][1] = scale * cos(rotation);
		//affin[1][2] = transformY;

		//affin[2][0] = 0.0f;
		//affin[2][1] = 0.0f;
		//affin[2][2] = 1.0f;

		//// �A�t�B���ϊ�
		//for (int i = 0; i < _countof(vertices); i++) {
		//	vertices[i].x = vertices[i].x * affin[0][0] +
		//		vertices[i].y * affin[0][1] + 1.0f * affin[0][2];
		//	vertices[i].y = vertices[i].x * affin[1][0] +
		//		vertices[i].y * affin[1][1] + 1.0f * affin[1][2];
		//	vertices[i].z = vertices[i].x * affin[2][0] +
		//		vertices[i].y * affin[2][1] + 1.0f * affin[2][2];
		//}


		//////////////////////////////////////////////
		//-------DireceX���t���[�������@�����܂�--------//
		////////////////////////////////////////////

#pragma endregion

#pragma region �O���t�B�b�N�X�R�}���h

		//4.�`��R�}���h��������
		dxCommon->PreDraw();
		// �`��O����
		Sprite::PreDraw(dxCommon->GetCommandList());
		sprite->Draw();
		// �`��㏈��
		Sprite::PostDraw();
		//// �[�x�o�b�t�@�N���A
		dxCommon->ClearDepthBuffer();
		//Mesh�̕`��--------------------------------------------------------------//
		
		mesh[0].Mesh_Draw(dxCommon->GetDevice(), indices_count, dxCommon->GetCommandList());
		//
		//for (int i = 0; i < 20; i++)
		//{
		//	line[i].Line_Draw(indices_count2, dxCommon->GetCommandList());
		//}
		//4.�`��R�}���h�����܂�
		
#pragma region �O�i�X�v���C�g�`��
// �`��O����
		Sprite::PreDraw(dxCommon->GetCommandList());

		

		// �`��㏈��
		Sprite::PostDraw();
#pragma endregion


		

#pragma region ��ʓ���ւ�
		dxCommon->PostDraw();
#pragma endregion


#pragma endregion




	}
#pragma region  WindowsAPI��n��

	//�����N���X�͎g��Ȃ��̂œo�^����������
	delete sprite;
	// ���͉��
	delete input;

	// WindouwsAPI�̏I������
	winApp->Finalize();

	// WindouwsAPI���
	delete winApp;

	// DirectXcommon�̉��
	delete dxCommon;
#pragma endregion

	//Render_basic::DestroyInstance();

	return 0;
}
