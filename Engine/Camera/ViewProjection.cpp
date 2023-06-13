#include "ViewProjection.h"
#include <d3dx12.h>
#include <cassert>
#include "WinApp.h"

Microsoft::WRL::ComPtr<ID3D12Device> ViewProjection::sDevice_ = nullptr;

void ViewProjection::StaticInitialize(ID3D12Device* device)
{
	assert(device);
	sDevice_ = device;
}

void ViewProjection::Initialize()
{
	CreateConstBuffer();
	Map();
	UpdateMatrix();
}

void ViewProjection::CreateConstBuffer()
{
	HRESULT result;

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataViewProjection) + 0xff) & ~0xff);

	// 定数バッファの生成
	result = sDevice_->CreateCommittedResource(
		&heapProps, // アップロード可能
		D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));
}

void ViewProjection::Map()
{
	// 定数バッファとのデータリンク
	HRESULT result = constBuff_->Map(0, nullptr, (void**)&constMap_);
	assert(SUCCEEDED(result));
}

void ViewProjection::MoveEyeVector(const Vector3& move)
{
	// 視点座標を移動し、反映
	Vector3 eye_moved = eye;

	eye_moved.x += move.x;
	eye_moved.y += move.y;
	eye_moved.z += move.z;

	eye = eye_moved;
}

void ViewProjection::MoveVector(const Vector3& move)
{
	// 視点と注視点座標を移動し、反映
	Vector3 eye_moved = eye;
	Vector3 target_moved = target;

	eye_moved.x += move.x;
	eye_moved.y += move.y;
	eye_moved.z += move.z;

	target_moved.x += move.x;
	target_moved.y += move.y;
	target_moved.z += move.z;

	eye = eye_moved;
	target = target_moved;
}

void ViewProjection::UpdateMatrix()
{
	// ビュー行列の作成
	matView_.ViewMat(eye, target, up);
	// 射影行列の作成
	matProjection_.ProjectionMat(fovAngleY, aspectRatio, nearZ, farZ);

	// 定数バッファへの書き込み
	constMap_->view = matView_;
	constMap_->projection = matProjection_;
	constMap_->cameraPos = eye;
}

void ViewProjection::DebugCameraInitialze(Input* input)
{
	Initialize();
	assert(input);
	this->input_ = input;
	// 画面サイズに対する相対的なスケールに調整
	scaleX_ = 1.0f / (float)WinApp::window_width;
	scaleY_ = 1.0f / (float)WinApp::window_height;

}

void ViewProjection::DebugCameraUpdate()
{
	bool dirty = false;
	float angleX = 0;
	float angleY = 0;

	// マウスの入力を取得
	Input::MouseMove mouseMove = input_->GetMouseMove();

	// マウスの左ボタンが押されていたらカメラを回転させる
	if (input_->PushMouseLeft()) {
		float dy = mouseMove.lX * scaleY_;
		float dx = mouseMove.lY * scaleX_;

		angleX = -dx * PI;
		angleY = -dy * PI;
		dirty = true;
	}

	// マウスの中ボタンが押されていたらカメラを並行移動させる
	if (input_->PushMouseMiddle()) {
		float dx = mouseMove.lX / 100.0f;
		float dy = mouseMove.lY / 100.0f;

		Vector3 move = { -dx, +dy,  0 };
		move = matRot_.transform(move, matRot_);

		MoveVector(move);
		dirty = true;
	}

	// ホイール入力で距離を変更
	if (mouseMove.lZ != 0) {
		distance_ -= mouseMove.lZ / 100.0f;
		distance_ = max(distance_, 1.0f);
		dirty = true;
	}

	if (dirty) {
		// 追加回転分の回転行列を生成
		Matrix4 matRotNew,matRotX,matRotY;
		matRotNew.identity();
		matRotX.rotateX(-angleX);
		matRotY.rotateY(-angleY);
		matRotNew = matRotX * matRotY;
		// 累積の回転行列を合成
		// ※回転行列を累積していくと、誤差でスケーリングがかかる危険がある為
		// クォータニオンを使用する方が望ましい
		matRot_ = matRotNew * matRot_;

		// 注視点から視点へのベクトルと、上方向ベクトル
		Vector3 vTargetEye = { 0.0f, 0.0f, -distance_};
		Vector3 vUp = { 0.0f, 1.0f, 0.0f };

		// ベクトルを回転
		vTargetEye = matRot_.transform(vTargetEye, matRot_);
		vUp = matRot_.transformNotW(vUp, matRot_);

		// 注視点からずらした位置に視点座標を決定
		const Vector3& target = this->target;
		this->eye = { target.x + vTargetEye.x, target.y + vTargetEye.y,target.z + vTargetEye.z };
		this->up={ vUp.x, vUp.y, vUp.z };
	}

	UpdateMatrix();
}