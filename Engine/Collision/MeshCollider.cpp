#include "MeshCollider.h"
#include "Collision.h"

void MeshCollider::ConstructTriangles(Model* model)
{
	// 三角形リストをクリア
	triangles.clear();

	// モデルの持つメッシュリストを取得
	const std::vector<Mesh*>& meshes = model->GetMeshes();
	// 現在のメッシュの開始三角形番号を入れておく変数（０で初期化）
	int start = 0;
	// 全メッシュについて順に処理する
	std::vector<Mesh*>::const_iterator it = meshes.cbegin();
	for (; it != meshes.cend(); ++it) {
		Mesh* mesh = *it;
		const std::vector<Mesh::VertexPosNormalUv>& vertices = mesh->GetVertices();
		const std::vector<unsigned short>& indices = mesh->GetIndices();

		// インデックスは、三角形の数ｘ３個あるので、
		// そこからメッシュ内の三角形の数を逆算する
		size_t triangleNum = indices.size() / 3;

		// 現在のメッシュの三角形の数だけ、三角形リストにスペースを追加する
		triangles.resize(triangles.size() + triangleNum);

		// 全三角形について順に処理する
		for (int i = 0; i < triangleNum; i++) {
			// 今から計算する三角形の参照
			Triangle& tri = triangles[start + i];
			int idx0 = indices[i * 3 + 0];
			int idx1 = indices[i * 3 + 1];
			int idx2 = indices[i * 3 + 2];
			// 三角形の3頂点の座標を代入
			tri.p0 = {
				vertices[idx0].pos.x,
				vertices[idx0].pos.y,
				vertices[idx0].pos.z };

			tri.p1 = {
				vertices[idx1].pos.x,
				vertices[idx1].pos.y,
				vertices[idx1].pos.z};

			tri.p2 = {
				vertices[idx2].pos.x,
				vertices[idx2].pos.y,
				vertices[idx2].pos.z };
			// 3頂点から法線を計算
			tri.ComputeNormal();
		}
		// 次のメッシュは、今までの三角形番号の次から使う。
		start += (int)triangleNum;
	}

}

void MeshCollider::Update()
{
	// ワールド行列の逆行列を計算
	invMatWorld.MakeInverse();
}

bool MeshCollider::CheckCollisionSphere(const Sphere& sphere, Vector3* inter)
{
	// オブジェクトのローカル座標系での球を得る（半径はXスケールを参照)
	Sphere localSphere;
	Matrix4 keisanM4;
	Vector3 keisanV3;
	Vector3 inv = { invMatWorld.m[0][0],invMatWorld.m[0][1] ,invMatWorld.m[0][2] };
	localSphere.center = keisanM4.transform(sphere.center, invMatWorld);
	localSphere.radius *= inv.length();

	std::vector<Triangle>::const_iterator it = triangles.cbegin();

	for (; it != triangles.cend(); ++it) {
		const Triangle& triangle = *it;

		if (Collision::CheckSphere2Triangle(localSphere, triangle, inter)) {
			if (inter) {
				const Matrix4& matWorld = GetObject3d()->GetMatWorld();

				*inter = keisanM4.transform(*inter, matWorld);
			}
			return true;
		}
	}

	
	return false;
}

bool MeshCollider::CheckCollisionRay(const Ray& ray, float* distance, Vector3* inter)
{
	// オブジェクトのローカル座標系でのレイを得る
	Ray localRay;
	Matrix4 keisanM4;
	Vector3 keisanV3;
	localRay.start = keisanM4.transform(ray.start, invMatWorld);
	localRay.dir = keisanM4.transformNotW(ray.dir, invMatWorld);

	// ローカル座標系で交差をチェック
	std::vector<Triangle>::const_iterator it = triangles.cbegin();

	for (; it != triangles.cend(); ++it) {
		const Triangle& triangle = *it;

		Vector3 tempInter;
		// レイと三角形の当たり判定
		if (Collision::CheckRay2Triangle(localRay, triangle, nullptr, &tempInter)) {

			const Matrix4& matWorld = GetObject3d()->GetMatWorld();

			// ワールド座標系での交点を得る
			tempInter = keisanM4.transform(tempInter, matWorld);

			if (distance) {
				// 交点とレイ始点の距離を計算
				Vector3 sub = tempInter - ray.start;
				*distance = keisanV3.dot(sub, ray.dir);
			}

			if (inter) {
				*inter = tempInter;
			}

			return true;
		}
	}
	return false;
}
