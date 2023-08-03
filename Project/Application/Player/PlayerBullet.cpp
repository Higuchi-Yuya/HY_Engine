#include "PlayerBullet.h"

void PlayerBullet::Initialize(Model* model, const WorldTransform worldTransform, const Vector3& velocity) {
	//NULLポインタチェック
	assert(model);

	SetModel(model);

	//引数で受け取った速度をメンバ変数に代入
	velocity_ = velocity;

	//ワールドトランスフォームの初期化
	Object3d::Initialize();

	//引数で受け取った初期座標をセット
	worldTransform_.translation = {
	  worldTransform.matWorld_.m[3][0], worldTransform.matWorld_.m[3][1],
	  worldTransform.matWorld_.m[3][2] };

	worldTransform_.scale.x = 0.6f;
	worldTransform_.scale.y = 0.6f;
	worldTransform_.scale.z = 0.6f;

	// 打つ方向に向けてオブジェクトを回転させる

	// Y軸周り角度(θy)
	worldTransform_.rotation.y = std::atan2(velocity_.x, velocity_.z);
	// Y軸周りに-θy回す回転行列を計算
	Matrix4 RotY;
	Matrix4 mathMat;
	RotY.rotateY(-worldTransform_.rotation.y);
	// velosity_に回転行列を掛け算してvelosityZを求める
	Vector3 velosityZ = velocity_;
	velosityZ = mathMat.transformNotW(velosityZ, RotY);

	// X軸周り角度(θx)
	worldTransform_.rotation.x = std::atan2(-velosityZ.y, velosityZ.z);
}

void PlayerBullet::Update()
{
	//座標を移動させる(１フレーム分の移動量を足しこむ)
	worldTransform_.translation += velocity_;

	worldTransform_.UpdateMatrix();

	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void PlayerBullet::Draw(ViewProjection* view)
{
	Object3d::Draw(view);
}

Vector3 PlayerBullet::GetBulletWorldPosition()
{
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.translation.x;
	worldPos.y = worldTransform_.translation.y;
	worldPos.z = worldTransform_.translation.z;

	return worldPos;
}

void PlayerBullet::OnCollision() { isDead_ = true; }
