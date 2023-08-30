#include "Collision.h"
#include <cmath>

bool Collision::CheckSphere2Plane(const Sphere& sphere, const Plane& plane, Vector3* inter)
{
    // ���W�n�̌��_���狅�̒��S���W�ւ̋���
    Vector3 V;
    float distV = V.dot(sphere.center, plane.normal);
    // ���ʂ̌��_���������Z���邱�ƂŁA���ʂƋ��̒��S�Ƃ̋������o��
    float dist = distV - plane.distance;
    // �����̐�Βl�����a���傫����Γ������Ă��Ȃ�
    if (fabsf(dist) > sphere.radius) return false;

    // �^����_���v�Z
    if (inter) {
        // ���ʏ�̍ŋߐړ_���A�^����_�Ƃ���
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
			// A�̔��a��0�̎����W��B�̒��S  B�̔��a��0�̎����W��A�̒��S  �ƂȂ�悤�ɕ��
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
	// ���S�_�̋����̂Q�� <= ���a�̘a�̂Q��@�Ȃ����
	Vector3 tmp;
	tmp = sphereA.center - sphereB.center;
	float dist = tmp.dot(tmp, tmp);
	float radius2 = sphereA.radius + sphereB.radius;
	radius2 *= radius2;

	if (dist <= radius2)
	{
		if (inter)
		{
			// A�̔��a��0�̎����W��B�̒��S�@B�̔��a��0�̎����W��A�̒��S�@�ƂȂ�悤�⊮
			float t = sphereB.radius / (sphereA.radius + sphereB.radius);
			*inter = tmp.lerp(sphereA.center, sphereB.center, t);
		}
		// �����o���x�N�g�����v�Z
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
	// point��p0�̊O���̒��_�̈�̒��ɂ��邩�ǂ����`�F�b�N
	Vector3 p0_p1 = triangle.p1 - triangle.p0;
	Vector3 p0_p2 = triangle.p2 - triangle.p0;
	Vector3 p0_pt = point - triangle.p0;

	Vector3 VV;
	float d1 = VV.dot(p0_p1, p0_pt);
	float d2 = VV.dot(p0_p2, p0_pt);

	if (d1 <= 0.0f && d2 <= 0.0f)
	{
		// p0���ŋߖT
		*closest = triangle.p0;
		return;
	}

	// point��p1�̊O���̒��_�̈�̒��ɂ��邩�ǂ����`�F�b�N
	Vector3 p1_pt = point - triangle.p1;

	float d3 = VV.dot(p0_p1, p1_pt);
	float d4 = VV.dot(p0_p2, p1_pt);

	if (d3 >= 0.0f && d4 <= d3)
	{
		// p1���ŋߖT
		*closest = triangle.p1;
		return;
	}

	// point��p0_p1�̕ӗ̈�̒��ɂ��邩�ǂ����`�F�b�N���A�����point��p0_p1��ɑ΂���ˉe��Ԃ�
	float vc = d1 * d4 - d3 * d2;
	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
	{
		float v = d1 / (d1 - d3);
		*closest = triangle.p0 + v * p0_p1;
		return;
	}

	// point��p2�̊O���̒��_�̈�̒��ɂ��邩�ǂ����`�F�b�N
	Vector3 p2_pt = point - triangle.p2;

	float d5 = VV.dot(p0_p1, p2_pt);
	float d6 = VV.dot(p0_p2, p2_pt);
	if (d6 >= 0.0f && d5 <= d6)
	{
		*closest = triangle.p2;
		return;
	}

	// point��p0_p2�̕ӗ̈�̒��ɂ��邩�ǂ����`�F�b�N���A�����point��p0_p2��ɑ΂���ˉe��Ԃ�
	float vb = d5 * d2 - d1 * d6;
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
	{
		float w = d2 / (d2 - d6);
		*closest = triangle.p0 + w * p0_p2;
		return;
	}

	// point��p1_p2�̕ӗ̈�̒��ɂ��邩�ǂ����`�F�b�N���A�����point��p1_p2��ɑ΂���ˉe��Ԃ�
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
	// ���̒��S�ɑ΂���ŋߐړ_�ł���O�p�`��ɂ���_p��������
	ClosestPtPoint2Triangle(sphere.center, triangle, &p);

	// �_p�Ƌ��̒��S�̍����x�N�g��
	Vector3 v = p - sphere.center;
	Vector3 VV;
	// �����̓������߂�
	// �i�����x�N�g�����m�̓��ς͎O�����̒藝�̃��[�g�����̎��ƈ�v����j
	float vvv = VV.dot(v, v);

	// ���ƎO�p�`�̋��������a�ȉ��Ȃ瓖�����Ă��Ȃ�
	if (vvv > sphere.radius * sphere.radius)return false;

	// �^����_���v�Z
	if (inter) {
		// �O�p�`��̍ŋߐړ_p���^����_�Ƃ���
		*inter = p;
	}
	// �����o���x�N�g�����v�Z
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
	const float epsilon = 1.0e-5f; // �덷�z���p�̔����Ȓl
	// �ʖ@���g���C�̕����x�N�g���̓���
	Vector3 V;
	float d1 = V.dot(plane.normal, ray.dir);
	// ���ʂɂ͓�����Ȃ�
	if (d1 > -epsilon) { return false; }
	// �n�_�ƌ��_�̋����i���ʂ̖@�������j
	// �ʖ@���g���C�̎��_���W�i�ʒu�x�N�g���j�̓���
	float d2 = V.dot(plane.normal, ray.start);
	// �n�_�ƕ��ʂ̋����i���ʂ̖@�������j
	float dist = d2 - plane.distance;
	// �n�_�ƕ��ʂ̋����i���C�����j
	float t = dist / -d1;
	// ��_���n�_�����ɂ���̂ŁA������Ȃ�
	if (t < 0)return false;
	// ��������������
	if (distance) { *distance = t; }

	// ��_���v�Z
	if (inter) { *inter = ray.start + t * ray.dir; }

	return true;
}

bool Collision::CheckRay2Triangle(const Ray& ray, const Triangle& triangle, float* distance, Vector3* inter)
{
	// �O�p�`������Ă��镽�ʂ��Z�o
	Plane plane;
	Vector3 interPlane;
	Vector3 V;
	plane.normal = triangle.normal;
	plane.distance = V.dot(triangle.normal, triangle.p0);

	// ���C�ƕ��ʂ��������Ă��Ȃ���΁A�������Ă��Ȃ�
	if (!CheckRay2Plane(ray, plane, distance, &interPlane)) { return false; }

	// ���C�ƕ��ʂ��������Ă����̂ŁA�����ƌ�_���������܂ꂽ
	// ���C�ƕ��ʂ̌�_���O�p�`�̓����ɂ��邩����
	const float epsilon = 1.0e-5f;// �덷�z���p�̔����Ȓl
	Vector3 m;

	// ��p0_p1�ɂ���
	Vector3 pt_p0 = triangle.p0 - interPlane;
	Vector3 p0_p1 = triangle.p1 - triangle.p0;
	m = pt_p0.cross(p0_p1);

	// �ӂ̊O���ł���Γ������Ă��Ȃ��̂Ŕ����ł��؂�
	if (V.dot(m, triangle.normal) < -epsilon) { return false; }

	// ��p1_p2�ɂ���
	Vector3 pt_p1 = triangle.p1 - interPlane;
	Vector3 p1_p2 = triangle.p2 - triangle.p1;
	m = pt_p1.cross(p1_p2);

	// �ӂ̊O���ł���Γ������Ă��Ȃ��̂Ŕ����ł��؂�
	if (V.dot(m, triangle.normal) < -epsilon) { return false; }

	// ��p2_p0�ɂ���
	Vector3 pt_p2 = triangle.p2 - interPlane;
	Vector3 p2_p0 = triangle.p0 - triangle.p2;
	m = pt_p2.cross(p2_p0);

	// �ӂ̊O���ł���Γ������Ă��Ȃ��̂Ŕ����ł��؂�
	if (V.dot(m, triangle.normal) < -epsilon) { return false; }

	// �����Ȃ̂ŁA�������Ă���
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

	// ray�̎n�_��sphere�̊O���ɂ���(c>0)�Aray��sphere���痣��Ă��������������Ă���ꍇ(b>0)�A������Ȃ�
	if (c > 0.0f && b > 0.0f) { return false; }

	float discr = b * b - c;

	// ���̔��ʎ��͗Ⴊ�����O��Ă��邱�ƂɈ�v
	if (discr < 0.0f) { return false; }

	// ���C�͋��ƌ������Ă���
	// ��������ŏ��̒lt���v�Z
	float t = -b - sqrtf(discr);

	// �������ł���ꍇ�A���C�͋��̓�������J�n���Ă���̂ł����[���ɃN�����v
	if (t < 0)t = 0.0f;
	if (distance) { *distance = t; }

	if (inter) { *inter = ray.start + t * ray.dir; }

	return true;
}

bool Collision::CheckSphere2AABB(const Sphere& sphere, const WorldTransform& transA, Vector3* inter, Vector3* reject)
{
	Box box;
	// �{�b�N�X�\���̂ɏ���o�^
	box.center = transA.translation;
	box.maxRadius = (transA.maxVertex_ * transA.scale);
	box.maxRadius.x = abs(box.maxRadius.x);
	box.maxRadius.y = abs(box.maxRadius.y);
	box.maxRadius.z = abs(box.maxRadius.z);

	// ���S�_�̋����̂Q�� <= ���a�̘a�̂Q��@�Ȃ����
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

		//if (inter) {
		//	// A�̔��a��0�̎����W��B�̒��S  B�̔��a��0�̎����W��A�̒��S  �ƂȂ�悤�ɕ��
		//	float t = sphereB.radius / (sphereA.radius + sphereB.radius);
		//	*inter = Vector3::lerp(sphereA.center, sphereB.center, t);
		//}

		// �����o���̃x�N�g���v�Z
		if (reject) {
			// ���̒��S���甼�a�����Z�������̃|�W�V����
			Vector3 sphereColP;
			sphereColP.x = sphere.center.x;
			sphereColP.y = sphere.center.y;
			sphereColP.z = sphere.center.z;
			Vector3 sphereColM;
			sphereColM.x = sphere.center.x;
			sphereColM.y = sphere.center.y;
			sphereColM.z = sphere.center.z;


			// �L���[�u�̒��S���甼�a�����Z�⌸�Z�����|�W�V����
			Vector3 cubeColP;
			cubeColP.x = box.center.x + box.maxRadius.x;
			cubeColP.y = box.center.y + box.maxRadius.y;
			cubeColP.z = box.center.z + box.maxRadius.z;
			Vector3 cubeColM;
			cubeColM.x = box.center.x - box.maxRadius.x;
			cubeColM.y = box.center.y - box.maxRadius.y;
			cubeColM.z = box.center.z - box.maxRadius.z;

			Vector3 rejectLen;

			// �O���ɉ����o��
			if (sphereColP.z <= cubeColM.z &&
			   (sphereColM.x <= cubeColP.x && sphereColP.x >= cubeColM.x)) {

				rejectLen.z = sphere.radius + box.maxRadius.z - sqrtf(distz);
			}
			// ����ɉ����o��
			if (sphereColM.z >= cubeColP.z &&
			   (sphereColM.x <= cubeColP.x && sphereColP.x >= cubeColM.x)) {

				rejectLen.z = sphere.radius + box.maxRadius.z - sqrtf(distz);
			}
			// �E���ɉ����o��
			if (sphereColM.x >= cubeColP.x &&
			   (sphereColM.z <= cubeColP.z && sphereColP.z >= cubeColM.z)) {

				rejectLen.x = sphere.radius + box.maxRadius.x - sqrtf(distx);
			}
			// �����ɉ����o��
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
	// �{�b�N�X�\���̂ɏ���o�^
	a.center = transA.translation;
	a.minRadius = transA.minVertex_ * transA.scale;
	a.maxRadius = transA.maxVertex_ * transA.scale;

	b.center = transB.translation;
	b.minRadius = transB.minVertex_ * transB.scale;
	b.maxRadius = transB.maxVertex_ * transB.scale;

	// �{�b�N�X�\���̂̒��S�_����̃v���X�ƃ}�C�i�X�̂��ꂼ�ꔼ�a���Z�o
	a.minCenterRadius = a.center + a.minRadius;
	a.maxCenterRadius = a.center + a.maxRadius;

	b.minCenterRadius = b.center + b.minRadius;
	b.maxCenterRadius = b.center + b.maxRadius;

	// �����̏������������Ȃ瓖�����Ă��Ȃ�
	if (a.minCenterRadius.x > b.maxCenterRadius.x ||
		a.maxCenterRadius.x < b.minCenterRadius.x ||
		a.minCenterRadius.y > b.maxCenterRadius.y ||
		a.maxCenterRadius.y < b.minCenterRadius.y ||
		a.minCenterRadius.z > b.maxCenterRadius.z ||
		a.maxCenterRadius.z < b.minCenterRadius.z) {
		return false;
	}

	// ��̏������ʂ��Ă��ȂȂ��Ȃ瓖�����Ă���
	return true;
}

bool Collision::CheckOBB(const WorldTransform& obbA, const WorldTransform& obbB)
{
	OBB a, b;
	// �|�W�V�����̃Z�b�g
	a.centerPos = obbA.translation;
	// �e���̕����x�N�g�� x,y,z
	a.directionVector[0] = { obbA.matWorld_.m[0][0],obbA.matWorld_.m[0][1],obbA.matWorld_.m[0][2] };
	a.directionVector[1] = { obbA.matWorld_.m[1][0],obbA.matWorld_.m[1][1],obbA.matWorld_.m[1][2] };
	a.directionVector[2] = { obbA.matWorld_.m[2][0],obbA.matWorld_.m[2][1],obbA.matWorld_.m[2][2] };

	// �|�W�V�����̃Z�b�g
	b.centerPos = obbB.translation;
	// �e���̕����x�N�g�� x,y,z
	b.directionVector[0] = { obbB.matWorld_.m[0][0],obbB.matWorld_.m[0][1],obbB.matWorld_.m[0][2] };
	b.directionVector[1] = { obbB.matWorld_.m[1][0],obbB.matWorld_.m[1][1],obbB.matWorld_.m[1][2] };
	b.directionVector[2] = { obbB.matWorld_.m[2][0],obbB.matWorld_.m[2][1],obbB.matWorld_.m[2][2] };

	// �{�b�N�X�\���̂ɏ���o�^
	a.minRadius = obbA.minVertex_ * obbA.scale;
	a.maxRadius = obbA.maxVertex_ * obbA.scale;

	b.minRadius = obbB.minVertex_ * obbB.scale;
	b.maxRadius = obbB.maxVertex_ * obbB.scale;

	// �e���̒���
	a.shaftLength[0] = (abs(a.maxRadius.x) + abs(a.minRadius.x)) * 0.5f;
	a.shaftLength[1] = (abs(a.maxRadius.y) + abs(a.minRadius.y)) * 0.5f;
	a.shaftLength[2] = (abs(a.maxRadius.z) + abs(a.minRadius.z)) * 0.5f;

	b.shaftLength[0] = (abs(b.maxRadius.x) + abs(b.minRadius.x)) * 0.5f;
	b.shaftLength[1] = (abs(b.maxRadius.y) + abs(b.minRadius.y)) * 0.5f;
	b.shaftLength[2] = (abs(b.maxRadius.z) + abs(b.minRadius.z)) * 0.5f;

	// �e�����x�N�g���̊m��
	// �iN***:�W���������x�N�g���j
	Vector3 NAe1 = a.directionVector[0], Ae1 = NAe1 * a.shaftLength[0];
	Vector3 NAe2 = a.directionVector[1], Ae2 = NAe2 * a.shaftLength[1];
	Vector3 NAe3 = a.directionVector[2], Ae3 = NAe3 * a.shaftLength[2];
	Vector3 NBe1 = b.directionVector[0], Be1 = NBe1 * b.shaftLength[0];
	Vector3 NBe2 = b.directionVector[1], Be2 = NBe2 * b.shaftLength[1];
	Vector3 NBe3 = b.directionVector[2], Be3 = NBe3 * b.shaftLength[2];
	Vector3 Interval = a.centerPos - b.centerPos;

	// ������ : Ae1
	Vector3 mathVec3;
	FLOAT rA = mathVec3.length(Ae1);
	FLOAT rB = LenSegOnSeparateAxis(&NAe1, &Be1, &Be2, &Be3);
	FLOAT L = fabs(mathVec3.dot(Interval, NAe1));
	if (L > rA + rB)
		return false; // �Փ˂��Ă��Ȃ�

	 // ������ : Ae2
	rA = mathVec3.length(Ae2);
	rB = LenSegOnSeparateAxis(&NAe2, &Be1, &Be2, &Be3);
	L = fabs(mathVec3.dot(Interval, NAe2));
	if (L > rA + rB)
		return false;

	// ������ : Ae3
	rA = mathVec3.length(Ae3);
	rB = LenSegOnSeparateAxis(&NAe3, &Be1, &Be2, &Be3);
	L = fabs(mathVec3.dot(Interval, NAe3));
	if (L > rA + rB)
		return false;

	// ������ : Be1
	rA = LenSegOnSeparateAxis(&NBe1, &Ae1, &Ae2, &Ae3);
	rB = mathVec3.length(Be1);
	L = fabs(mathVec3.dot(Interval, NBe1));
	if (L > rA + rB)
		return false;

	// ������ : Be2
	rA = LenSegOnSeparateAxis(&NBe2, &Ae1, &Ae2, &Ae3);
	rB = mathVec3.length(Be2);
	L = fabs(mathVec3.dot(Interval, NBe2));
	if (L > rA + rB)
		return false;

	// ������ : Be3
	rA = LenSegOnSeparateAxis(&NBe3, &Ae1, &Ae2, &Ae3);
	rB = mathVec3.length(Be3);
	L = fabs(mathVec3.dot(Interval, NBe3));
	if (L > rA + rB)
		return false;

	// ������ : C11
	Vector3 Cross;
	Cross = mathVec3.cross(NAe1, NBe1);
	rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3);
	L = fabs(mathVec3.dot(Interval, Cross));
	if (L > rA + rB)
		return false;

	// ������ : C12
	Cross = mathVec3.cross(NAe1, NBe2);
	rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3);
	L = fabs(mathVec3.dot(Interval, Cross));
	if (L > rA + rB)
		return false;

	// ������ : C13
	Cross = mathVec3.cross(NAe1, NBe3);
	rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2);
	L = fabs(mathVec3.dot(Interval, Cross));
	if (L > rA + rB)
		return false;

	// ������ : C21
	Cross = mathVec3.cross(NAe2, NBe1);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3);
	L = fabs(mathVec3.dot(Interval, Cross));
	if (L > rA + rB)
		return false;

	// ������ : C22
	Cross = mathVec3.cross(NAe2, NBe2);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3);
	L = fabs(mathVec3.dot(Interval, Cross));
	if (L > rA + rB)
		return false;

	// ������ : C23
	Cross = mathVec3.cross(NAe2, NBe3);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2);
	L = fabs(mathVec3.dot(Interval, Cross));
	if (L > rA + rB)
		return false;

	// ������ : C31
	Cross = mathVec3.cross(NAe3, NBe1);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2);
	rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3);
	L = fabs(mathVec3.dot(Interval, Cross));
	if (L > rA + rB)
		return false;

	// ������ : C32
	Cross = mathVec3.cross(NAe3, NBe2);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3);
	L = fabs(mathVec3.dot(Interval, Cross));
	if (L > rA + rB)
		return false;

	// ������ : C33
	Cross = mathVec3.cross(NAe3, NBe3);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2);
	L = fabs(mathVec3.dot(Interval, Cross));
	if (L > rA + rB)
		return false;

	// �������ʂ����݂��Ȃ��̂Łu�Փ˂��Ă���v
	return true;
}

float Collision::LenSegOnSeparateAxis(Vector3* Sep, Vector3* e1, Vector3* e2, Vector3* e3)
{
	Vector3 mathVec3;
	// 3�̓��ς̐�Βl�̘a�œ��e���������v�Z
   // ������Sep�͕W��������Ă��邱��
	FLOAT r1 = fabs(mathVec3.dot(*Sep, *e1));
	FLOAT r2 = fabs(mathVec3.dot(*Sep, *e2));
	FLOAT r3 = e3 ? (fabs(mathVec3.dot(*Sep, *e3))) : 0;

	return r1 + r2 + r3;
}
