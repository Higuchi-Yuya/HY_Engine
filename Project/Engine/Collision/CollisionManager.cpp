#include "CollisionManager.h"
#include "BaseCollider.h"
#include "Collision.h"
#include "MeshCollider.h"


CollisionManager* CollisionManager::GetInstance()
{
	static CollisionManager instance;

	return &instance;
}

bool CollisionManager::Raycast(const Ray& ray, RaycastHit* hitInfo, float maxDistance)
{
	return Raycast(ray, 0xffff, hitInfo, maxDistance);
}

bool CollisionManager::Raycast(const Ray& ray, unsigned short attribute, RaycastHit* hitInfo, float maxDistance)
{
	bool result = false;
	// 走査用のイテレータ
	std::forward_list<BaseCollider*>::iterator it;
	// 今までで最も近いコライダーを記録するためのイテレータ
	std::forward_list<BaseCollider*>::iterator it_hit;
	// 今までで最も近いコライダーの距離を記録する変数
	float distance = maxDistance;
	// 今までで最も近いコライダーとの交点を記録する変数
	Vector3 inter;

	it = colliders_.begin();
	for (; it != colliders_.end(); ++it) {
		BaseCollider* colA = *it;

		// 属性が合わなければスキップ
		if (!(colA->attribute_ & attribute)) {
			continue;
		}

		// 球の場合
		if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE) {
			Sphere* sphere = dynamic_cast<Sphere*>(colA);
			float tempDistance;
			Vector3 tempInter;
			// 当たらなければ除外
			if (!Collision::CheckRay2Sphere(ray, *sphere, &tempDistance, &tempInter))continue;
			// 距離が最小でなければ除外
			if (tempDistance >= distance)continue;
			// 今までで最も近いので記録をとる
			result = true;
			distance = tempDistance;
			inter = tempInter;
			it_hit = it;

		}
		// メッシュの場合
		else if (colA->GetShapeType() == COLLISIONSHAPE_MESH) {
			MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(colA);

			float tempDistance;
			Vector3 tempInter;
			if (!meshCollider->CheckCollisionRay(ray, &tempDistance, &tempInter)) continue;
			if (tempDistance >= distance) continue;

			result = true;
			distance = tempDistance;
			inter = tempInter;
			it_hit = it;
		}
	}
	if (result && hitInfo) {
		hitInfo->distance = distance;
		hitInfo->inter = inter;
		hitInfo->collider = *it_hit;
		//hitInfo->object = hitInfo->collider->GetObject3d();
	}

	return result;
}

void CollisionManager::QuerySphere(const Sphere& sphere, QueryCallback* callback, unsigned short attribute)
{
	assert(callback);

	std::forward_list<BaseCollider*>::iterator it;

	// 全てのコライダーと総当りチェック
	it = colliders_.begin();
	for (; it != colliders_.end(); ++it) {
		BaseCollider* col = *it;

		// 属性が合わなければスキップ
		if (!(col->attribute_ & attribute)) {
			continue;
		}

		// 球
		if (col->GetShapeType() == COLLISIONSHAPE_SPHERE) {
			Sphere* sphereB = dynamic_cast<Sphere*>(col);

			Vector3 tempInter;
			Vector3 tempReject;
			if (!Collision::CheckSphere2Sphere(sphere, *sphereB, &tempInter, &tempReject)) continue;

			// 交差情報をセット
			QueryHit info;
			info.collider = col;
			//info.object = col->GetObject3d();
			info.inter = tempInter;
			info.reject = tempReject;

			// クエリーコールバック呼び出し
			if (!callback->OnQueryHit(info)) {
				// 戻り値がfalseの場合、継続せず終了
				return;
			}
		}
		// メッシュ
		else if (col->GetShapeType() == COLLISIONSHAPE_MESH) {
			MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(col);

			Vector3 tempInter;
			Vector3 tempReject;
			if (!meshCollider->CheckCollisionSphere(sphere, &tempInter, &tempReject)) continue;

			// 交差情報をセット
			QueryHit info;
			info.collider = col;
			//info.object = col->GetObject3d();
			info.inter = tempInter;
			info.reject = tempReject;

			// クエリーコールバック呼び出し
			if (!callback->OnQueryHit(info)) {
				// 戻り値がfalseの場合、継続せず終了
				return;
			}
		}
	}
}

void CollisionManager::CheckAllCollisions()
{
	std::forward_list<BaseCollider*>::iterator itA;
	std::forward_list<BaseCollider*>::iterator itB;

	// 全ての組み合わせについて総当たりチェック
	itA = colliders_.begin();
	for (; itA != colliders_.end(); ++itA) {
		itB = itA;
		++itB;
		for (; itB != colliders_.end(); ++itB) {
			BaseCollider* colA = *itA;
			BaseCollider* colB = *itB;

			// ともに球
			if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE &&
				colB->GetShapeType() == COLLISIONSHAPE_SPHERE) {
				Sphere* SphereA = dynamic_cast<Sphere*>(colA);
				Sphere* SphereB = dynamic_cast<Sphere*>(colB);
				Vector3 inter;
				if (Collision::CheckSphere2Sphere(*SphereA, *SphereB, &inter)) {
					// 後でやる部分------★
					//colA->OnCollision(CollisionInfo(colB->GetObject3d(), colB, inter));
					//colB->OnCollision(CollisionInfo(colA->GetObject3d(), colA, inter));
				}
			}
			else if (colA->GetShapeType() == COLLISIONSHAPE_MESH &&
				colB->GetShapeType() == COLLISIONSHAPE_SPHERE) {
				MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(colA);
				Sphere* sphere = dynamic_cast<Sphere*>(colB);
				Vector3 inter;
				if (meshCollider->CheckCollisionSphere(*sphere, &inter)) {
					//colA->OnCollision(CollisionInfo(colB->GetObject3d(), colB, inter));
					//colB->OnCollision(CollisionInfo(colA->GetObject3d(), colA, inter));
				}
			}
			else if (colA->GetShapeType() == COLLISIONSHAPE_SPHERE &&
				colB->GetShapeType() == COLLISIONSHAPE_MESH) {
				MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(colB);
				Sphere* sphere = dynamic_cast<Sphere*>(colA);
				Vector3 inter;
				if (meshCollider->CheckCollisionSphere(*sphere, &inter)) {
					//colA->OnCollision(CollisionInfo(colB->GetObject3d(), colB, inter));
					//colB->OnCollision(CollisionInfo(colA->GetObject3d(), colA, inter));
				}
			}
		}
	}
}
