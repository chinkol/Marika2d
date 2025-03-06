#include "Transform.h"

#include "Core/GameObject/GameObject.h"

Mrk::Transform::Transform() :
    localMatrix(1.0f),
    worldMatrix(1.0f),
    localPosition(0.0f),
    localScale(1.0f),
    localRotation(1.0f, 0.0f, 0.0f, 0.0f),
    localUp(0.0f, 1.0f, 0.0f),
    localLeft(1.0f, 0.0f, 0.0f),
    localForward(0.0f, 0.0f, 1.0f),
    worldUp(0.0f, 1.0f, 0.0f),
    worldLeft(1.0f, 0.0f, 0.0f),
    worldForward(0.0f, 0.0f, 1.0f)
{
}

// 标记本地变换为脏
void Mrk::Transform::SetLocalDirty()
{
    isLocalDirty = true;
    isDirectionsDirty = true;
    SetWorldDirty();
}

// 标记世界变换为脏，并递归传播到子物体
void Mrk::Transform::SetWorldDirty()
{
    isWorldDirty = true;
    isDirectionsDirty = true;
    for (auto& child : holder.lock()->GetChildren())
    {
        if (auto childTrans = child->GetComponent<Transform>())
        {
            childTrans->SetWorldDirty();
        }
    }
}

// 更新本地矩阵
void Mrk::Transform::UpdateLocalMatrix()
{
    if (isLocalDirty)
    {
        localMatrix = glm::translate(glm::mat4(1.0f), localPosition) *
            glm::toMat4(localRotation) *
            glm::scale(glm::mat4(1.0f), localScale);
        isLocalDirty = false;
    }
}

// 更新世界矩阵
void Mrk::Transform::UpdateWorldMatrix()
{
    if (isWorldDirty)
    {
        UpdateLocalMatrix();
        if (parent)
        {
            parent->UpdateLocalMatrix();
            parent->UpdateWorldMatrix();
            worldMatrix = parent->worldMatrix * localMatrix;
        }
        else
        {
            worldMatrix = localMatrix;
        }
        isWorldDirty = false;
    }
}

// 更新方向向量
void Mrk::Transform::UpdateDirections()
{
    if (!isDirectionsDirty) return;
    UpdateLocalMatrix();
    UpdateWorldMatrix();
    localUp = glm::normalize(localMatrix[1].xyz());
    localLeft = glm::normalize(localMatrix[0].xyz());
    localForward = glm::normalize(localMatrix[2].xyz());
    worldUp = glm::normalize(worldMatrix[1].xyz());
    worldLeft = glm::normalize(worldMatrix[0].xyz());
    worldForward = glm::normalize(worldMatrix[2].xyz());
    isDirectionsDirty = false;
}

// 平移（本地空间）
void Mrk::Transform::TranslateLocal(const Mrk::Vector3& offset)
{
    localPosition += localRotation * offset;
    SetLocalDirty();
}

// 平移（世界空间）
void Mrk::Transform::TranslateWorld(const Mrk::Vector3& offset)
{
    localPosition += offset;
    SetLocalDirty();
}

// 旋转（本地空间）
void Mrk::Transform::RotateLocal(float angle, const Mrk::Vector3& axis)
{
    localRotation = localRotation * glm::angleAxis(glm::radians(angle), axis);
    localRotation = glm::normalize(localRotation);
    SetLocalDirty();
}

// 旋转（世界空间）
void Mrk::Transform::RotateWorld(float angle, const Mrk::Vector3& axis)
{
    UpdateLocalMatrix();
    UpdateWorldMatrix();
    auto newWorldRotation = glm::angleAxis(glm::radians(angle), axis) * glm::toQuat(worldMatrix);
    newWorldRotation = glm::normalize(newWorldRotation);
    if (parent)
    {
        localRotation = glm::inverse(glm::toQuat(parent->worldMatrix)) * newWorldRotation;
    }
    else
    {
        localRotation = newWorldRotation;
    }
    localRotation = glm::normalize(localRotation);
    SetLocalDirty();
}

// 获取本地位置
const Mrk::Vector3& Mrk::Transform::GetLocalPosition()
{
    return localPosition;
}

// 获取世界位置
Mrk::Vector3 Mrk::Transform::GetWorldPosition()
{
    if (isLocalDirty) UpdateLocalMatrix();
    if (isWorldDirty) UpdateWorldMatrix();
    return worldMatrix[3].xyz();
}

// 设置本地位置
void Mrk::Transform::SetLocalPosition(const Mrk::Vector3& newVal)
{
    localPosition = newVal;
    SetLocalDirty();
}

// 设置世界位置
void Mrk::Transform::SetWorldPosition(const Mrk::Vector3& newVal)
{
    if (isLocalDirty) UpdateLocalMatrix();
    if (isWorldDirty) UpdateWorldMatrix();
    if (parent)
    {
        glm::vec4 worldPos(newVal.x, newVal.y, newVal.z, 1.0f);
        localPosition = (glm::inverse(parent->worldMatrix) * worldPos).xyz();
    }
    else
    {
        localPosition = newVal;
    }
    SetLocalDirty();
}

// 获取本地缩放
const Mrk::Vector3& Mrk::Transform::GetLocalScale()
{
    return localScale;
}

// 获取世界缩放
Mrk::Vector3 Mrk::Transform::GetWorldScale()
{
    if (isLocalDirty) UpdateLocalMatrix();
    if (isWorldDirty) UpdateWorldMatrix();
    return { glm::length(worldMatrix[0].xyz()),
             glm::length(worldMatrix[1].xyz()),
             glm::length(worldMatrix[2].xyz()) };
}

// 设置本地缩放
void Mrk::Transform::SetLocalScale(const Vector3& newVal)
{
    localScale = newVal;
    SetLocalDirty();
}

// 设置世界缩放
void Mrk::Transform::SetWorldScale(const Vector3& newVal)
{
    if (isLocalDirty) UpdateLocalMatrix();
    if (isWorldDirty) UpdateWorldMatrix();
    if (parent)
    {
        Vector3 parentWorldScale = parent->GetWorldScale();
        localScale = newVal / parentWorldScale;
    }
    else
    {
        localScale = newVal;
    }
    SetLocalDirty();
}

// 获取本地旋转
const Mrk::Quaternion& Mrk::Transform::GetLocalRotation()
{
    return localRotation;
}

// 获取世界旋转
Mrk::Quaternion Mrk::Transform::GetWorldRotation()
{
    if (isLocalDirty) UpdateLocalMatrix();
    if (isWorldDirty) UpdateWorldMatrix();
    return glm::normalize(glm::toQuat(worldMatrix));
}

// 设置本地旋转
void Mrk::Transform::SetLocalRotation(const Mrk::Quaternion& newVal)
{
    localRotation = glm::normalize(newVal);
    SetLocalDirty();
}

// 设置世界旋转
void Mrk::Transform::SetWorldRotation(const Mrk::Quaternion& newVal)
{
    if (isLocalDirty) UpdateLocalMatrix();
    if (isWorldDirty) UpdateWorldMatrix();
    auto normalizedVal = glm::normalize(newVal);
    if (parent)
    {
        localRotation = glm::inverse(glm::toQuat(parent->worldMatrix)) * normalizedVal;
    }
    else
    {
        localRotation = normalizedVal;
    }
    localRotation = glm::normalize(localRotation);
    SetLocalDirty();
}

// 获取本地矩阵
Mrk::Matrix4 Mrk::Transform::GetLocalMatrix()
{
    if (isLocalDirty) UpdateLocalMatrix();
    return localMatrix;
}

// 获取世界矩阵
Mrk::Matrix4 Mrk::Transform::GetWorldMatrix()
{
    if (isLocalDirty) UpdateLocalMatrix();
    if (isWorldDirty) UpdateWorldMatrix();
    return worldMatrix;
}

// 矩阵分解
void MatrixDecompose(const Mrk::Matrix4& mat, Mrk::Vector3& outPos, Mrk::Quaternion& outRot, Mrk::Vector3& outScl)
{
    outPos = mat[3].xyz();
    outScl.x = glm::length(mat[0].xyz());
    outScl.y = glm::length(mat[1].xyz());
    outScl.z = glm::length(mat[2].xyz());

    if (outScl.x > 0.0f && outScl.y > 0.0f && outScl.z > 0.0f)
    {
        glm::mat4 rotationMatrix = mat;
        rotationMatrix[0] /= outScl.x;
        rotationMatrix[1] /= outScl.y;
        rotationMatrix[2] /= outScl.z;
        outRot = glm::normalize(glm::toQuat(rotationMatrix));
    }
    else
    {
        outRot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }
}

// 设置本地矩阵
void Mrk::Transform::SetLocalMatrix(const Mrk::Matrix4& newVal)
{
    localMatrix = newVal;
    MatrixDecompose(localMatrix, localPosition, localRotation, localScale);
    SetLocalDirty();
}

// 设置世界矩阵
void Mrk::Transform::SetWorldMatrix(const Mrk::Matrix4& newVal)
{
    if (isLocalDirty) UpdateLocalMatrix();
    if (isWorldDirty) UpdateWorldMatrix();
    worldMatrix = newVal;
    if (parent)
    {
        localMatrix = glm::inverse(parent->worldMatrix) * newVal;
    }
    else
    {
        localMatrix = newVal;
    }
    MatrixDecompose(localMatrix, localPosition, localRotation, localScale);
    SetLocalDirty();
}

// 获取本地方向向量
Mrk::Vector3 Mrk::Transform::GetLocalUp()
{
    UpdateDirections();
    return localUp;
}

Mrk::Vector3 Mrk::Transform::GetWorldUp()
{
    UpdateDirections();
    return worldUp;
}

Mrk::Vector3 Mrk::Transform::GetLocalLeft()
{
    UpdateDirections();
    return localLeft;
}

Mrk::Vector3 Mrk::Transform::GetWorldLeft()
{
    UpdateDirections();
    return worldLeft;
}

Mrk::Vector3 Mrk::Transform::GetLocalForward()
{
    UpdateDirections();
    return localForward;
}

Mrk::Vector3 Mrk::Transform::GetWorldForward()
{
    UpdateDirections();
    return worldForward;
}

// 获取视图矩阵
Mrk::Matrix4 Mrk::Transform::GetViewMatrix()
{
    if (isLocalDirty) UpdateLocalMatrix();
    if (isWorldDirty) UpdateWorldMatrix();

    auto eye = worldMatrix[3].xyz();
    auto lookDir = glm::normalize(worldMatrix[2].xyz());
    auto up = glm::normalize(worldMatrix[1].xyz());
    return glm::lookAtRH(eye, eye + lookDir, up);
}

void Mrk::Transform::UpdateParentCache()
{
    if (auto parentHolder = holder.lock()->GetParent())
    {
        parent = parentHolder->GetComponent<Transform>();
    }
    else
    {
        parent.reset();
    }
}
