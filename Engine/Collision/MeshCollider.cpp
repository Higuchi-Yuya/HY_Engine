#include "MeshCollider.h"
#include "Collision.h"

void MeshCollider::ConstructTriangles(Model* model)
{
	// �O�p�`���X�g���N���A
	triangles.clear();

	// ���f���̎����b�V�����X�g���擾
	const std::vector<Mesh*>& meshes = model->GetMeshes();
	// ���݂̃��b�V���̊J�n�O�p�`�ԍ������Ă����ϐ��i�O�ŏ������j
	int start = 0;
	// �S���b�V���ɂ��ď��ɏ�������
	std::vector<Mesh*>::const_iterator it = meshes.cbegin();
	for (; it != meshes.cend(); ++it) {
		Mesh* mesh = *it;
		const std::vector<Mesh::VertexPosNormalUv>& vertices = mesh->GetVertices();
		const std::vector<unsigned short>& indices = mesh->GetIndices();

		// �C���f�b�N�X�́A�O�p�`�̐����R����̂ŁA
		// �������烁�b�V�����̎O�p�`�̐����t�Z����
		size_t triangleNum = indices.size() / 3;

		// ���݂̃��b�V���̎O�p�`�̐������A�O�p�`���X�g�ɃX�y�[�X��ǉ�����
		triangles.resize(triangles.size() + triangleNum);

		// �S�O�p�`�ɂ��ď��ɏ�������
		for (int i = 0; i < triangleNum; i++) {
			// ������v�Z����O�p�`�̎Q��
			Triangle& tri = triangles[start + i];
			int idx0 = indices[i * 3 + 0];
			int idx1 = indices[i * 3 + 1];
			int idx2 = indices[i * 3 + 2];
			// �O�p�`��3���_�̍��W����
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
			// 3���_����@�����v�Z
			tri.ComputeNormal();
		}
		// ���̃��b�V���́A���܂ł̎O�p�`�ԍ��̎�����g���B
		start += (int)triangleNum;
	}

}

void MeshCollider::Update()
{
	// ���[���h�s��̋t�s����v�Z
	invMatWorld.MakeInverse();
}

bool MeshCollider::CheckCollisionSphere(const Sphere& sphere, Vector3* inter)
{
	// �I�u�W�F�N�g�̃��[�J�����W�n�ł̋��𓾂�i���a��X�X�P�[�����Q��)
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
	// �I�u�W�F�N�g�̃��[�J�����W�n�ł̃��C�𓾂�
	Ray localRay;
	Matrix4 keisanM4;
	Vector3 keisanV3;
	localRay.start = keisanM4.transform(ray.start, invMatWorld);
	localRay.dir = keisanM4.transformNotW(ray.dir, invMatWorld);

	// ���[�J�����W�n�Ō������`�F�b�N
	std::vector<Triangle>::const_iterator it = triangles.cbegin();

	for (; it != triangles.cend(); ++it) {
		const Triangle& triangle = *it;

		Vector3 tempInter;
		// ���C�ƎO�p�`�̓����蔻��
		if (Collision::CheckRay2Triangle(localRay, triangle, nullptr, &tempInter)) {

			const Matrix4& matWorld = GetObject3d()->GetMatWorld();

			// ���[���h���W�n�ł̌�_�𓾂�
			tempInter = keisanM4.transform(tempInter, matWorld);

			if (distance) {
				// ��_�ƃ��C�n�_�̋������v�Z
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
