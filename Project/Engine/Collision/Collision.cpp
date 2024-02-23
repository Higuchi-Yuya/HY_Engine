#include "Collision.h"
#include <cmath>


bool Collision::CheckSphere2Plane(const Sphere& sphere, const Plane& plane, Vector3* inter)
{
    // 座標系の原点から球の中心座標への距離
    Vector3 V;
    float distV = V.dot(sphere.center, plane.normal);
    // 平面の原点距離を減算することで、平面と球の中心との距離が出る
    float dist = distV - plane.distance;
    // 距離の絶対値が半径より大きければ当たっていない
    if (fabsf(dist) > sphere.radius) return false;

    // 疑似交点を計算
    if (inter) {
        // 平面上の最近接点を、疑似交点とする
        *inter = -dist * plane.normal + sphere.center;
    }

    return true;
}

bool Collision::CheckSphere2Sphere(const Sphere& sphereA, const Sphere& sphereB, Vector3* inter, Vector3* reject)
{
	float x = (sphereB.center.x - sphereA.center.x) * (sphereB.center.x - sphereA.center.x);
	float y = (sphereB.center.y - sphereA.center.y) * (sphereB.center.y - sphereA.center.y);
	float z = (sphereB.center.z - sphereA.center.z) * (sphereB.center.z - sphereA.center.z);

	float dist = x + y + z;
	float radius = (sphereA.radius + sphereB.radius) * (sphereA.radius + sphereB.radius);

	if (dist <= radius) {
		if (inter) {
			// Aの半径が0の時座標はBの中心  Bの半径が0の時座標はAの中心  となるように補間
			float t = sphereB.radius / (sphereA.radius + sphereB.radius);
			*inter = Vector3::lerp(sphereA.center, sphereB.center, t);
		}
		if (reject) {
			float rejectLen = sphereA.radius + sphereB.radius - sqrtf(dist);
			Vector3 center = sphereA.center - sphereB.center;
			*reject = center.normalize();
			*reject *= rejectLen;
		
		}
		return true;
	}
	return false;
}

bool Collision::CheckSphere2SphereOiOi(const Sphere& sphereA, const Sphere& sphereB, Vector3* inter, Vector3* reject)
{
	// 中心点の距離の２乗 <= 半径の和の２乗　なら交差
	Vector3 tmp;
	tmp = sphereA.center - sphereB.center;
	float dist = tmp.dot(tmp, tmp);
	float radius2 = sphereA.radius + sphereB.radius;
	radius2 *= radius2;

	if (dist <= radius2)
	{
		if (inter)
		{
			// Aの半径が0の時座標はBの中心　Bの半径が0の時座標はAの中心　となるよう補完
			float t = sphereB.radius / (sphereA.radius + sphereB.radius);
			*inter = tmp.lerp(sphereA.center, sphereB.center, t);
		}
		// 押し出すベクトルを計算
		if (reject)
		{
			float rejectLen = sphereA.radius + sphereB.radius - sqrtf(dist);
			tmp = sphereA.center - sphereB.center;
			*reject = tmp.normalize();
			*reject *= rejectLen;
		}
		return true;
	}

	
	return false;
}

void Collision::ClosestPtPoint2Triangle(const Vector3& point, const Triangle& triangle, Vector3* closest)
{
	// pointがp0の外側の頂点領域の中にあるかどうかチェック
	Vector3 p0_p1 = triangle.p1 - triangle.p0;
	Vector3 p0_p2 = triangle.p2 - triangle.p0;
	Vector3 p0_pt = point - triangle.p0;

	Vector3 VV;
	float d1 = VV.dot(p0_p1, p0_pt);
	float d2 = VV.dot(p0_p2, p0_pt);

	if (d1 <= 0.0f && d2 <= 0.0f)
	{
		// p0が最近傍
		*closest = triangle.p0;
		return;
	}

	// pointがp1の外側の頂点領域の中にあるかどうかチェック
	Vector3 p1_pt = point - triangle.p1;

	float d3 = VV.dot(p0_p1, p1_pt);
	float d4 = VV.dot(p0_p2, p1_pt);

	if (d3 >= 0.0f && d4 <= d3)
	{
		// p1が最近傍
		*closest = triangle.p1;
		return;
	}

	// pointがp0_p1の辺領域の中にあるかどうかチェックし、あればpointのp0_p1上に対する射影を返す
	float vc = d1 * d4 - d3 * d2;
	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
	{
		float v = d1 / (d1 - d3);
		*closest = triangle.p0 + v * p0_p1;
		return;
	}

	// pointがp2の外側の頂点領域の中にあるかどうかチェック
	Vector3 p2_pt = point - triangle.p2;

	float d5 = VV.dot(p0_p1, p2_pt);
	float d6 = VV.dot(p0_p2, p2_pt);
	if (d6 >= 0.0f && d5 <= d6)
	{
		*closest = triangle.p2;
		return;
	}

	// pointがp0_p2の辺領域の中にあるかどうかチェックし、あればpointのp0_p2上に対する射影を返す
	float vb = d5 * d2 - d1 * d6;
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
	{
		float w = d2 / (d2 - d6);
		*closest = triangle.p0 + w * p0_p2;
		return;
	}

	// pointがp1_p2の辺領域の中にあるかどうかチェックし、あればpointのp1_p2上に対する射影を返す
	float va = d3 * d6 - d5 * d4;
	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
	{
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		*closest = triangle.p1 + w * (triangle.p2 - triangle.p1);
		return;
	}

	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	*closest = triangle.p0 + p0_p1 * v + p0_p2 * w;
}

bool Collision::CheckSphere2Triangle(const Sphere& sphere, const Triangle& triangle, Vector3* inter, Vector3* reject)
{
	Vector3 p;
	// 球の中心に対する最近接点である三角形上にある点pを見つける
	ClosestPtPoint2Triangle(sphere.center, triangle, &p);

	// 点pと球の中心の差分ベクトル
	Vector3 v = p - sphere.center;
	Vector3 VV;
	// 距離の二乗を求める
	// （同じベクトル同士の内積は三平方の定理のルート内部の式と一致する）
	float vvv = VV.dot(v, v);

	// 球と三角形の距離が半径以下なら当たっていない
	if (vvv > sphere.radius * sphere.radius)return false;

	// 疑似交点を計算
	if (inter) {
		// 三角形上の最近接点pを疑似交点とする
		*inter = p;
	}
	// 押し出すベクトルを計算
	if (reject) {
		float ds = Vector3::dot(sphere.center, triangle.normal);
		float dt = Vector3::dot(triangle.p0, triangle.normal);
		float rejectLen = dt - ds + sphere.radius;
		*reject = triangle.normal * rejectLen;
	}
	return true;
}

bool Collision::CheckRay2Plane(const Ray& ray, const Plane& plane, float* distance, Vector3* inter)
{
	const float epsilon = 1.0e-5f; // 誤差吸収用の微小な値
	// 面法線トレイの方向ベクトルの内積
	Vector3 V;
	float d1 = V.dot(plane.normal, ray.dir);
	// 裏面には当たらない
	if (d1 > -epsilon) { return false; }
	// 始点と原点の距離（平面の法線方向）
	// 面法線トレイの視点座標（位置ベクトル）の内積
	float d2 = V.dot(plane.normal, ray.start);
	// 始点と平面の距離（平面の法線方向）
	float dist = d2 - plane.distance;
	// 始点と平面の距離（レイ方向）
	float t = dist / -d1;
	// 交点が始点より後ろにあるので、当たらない
	if (t < 0)return false;
	// 距離を書き込む
	if (distance) { *distance = t; }

	// 交点を計算
	if (inter) { *inter = ray.start + t * ray.dir; }

	return true;
}

bool Collision::CheckRay2Triangle(const Ray& ray, const Triangle& triangle, float* distance, Vector3* inter)
{
	// 三角形が乗っている平面を算出
	Plane plane;
	Vector3 interPlane;
	Vector3 V;
	plane.normal = triangle.normal;
	plane.distance = V.dot(triangle.normal, triangle.p0);

	// レイと平面が当たっていなければ、当たっていない
	if (!CheckRay2Plane(ray, plane, distance, &interPlane)) { return false; }

	// レイと平面が当たっていたので、距離と交点が書き込まれた
	// レイと平面の交点が三角形の内側にあるか判定
	const float epsilon = 1.0e-5f;// 誤差吸収用の微小な値
	Vector3 m;

	// 辺p0_p1について
	Vector3 pt_p0 = triangle.p0 - interPlane;
	Vector3 p0_p1 = triangle.p1 - triangle.p0;
	m = pt_p0.cross(p0_p1);

	// 辺の外側であれば当たっていないので判定を打ち切る
	if (V.dot(m, triangle.normal) < -epsilon) { return false; }

	// 辺p1_p2について
	Vector3 pt_p1 = triangle.p1 - interPlane;
	Vector3 p1_p2 = triangle.p2 - triangle.p1;
	m = pt_p1.cross(p1_p2);

	// 辺の外側であれば当たっていないので判定を打ち切る
	if (V.dot(m, triangle.normal) < -epsilon) { return false; }

	// 辺p2_p0について
	Vector3 pt_p2 = triangle.p2 - interPlane;
	Vector3 p2_p0 = triangle.p0 - triangle.p2;
	m = pt_p2.cross(p2_p0);

	// 辺の外側であれば当たっていないので判定を打ち切る
	if (V.dot(m, triangle.normal) < -epsilon) { return false; }

	// 内側なので、当たっている
	if (inter) {
		*inter = interPlane;
	}

	return true;
}

bool Collision::CheckRay2Sphere(const Ray& ray, const Sphere& sphere, float* distance, Vector3* inter)
{
	Vector3 m = ray.start - sphere.center;
	Vector3 V;
	float b = V.dot(m, ray.dir);
	float c = V.dot(m, m) - sphere.radius * sphere.radius;

	// rayの始点がsphereの外側にあり(c>0)、rayがsphereから離れていく方向を差している場合(b>0)、当たらない
	if (c > 0.0f && b > 0.0f) { return false; }

	float discr = b * b - c;

	// 負の判別式は例が球を外れていることに一致
	if (discr < 0.0f) { return false; }

	// レイは球と交差している
	// 交差する最小の値tを計算
	float t = -b - sqrtf(discr);

	// ｔが負である場合、レイは球の内側から開始しているのでｔをゼロにクランプ
	if (t < 0)t = 0.0f;
	if (distance) { *distance = t; }

	if (inter) { *inter = ray.start + t * ray.dir; }

	return true;
}

bool Collision::CheckSphere2AABB(const Sphere& sphere, const WorldTransform& transA, Vector3* inter, Vector3* reject)
{
	Box box;
	// ボックス構造体に情報を登録
	box.center = transA.translation;
	box.maxRadius = (transA.maxVertex_ * transA.scale);
	box.maxRadius.x = abs(box.maxRadius.x);
	box.maxRadius.y = abs(box.maxRadius.y);
	box.maxRadius.z = abs(box.maxRadius.z);

	// 中心点の距離の２乗 <= 半径の和の２乗　なら交差
	Vector3 tmp = sphere.center - box.center;
	float distx = (box.center.x - sphere.center.x) * (box.center.x - sphere.center.x);
	float disty = (box.center.y - sphere.center.y) * (box.center.y - sphere.center.y);
	float distz = (box.center.z - sphere.center.z) * (box.center.z - sphere.center.z);

	//float dist = Vector3::dot(tmp, tmp);
	Vector3 radius = {
		sphere.radius + box.maxRadius.x,
		sphere.radius + box.maxRadius.y,
		sphere.radius + box.maxRadius.z
	};
	radius *= radius;

	if (distx <= radius.x&&
		disty <= radius.y&&
		distz <= radius.z) {

		if (inter) {

		}

		// 押し出しのベクトル計算
		if (reject) {
			// 球の中心から半径を加算した時のポジション
			Vector3 sphereColP;
			sphereColP.x = sphere.center.x;
			sphereColP.y = sphere.center.y;
			sphereColP.z = sphere.center.z;
			Vector3 sphereColM;
			sphereColM.x = sphere.center.x;
			sphereColM.y = sphere.center.y;
			sphereColM.z = sphere.center.z;


			// キューブの中心から半径を加算や減算したポジション
			Vector3 cubeColP;
			cubeColP.x = box.center.x + box.maxRadius.x;
			cubeColP.y = box.center.y + box.maxRadius.y;
			cubeColP.z = box.center.z + box.maxRadius.z;
			Vector3 cubeColM;
			cubeColM.x = box.center.x - box.maxRadius.x;
			cubeColM.y = box.center.y - box.maxRadius.y;
			cubeColM.z = box.center.z - box.maxRadius.z;

			Vector3 rejectLen;

			// 前方に押し出し
			if (sphereColP.z <= cubeColM.z &&
			   (sphereColM.x <= cubeColP.x && sphereColP.x >= cubeColM.x)) {

				rejectLen.z = sphere.radius + box.maxRadius.z - sqrtf(distz);
			}
			// 後方に押し出し
			if (sphereColM.z >= cubeColP.z &&
			   (sphereColM.x <= cubeColP.x && sphereColP.x >= cubeColM.x)) {

				rejectLen.z = sphere.radius + box.maxRadius.z - sqrtf(distz);
			}
			// 右方に押し出し
			if (sphereColM.x >= cubeColP.x &&
			   (sphereColM.z <= cubeColP.z && sphereColP.z >= cubeColM.z)) {

				rejectLen.x = sphere.radius + box.maxRadius.x - sqrtf(distx);
			}
			// 左方に押し出し
			if (sphereColP.x <= cubeColM.x &&
			   (sphereColM.z <= cubeColP.z && sphereColP.z >= cubeColM.z)) {

				rejectLen.x = sphere.radius + box.maxRadius.x - sqrtf(distx);
			}

			Vector3 center = sphere.center - box.center;

			*reject = center.normalize();
			*reject *= rejectLen;

		}
		return true;
	}
	return false;
}

bool Collision::CheckAABB(const WorldTransform& transA, const WorldTransform& transB)
{
	Box a,b;
	// ボックス構造体に情報を登録
	a.center = transA.translation;
	a.minRadius = transA.minVertex_ * transA.scale;
	a.maxRadius = transA.maxVertex_ * transA.scale;

	b.center = transB.translation;
	b.minRadius = transB.minVertex_ * transB.scale;
	b.maxRadius = transB.maxVertex_ * transB.scale;

	// ボックス構造体の中心点からのプラスとマイナスのそれぞれ半径を算出
	a.minCenterRadius = a.center + a.minRadius;
	a.maxCenterRadius = a.center + a.maxRadius;

	b.minCenterRadius = b.center + b.minRadius;
	b.maxCenterRadius = b.center + b.maxRadius;

	// これらの条件が合ったなら当たっていない
	if (a.minCenterRadius.x > b.maxCenterRadius.x ||
		a.maxCenterRadius.x < b.minCenterRadius.x ||
		a.minCenterRadius.y > b.maxCenterRadius.y ||
		a.maxCenterRadius.y < b.minCenterRadius.y ||
		a.minCenterRadius.z > b.maxCenterRadius.z ||
		a.maxCenterRadius.z < b.minCenterRadius.z) {
		return false;
	}

	// 上の条件が通っていなないなら当たっている
	return true;
}

bool Collision::CheckOBB(const WorldTransform& obbA, const WorldTransform& obbB)
{
	OBB a, b;
	// ポジションのセット
	a.centerPos = obbA.translation;
	// 各軸の方向ベクトル x,y,z
	a.directionVector[0] = { obbA.matWorld_.m[0][0],obbA.matWorld_.m[0][1],obbA.matWorld_.m[0][2] };
	a.directionVector[1] = { obbA.matWorld_.m[1][0],obbA.matWorld_.m[1][1],obbA.matWorld_.m[1][2] };
	a.directionVector[2] = { obbA.matWorld_.m[2][0],obbA.matWorld_.m[2][1],obbA.matWorld_.m[2][2] };

	// ポジションのセット
	b.centerPos = obbB.translation;
	// 各軸の方向ベクトル x,y,z
	b.directionVector[0] = { obbB.matWorld_.m[0][0],obbB.matWorld_.m[0][1],obbB.matWorld_.m[0][2] };
	b.directionVector[1] = { obbB.matWorld_.m[1][0],obbB.matWorld_.m[1][1],obbB.matWorld_.m[1][2] };
	b.directionVector[2] = { obbB.matWorld_.m[2][0],obbB.matWorld_.m[2][1],obbB.matWorld_.m[2][2] };

	// ボックス構造体に情報を登録
	a.minRadius = obbA.minVertex_ * obbA.scale;
	a.maxRadius = obbA.maxVertex_ * obbA.scale;

	b.minRadius = obbB.minVertex_ * obbB.scale;
	b.maxRadius = obbB.maxVertex_ * obbB.scale;

	// 各軸の長さ
	a.shaftLength[0] = (abs(a.maxRadius.x) + abs(a.minRadius.x)) * 0.5f;
	a.shaftLength[1] = (abs(a.maxRadius.y) + abs(a.minRadius.y)) * 0.5f;
	a.shaftLength[2] = (abs(a.maxRadius.z) + abs(a.minRadius.z)) * 0.5f;

	b.shaftLength[0] = (abs(b.maxRadius.x) + abs(b.minRadius.x)) * 0.5f;
	b.shaftLength[1] = (abs(b.maxRadius.y) + abs(b.minRadius.y)) * 0.5f;
	b.shaftLength[2] = (abs(b.maxRadius.z) + abs(b.minRadius.z)) * 0.5f;

	// 各方向ベクトルの確保
	// （N***:標準化方向ベクトル）
	Vector3 NAe1 = a.directionVector[0], Ae1 = NAe1 * a.shaftLength[0];
	Vector3 NAe2 = a.directionVector[1], Ae2 = NAe2 * a.shaftLength[1];
	Vector3 NAe3 = a.directionVector[2], Ae3 = NAe3 * a.shaftLength[2];
	Vector3 NBe1 = b.directionVector[0], Be1 = NBe1 * b.shaftLength[0];
	Vector3 NBe2 = b.directionVector[1], Be2 = NBe2 * b.shaftLength[1];
	Vector3 NBe3 = b.directionVector[2], Be3 = NBe3 * b.shaftLength[2];
	Vector3 Interval = a.centerPos - b.centerPos;

	// 分離軸 : Ae1
	Vector3 mathVec3;
	FLOAT rA = mathVec3.length(Ae1);
	FLOAT rB = LenSegOnSeparateAxis(&NAe1, &Be1, &Be2, &Be3);
	FLOAT L = fabs(mathVec3.dot(Interval, NAe1));
	if (L > rA + rB)
		return false; // 衝突していない

	 // 分離軸 : Ae2
	rA = mathVec3.length(Ae2);
	rB = LenSegOnSeparateAxis(&NAe2, &Be1, &Be2, &Be3);
	L = fabs(mathVec3.dot(Interval, NAe2));
	if (L > rA + rB)
		return false;

	// 分離軸 : Ae3
	rA = mathVec3.length(Ae3);
	rB = LenSegOnSeparateAxis(&NAe3, &Be1, &Be2, &Be3);
	L = fabs(mathVec3.dot(Interval, NAe3));
	if (L > rA + rB)
		return false;

	// 分離軸 : Be1
	rA = LenSegOnSeparateAxis(&NBe1, &Ae1, &Ae2, &Ae3);
	rB = mathVec3.length(Be1);
	L = fabs(mathVec3.dot(Interval, NBe1));
	if (L > rA + rB)
		return false;

	// 分離軸 : Be2
	rA = LenSegOnSeparateAxis(&NBe2, &Ae1, &Ae2, &Ae3);
	rB = mathVec3.length(Be2);
	L = fabs(mathVec3.dot(Interval, NBe2));
	if (L > rA + rB)
		return false;

	// 分離軸 : Be3
	rA = LenSegOnSeparateAxis(&NBe3, &Ae1, &Ae2, &Ae3);
	rB = mathVec3.length(Be3);
	L = fabs(mathVec3.dot(Interval, NBe3));
	if (L > rA + rB)
		return false;

	// 分離軸 : C11
	Vector3 Cross;
	Cross = mathVec3.cross(NAe1, NBe1);
	rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3);
	L = fabs(mathVec3.dot(Interval, Cross));
	if (L > rA + rB)
		return false;

	// 分離軸 : C12
	Cross = mathVec3.cross(NAe1, NBe2);
	rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3);
	L = fabs(mathVec3.dot(Interval, Cross));
	if (L > rA + rB)
		return false;

	// 分離軸 : C13
	Cross = mathVec3.cross(NAe1, NBe3);
	rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2);
	L = fabs(mathVec3.dot(Interval, Cross));
	if (L > rA + rB)
		return false;

	// 分離軸 : C21
	Cross = mathVec3.cross(NAe2, NBe1);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3);
	L = fabs(mathVec3.dot(Interval, Cross));
	if (L > rA + rB)
		return false;

	// 分離軸 : C22
	Cross = mathVec3.cross(NAe2, NBe2);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3);
	L = fabs(mathVec3.dot(Interval, Cross));
	if (L > rA + rB)
		return false;

	// 分離軸 : C23
	Cross = mathVec3.cross(NAe2, NBe3);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2);
	L = fabs(mathVec3.dot(Interval, Cross));
	if (L > rA + rB)
		return false;

	// 分離軸 : C31
	Cross = mathVec3.cross(NAe3, NBe1);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2);
	rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3);
	L = fabs(mathVec3.dot(Interval, Cross));
	if (L > rA + rB)
		return false;

	// 分離軸 : C32
	Cross = mathVec3.cross(NAe3, NBe2);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3);
	L = fabs(mathVec3.dot(Interval, Cross));
	if (L > rA + rB)
		return false;

	// 分離軸 : C33
	Cross = mathVec3.cross(NAe3, NBe3);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2);
	L = fabs(mathVec3.dot(Interval, Cross));
	if (L > rA + rB)
		return false;

	// 分離平面が存在しないので「衝突している」
	return true;
}

float Collision::LenSegOnSeparateAxis(Vector3* Sep, Vector3* e1, Vector3* e2, Vector3* e3)
{
	Vector3 mathVec3;
	// 3つの内積の絶対値の和で投影線分長を計算
   // 分離軸Sepは標準化されていること
	FLOAT r1 = fabs(mathVec3.dot(*Sep, *e1));
	FLOAT r2 = fabs(mathVec3.dot(*Sep, *e2));
	FLOAT r3 = e3 ? (fabs(mathVec3.dot(*Sep, *e3))) : 0;

	return r1 + r2 + r3;
}

bool Collision::CheckSphereToAABB2D(const Sphere& sphere, const WorldTransform& transA, Vector3* inter, Vector3* reject)
{
	inter, reject;
	Box box;
	// ボックス構造体に情報を登録
	box.center = transA.translation;
	box.maxRadius = (transA.maxVertex_ * transA.scale);
	box.maxRadius.x = abs(box.maxRadius.x);
	box.maxRadius.y = abs(box.maxRadius.y);
	box.maxRadius.z = abs(box.maxRadius.z);

	// 円の中心と矩形のtransAの間の距離の平方を計算
	float sqDist = SquareDistance_PointRectangle(sphere.center, box.center, box.maxRadius);

	// 円と矩形 r が交差するのは、それらの間の平方した距離が
	// 平方した円の半径よりも小さい場合
	if (sqDist <= sphere.radius * sphere.radius) {
		return true;
	}
	return false;
}

bool Collision::CheckSphereToAABB2D(const Sphere& sphere, const Box& box, WorldTransform* trans)
{
	Vector3 leftTopPos;
	Vector3 rightTopPos;
	Vector3 leftDownPos;
	Vector3 rightDownPos;

	Vector3 sc = sphere.center;

	leftTopPos.x = box.center.x - box.radius.x;
	leftTopPos.z = box.center.z + box.radius.z;

	rightTopPos.x = box.center.x + box.radius.x;
	rightTopPos.z = box.center.z + box.radius.z;

	leftDownPos.x= box.center.x - box.radius.x;
	leftDownPos.z = box.center.z - box.radius.z;

	rightDownPos.x = box.center.x + box.radius.x;
	rightDownPos.z = box.center.z - box.radius.z;

	bool A = false, B = false, C = false, D = false, E = false, F = false;

	C, D, E, F;
	// 上下の範囲判定
	if (sc.x > leftTopPos.x &&
		sc.x < rightDownPos.x &&
		sc.z < leftTopPos.z + sphere.radius&&
		sc.z > rightDownPos.z - sphere.radius ){
		if (trans != nullptr) {
			// 上へ押し出し
			if (sc.z > box.center.z) {
				trans->translation.z = box.center.z + (box.radius.z + sphere.radius) + 0.1f;
			}
			// 下への押し出し
			if (sc.z < box.center.z) {
				trans->translation.z = box.center.z - (box.radius.z + sphere.radius) - 0.1f;
			}
		}
		A = true;
	}
	// 左右の範囲判定
	if (sc.x > leftTopPos.x - sphere.radius &&
		sc.x < rightDownPos.x + sphere.radius &&
		sc.z < leftTopPos.z  &&
		sc.z > rightDownPos.z ) {
		if (trans != nullptr) {
			// 右へ押し出し
			if (sc.x > box.center.x) {
				trans->translation.x = box.center.x + (box.radius.x + sphere.radius) + 0.1f;
			}
			// 左への押し出し
			if (sc.x < box.center.x) {
				trans->translation.x = box.center.x - (box.radius.x + sphere.radius) - 0.1f;
			}
		}
		B = true;
	}
	// 左上の範囲判定
	if (((leftTopPos.x - sc.x) * (leftTopPos.x - sc.x)) +
		((leftTopPos.z - sc.z) * (leftTopPos.z - sc.z)) < sphere.radius * sphere.radius) {
		C = true;
	}
	// 右上の範囲判定
	if (((rightDownPos.x - sc.x) * (rightDownPos.x - sc.x)) +
		((leftTopPos.z - sc.z) * (leftTopPos.z - sc.z)) < sphere.radius * sphere.radius) {
		D = true;
	}
	// 左下の範囲判定
	if (((rightDownPos.x - sc.x) * (rightDownPos.x - sc.x)) +
		((rightDownPos.z - sc.z) * (rightDownPos.z - sc.z)) < sphere.radius * sphere.radius) {
		E = true;
	}
	// 右下の範囲判定
	if (((leftTopPos.x - sc.x) * (leftTopPos.x - sc.x)) +
		((rightDownPos.z - sc.z) * (rightDownPos.z - sc.z)) < sphere.radius * sphere.radius) {
		F = true;
	}

	// どれかの条件が通ったら当たった判定をとる
	if (A == true || B == true ||
		C == true || D == true ||
		E == true || F == true) {

		return true;
	}
	return false;
}

float Collision::SquareDistance_PointRectangle(Vector3 p, Vector3 c, Vector3 r)
{
	float sqDist = 0.0f;
	if (p.x < c.x) {
		sqDist += (r.x - p.x)* (r.x - p.x);
	}
	if (p.x > c.x + r.x) {
		sqDist += (p.x - c.x - r.x) * (p.x - c.x - r.x);
	}
	if (p.z < c.z) {
		sqDist += (r.z - p.z) * (r.z - p.z);
	}
	if (p.z > c.z + r.z) {
		sqDist += (p.z - c.z - r.z) * (p.z - c.z - r.z);
	}

	return sqDist;
}

bool Collision::CheckSphereToCone(const Sphere& sphere, const Cone& cone)
{
	// 球体の中心から円錐の頂点へのベクトル
	Vector3 sphereToCone = Vector3(cone.vertexPos.x - sphere.center.x, cone.vertexPos.y - sphere.center.y, cone.vertexPos.z - sphere.center.z);

	// 円錐の高さのベクトル
	Vector3 coneAxis = cone.directionVec;

	// 円錐の頂点から底面へのベクトル
	Vector3 coneToBase = Vector3(cone.vertexPos.x, cone.vertexPos.y, cone.vertexPos.z);
	coneToBase.y -= cone.height; // 円錐の底面に移動

	// 円錐の軸方向に対する球体中心からの距離
	float dotProduct = (sphereToCone.x * coneAxis.x) + (sphereToCone.y * coneAxis.y) + (sphereToCone.z * coneAxis.z);

	// 円錐の頂点から球体中心までの距離
	float distanceToVertex = std::sqrt((sphereToCone.x * sphereToCone.x) + (sphereToCone.y * sphereToCone.y) + (sphereToCone.z * sphereToCone.z));

	// 円錐の底面から球体中心までの距離
	float distanceToBase = std::sqrt((coneToBase.x * coneToBase.x) + (coneToBase.y * coneToBase.y) + (coneToBase.z * coneToBase.z));

	// 球体が円錐の側面にあるかどうかをチェック
	if (dotProduct > 0 && dotProduct < distanceToBase) {
		float radiusAtSphereHeight = (dotProduct / distanceToBase) * cone.radius;
		float distanceToAxis = std::sqrt((distanceToVertex * distanceToVertex) - (radiusAtSphereHeight * radiusAtSphereHeight));
		if (distanceToAxis <= sphere.radius) {
			return true; // 交差している
		}
	}

	// 球体が円錐の底面にあるかどうかをチェック
	if (distanceToVertex <= cone.radius && sphere.center.y >= cone.vertexPos.y - cone.height) {
		return true; // 交差している
	}

	return false; // 交差していない

}
