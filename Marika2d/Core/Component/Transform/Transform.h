#pragma once

#include "Core/Component/Component.h"
#include "Core/Math/Math.h"

namespace Mrk
{
    using Vector3 = glm::vec3;
    using Vector4 = glm::vec4;
    using Quaternion = glm::quat;
    using Matrix4 = glm::mat4;

    class Transform : public Component
    {
        MRK_COMPONENT(Transform) MRK_COMPONENT_UNREMOVABLE
    public:
        Transform();

        void TranslateLocal(const Vector3& offset);
        void TranslateWorld(const Vector3& offset);
        void RotateLocal(float angle, const Vector3& axis);
        void RotateWorld(float angle, const Vector3& axis);

        const Vector3& GetLocalPosition();
        Vector3 GetWorldPosition();
        void SetLocalPosition(const Vector3& newVal);
        void SetWorldPosition(const Vector3& newVal);

        const Vector3& GetLocalScale();
        Vector3 GetWorldScale();
        void SetLocalScale(const Vector3& newVal);
        void SetWorldScale(const Vector3& newVal);

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

        // 更新父物体缓存（在父子关系变化时调用）
        void UpdateParentCache();

    private:
        void SetLocalDirty();
        void SetWorldDirty();
        void UpdateLocalMatrix();
        void UpdateWorldMatrix();
        void UpdateDirections(); // 更新方向向量

        Matrix4 localMatrix;
        Matrix4 worldMatrix;
        Quaternion localRotation;
        Vector3 localPosition;
        Vector3 localScale;

        bool isLocalDirty = true;
        bool isWorldDirty = true;
        bool isDirectionsDirty = true;

        // 缓存方向向量
        Vector3 localUp, localLeft, localForward;
        Vector3 worldUp, worldLeft, worldForward;

        // 缓存父物体的 Transform
        std::shared_ptr<Transform> parent; // 已确保不会发生循环引用
    };
}