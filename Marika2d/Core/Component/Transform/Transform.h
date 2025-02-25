#pragma once

#include "Core/Component/Component.h"
#include "Core/Math/Math.h"

namespace Mrk
{
	class Transform : public Component
	{
		MRK_COMPONENT(Transform)
	public:
		Transform();

		void TranslateLocal(const Vector3& offset);
		void TranslateWorld(const Vector3& offset);
		void RotateLocal(float angle, const Vector3& axis);
		void RotateWorld(float angle, const Vector3& axis);

		const Mrk::Vector3& GetLocalPosition();
		Vector3 GetWorldPosition();
		void SetLocalPosition(const Vector3& newVal);
		void SetWorldPosition(const Vector3& newVal);

		const Quaternion& GetLocalRotation();
		Quaternion GetWorldRotation();
		void SetLocalRotation(const Quaternion& newVal);
		void SetWorldRotation(const Quaternion& newVal);

		Matrix4 GetLocalMatrix();
		Matrix4 GetWorldMatrix();
		void SetLocalMatrix(const Matrix4& newVal);
		void SetWorldMatrix(const Matrix4& newVal);

		Vector3 GetLocalUp();
		Vector3 GetWorldUp();
		Vector3 GetLocalLeft();
		Vector3 GetWorldLeft();
		Vector3 GetLocalForward();
		Vector3 GetWorldForward();

		Matrix4 GetViewMatrix();

	private:
		void SetLocalDirty();
		void SetWorldDirty();
		void UpdateLocalMatrix();
		void UpdateWorldMatrix();

		Matrix4 localMatrix;
		Matrix4 worldMatrix;
		Quaternion localRotation;
		Vector3 localPosition;
		bool isLocalDirty = true;
		Vector3 localScale;
		bool isWorldDirty = true;
	};
}