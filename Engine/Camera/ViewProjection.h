#pragma once
#include"Vector3.h"
#include "Vector4.h"
#include"Matrix4.h"
#include "WinApp.h"
#include<d3d12.h>
#include<wrl.h>
#include "Input.h"

class ViewProjection
{
public:// サブクラス
	// 定数バッファ用データ構造体
	struct ConstBufferDataViewProjection {
		Matrix4 view;       // ワールド → ビュー変換行列
		Matrix4 projection; // ビュー → プロジェクション変換行列
		Vector3 cameraPos;  // カメラ座標（ワールド座標）
	};
public:// メンバ関数
	// 静的初期化
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 行列を更新する
	/// </summary>
	void UpdateMatrix();

	// バッファのゲッター
	ID3D12Resource* GetBuff() { return constBuff_.Get(); }

	// デバッグカメラの初期化
	void DebugCameraInitialze(Input* input);

	// デバッグカメラの更新処理
	void DebugCameraUpdate();

	// カメラ注視点までの距離のセット
	void SetDistance(float distance) {
		distance_ = distance;
	}

private:// プライベート関数
	// 円周率
	const float PI = 3.141592f;

	/// <summary>
	/// 定数バッファ生成
	/// </summary>
	void CreateConstBuffer();

	/// <summary>
	/// マッピングする
	/// </summary>
	void Map();

	// 度数からラジアンに変換
	float ToRadian(float angle) { return angle * (PI / 180); }

	// ベクトルによる視点移動
	void MoveEyeVector(const Vector3& move);

	// ベクトルによる移動
	void MoveVector(const Vector3& move);



public:// パブリック変数
#pragma region ビュー行列の設定
	// 視点座標
	Vector3 eye = { 0, 0, -20.0f };
	// 注視点座標
	Vector3 target = { 0, 0, 0 };
	// 上方向ベクトル
	Vector3 up = { 0, 1, 0 };
#pragma endregion

#pragma region 射影行列の設定
	// 垂直方向視野角
	float fovAngleY = ToRadian(45.0f);
	// ビューポートのアスペクト比
	float aspectRatio = (float)WinApp::window_width / WinApp::window_height;
	// 深度限界（手前側）
	float nearZ = 0.1f;
	// 深度限界（奥側）
	float farZ = 1000.0f;
#pragma endregion

	// ビュー行列
	Matrix4 matView_;
	// 射影行列
	Matrix4 matProjection_;

private:// メンバ変数

	// デバイス（借りてくる）
	static Microsoft::WRL::ComPtr<ID3D12Device> sDevice_;

	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;

	// マッピング済みアドレス
	ConstBufferDataViewProjection* constMap_ = nullptr;

	// デバッグカメラに必要な変数
#pragma region デバッグカメラ
	// 入力クラスのポインタ
	Input* input_;
	// カメラ注視点までの距離
	float distance_ = 3;
	// スケーリング
	float scaleX_ = 1.0f;
	float scaleY_ = 1.0f;
	// 回転行列
	Matrix4 matRot_;
#pragma endregion

};

