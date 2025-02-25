#include "Transform.h"

#include "Core/GameObject/GameObject.h"

Mrk::Transform::Transform() :
	localMatrix(1.0f),
	worldMatrix(1.0f),
	localPosition(0.0f),
	localScale(1.0f),
	localRotation(1, 0, 0, 0)
{

}

void Mrk::Transform::SetLocalDirty()
{
	isLocalDirty = true;
	SetWorldDirty();
}

void Mrk::Transform::SetWorldDirty()
{
	for (auto& child : holder.lock()->GetChildren())
	{
		child->GetComponent<Transform>()->SetWorldDirty();
	}
}

void Mrk::Transform::UpdateLocalMatrix()
{
	if (isLocalDirty)
	{
		localMatrix = 1.0f;
		glm::translate(localMatrix, localPosition);
		localMatrix *= glm::toMat4(localRotation);
		glm::scale(localMatrix, localScale);

		isLocalDirty = false;
	}
}

void Mrk::Transform::UpdateWorldMatrix()
{
	if (isWorldDirty)
	{
		if (auto parent = this->holder.lock()->GetParent())
		{
			auto parentTrans = parent->GetComponent<Transform>();
			parentTrans->UpdateLocalMatrix();
			parentTrans->UpdateWorldMatrix();
			worldMatrix = parentTrans->worldMatrix * localMatrix;
		}
		else
		{
			worldMatrix = localMatrix;
		}
		isWorldDirty = false;
	}
}

void Mrk::Transform::TranslateLocal(const Mrk::Vector3& offset)
{
	localPosition += localRotation * offset;
	SetLocalDirty();
}

void Mrk::Transform::TranslateWorld(const Mrk::Vector3& offset)
{
	localPosition += offset;
	SetLocalDirty();
}

void Mrk::Transform::RotateLocal(float angle, const Mrk::Vector3& axis)
{
	localRotation = localRotation * glm::angleAxis(glm::radians(angle), axis);
	localRotation = glm::normalize(localRotation);
	SetLocalDirty();
}

void Mrk::Transform::RotateWorld(float angle, const Mrk::Vector3& axis)
{
	//TODO : !!!!!!!! 矩阵归一化 看之前的代码

	UpdateLocalMatrix();
	UpdateWorldMatrix();
	auto newWorldRotation = glm::angleAxis(glm::radians(angle), axis) * glm::toQuat(worldMatrix); //TODO : glm::toQuat(glm::normalize(worldMatrix));
	if (auto parent = holder.lock()->GetParent())
	{
		//TODO:考虑缓存parentTransform
		localRotation = glm::inverse(glm::toQuat(parent->GetComponent<Transform>()->worldMatrix)) * newWorldRotation;
	}
	else
	{
		localRotation = newWorldRotation;
	}

	SetLocalDirty();
}

const Mrk::Vector3& Mrk::Transform::GetLocalPosition()
{
	return localPosition;
}

Mrk::Vector3 Mrk::Transform::GetWorldPosition()
{
	UpdateLocalMatrix();
	UpdateWorldMatrix();
	return worldMatrix[3];
}

void Mrk::Transform::SetLocalPosition(const Mrk::Vector3& newVal)
{
	localPosition = newVal;
	SetLocalDirty();
}

void Mrk::Transform::SetWorldPosition(const Mrk::Vector3& newVal)
{
	UpdateLocalMatrix();
	UpdateWorldMatrix();
	if (auto parent = holder.lock()->GetParent())
	{
		localPosition = glm::inverse(parent->GetComponent<Transform>()->worldMatrix) * Vector4(newVal, 1.0f);
	}
	else
	{
		localPosition = newVal;
	}
	SetLocalDirty();
}

const Mrk::Quaternion& Mrk::Transform::GetLocalRotation()
{
	return localRotation;
}

Mrk::Quaternion Mrk::Transform::GetWorldRotation()
{
	//TODO : !!!!!!!! 矩阵归一化 看之前的代码

	UpdateLocalMatrix();
	UpdateWorldMatrix();
	return Mrk::Quaternion(glm::toQuat(worldMatrix));
}

void Mrk::Transform::SetLocalRotation(const Mrk::Quaternion& newVal)
{
	localRotation = newVal;
	localRotation = glm::normalize(localRotation);
	SetLocalDirty();
}

void Mrk::Transform::SetWorldRotation(const Mrk::Quaternion& newVal)
{
	//TODO : !!!!!!!! 矩阵归一化 看之前的代码

	UpdateLocalMatrix();
	UpdateWorldMatrix();
	if (auto parent = holder.lock()->GetParent())
	{
		localRotation = glm::inverse(glm::toQuat(parent->GetComponent<Transform>()->worldMatrix)) * newVal;
	}
	else
	{
		localRotation = newVal;
	}
	localRotation = glm::normalize(localRotation);
	SetLocalDirty();
}

Mrk::Matrix4 Mrk::Transform::GetLocalMatrix()
{
	UpdateLocalMatrix();
	return localMatrix;
}

Mrk::Matrix4 Mrk::Transform::GetWorldMatrix()
{
	UpdateLocalMatrix();
	UpdateWorldMatrix();
	return worldMatrix;
}

void MatrixDecompose(const Mrk::Matrix4& mat, Mrk::Vector3& outPos, Mrk::Quaternion& outRot, Mrk::Vector3& outScl)
{
	//TODO : !!!!!!!! 矩阵归一化 看之前的代码

	outPos = mat[3];
	outRot = glm::toQuat(mat);
	outScl = { mat[0].length(), mat[1].length(), mat[2].length() };
}

void Mrk::Transform::SetLocalMatrix(const Mrk::Matrix4& newVal)
{
	localMatrix = newVal;
	MatrixDecompose(localMatrix, localPosition, localRotation, localScale);
	SetWorldDirty();
}

void Mrk::Transform::SetWorldMatrix(const Mrk::Matrix4& newVal)
{
	UpdateLocalMatrix();
	UpdateWorldMatrix();
	worldMatrix = newVal;
	if (auto parent = holder.lock()->GetParent())
	{
		localMatrix = glm::inverse(parent->GetComponent<Transform>()->worldMatrix) * newVal;
	}
	else
	{
		localMatrix = newVal;
	}
	MatrixDecompose(localMatrix, localPosition, localRotation, localScale);
	SetWorldDirty();
}

Mrk::Vector3 Mrk::Transform::GetLocalUp()
{
	UpdateLocalMatrix();
	return glm::normalize(localMatrix[1].xyz());
}

Mrk::Vector3 Mrk::Transform::GetWorldUp()
{
	UpdateLocalMatrix();
	UpdateWorldMatrix();
	return glm::normalize(worldMatrix[1].xyz());
}

Mrk::Vector3 Mrk::Transform::GetLocalLeft()
{
	UpdateLocalMatrix();
	return glm::normalize(localMatrix[0].xyz());
}

Mrk::Vector3 Mrk::Transform::GetWorldLeft()
{
	UpdateLocalMatrix();
	UpdateWorldMatrix();
	return glm::normalize(worldMatrix[0].xyz());
}

Mrk::Vector3 Mrk::Transform::GetLocalForward()
{
	UpdateLocalMatrix();
	return glm::normalize(localMatrix[2].xyz());
}

Mrk::Vector3 Mrk::Transform::GetWorldForward()
{
	UpdateLocalMatrix();
	UpdateWorldMatrix();
	return glm::normalize(worldMatrix[2].xyz());
}

Mrk::Matrix4 Mrk::Transform::GetViewMatrix()
{
	UpdateLocalMatrix();
	UpdateWorldMatrix();

	auto eye = worldMatrix[3].xyz();
	auto lookDir = glm::normalize(worldMatrix[2].xyz());
	auto up = glm::normalize(worldMatrix[1].xyz());
	return glm::lookAtRH(eye, eye + lookDir, up);
}
