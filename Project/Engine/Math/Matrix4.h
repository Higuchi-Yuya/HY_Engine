#pragma once

#include "Vector3.h"
#include<assimp/scene.h>
#include <Vector4.h>

class Matrix4
{
public:
	// Matrix4　構造体
	float m[4][4];

	// コンストラクタ
	Matrix4();
	// 成分を指定しての生成
	Matrix4(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33);

	// 単位行列を求める
	Matrix4 identity();

	// 逆行列を求める
	Matrix4 MakeInverse();

	//拡大縮小行列の設定
	Matrix4 scale(const Vector3& s);

	// 回転行列の設定
	Matrix4 rotateX(float angle);
	Matrix4 rotateY(float angle);
	Matrix4 rotateZ(float angle);

	// 回転行列のまとめて合成( 順番は Z,X,Y )
	Matrix4 rotation(Vector3 angle);

	// 平行移動行列の作成
	Matrix4 translate(const Vector3& t);

	// 座標変換（ベクトルと行列の掛け算をする）
	static Vector3 transform(const Vector3& v, const Matrix4& m);

	// 座標変換（ベクトルと行列の掛け算をする）
	static Vector3 transformNotW(const Vector3& v, const Matrix4& m);

	// ビュー行列作成
	Matrix4 ViewMat(Vector3 eye, Vector3 target, Vector3 up);

	// 射影行列作成
	Matrix4 ProjectionMat(float fovAngleY, float aspectRatio, float nearZ, float farZ);

	// マットリックスの掛け算を返す
	Matrix4 MatMul(const Matrix4& Mat);

	// Vec4の掛け
	Vector4 Vec4MulPs(const Vector4& v4_1, const Vector4& v4_2);

	// Vec4の足し
	Vector4 Vec4AddPs(const Vector4& v4_1, const Vector4& v4_2);

	// 代入演算子オーバーロード
	Matrix4 operator - () const;
	Matrix4& operator*=(const Matrix4& m1);

	Matrix4 operator*(const Matrix4& m1);

private:

};

// 2項演算子オーバーロード

Vector3 operator*(const Vector3& v, const Matrix4& m);

Matrix4 operator*(const Matrix4& m1, const Matrix4& m2);