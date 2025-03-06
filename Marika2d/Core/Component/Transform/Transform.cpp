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

// ��Ǳ��ر任Ϊ��
void Mrk::Transform::SetLocalDirty()
{
    isLocalDirty = true;
    isDirectionsDirty = true;
    SetWorldDirty();
}

// �������任Ϊ�࣬���ݹ鴫����������
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

// ���±��ؾ���
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

// �����������
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

// ���·�������
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

// ƽ�ƣ����ؿռ䣩
void Mrk::Transform::TranslateLocal(const Mrk::Vector3& offset)
{
    localPosition += localRotation * offset;
    SetLocalDirty();
}

// ƽ�ƣ�����ռ䣩
void Mrk::Transform::TranslateWorld(const Mrk::Vector3& offset)
{
    localPosition += offset;
    SetLocalDirty();
}

// ��ת�����ؿռ䣩
void Mrk::Transform::RotateLocal(float angle, const Mrk::Vector3& axis)
{
    localRotation = localRotation * glm::angleAxis(glm::radians(angle), axis);
    localRotation = glm::normalize(localRotation);
    SetLocalDirty();
}

// ��ת������ռ䣩
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

// ��ȡ����λ��
const Mrk::Vector3& Mrk::Transform::GetLocalPosition()
{
    return localPosition;
}

// ��ȡ����λ��
Mrk::Vector3 Mrk::Transform::GetWorldPosition()
{
    if (isLocalDirty) UpdateLocalMatrix();
    if (isWorldDirty) UpdateWorldMatrix();
    return worldMatrix[3].xyz();
}

// ���ñ���λ��
void Mrk::Transform::SetLocalPosition(const Mrk::Vector3& newVal)
{
    localPosition = newVal;
    SetLocalDirty();
}

// ��������λ��
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

// ��ȡ��������
const Mrk::Vector3& Mrk::Transform::GetLocalScale()
{
    return localScale;
}

// ��ȡ��������
Mrk::Vector3 Mrk::Transform::GetWorldScale()
{
    if (isLocalDirty) UpdateLocalMatrix();
    if (isWorldDirty) UpdateWorldMatrix();
    return { glm::length(worldMatrix[0].xyz()),
             glm::length(worldMatrix[1].xyz()),
             glm::length(worldMatrix[2].xyz()) };
}

// ���ñ�������
void Mrk::Transform::SetLocalScale(const Vector3& newVal)
{
    localScale = newVal;
    SetLocalDirty();
}

// ������������
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

// ��ȡ������ת
const Mrk::Quaternion& Mrk::Transform::GetLocalRotation()
{
    return localRotation;
}

// ��ȡ������ת
Mrk::Quaternion Mrk::Transform::GetWorldRotation()
{
    if (isLocalDirty) UpdateLocalMatrix();
    if (isWorldDirty) UpdateWorldMatrix();
    return glm::normalize(glm::toQuat(worldMatrix));
}

// ���ñ�����ת
void Mrk::Transform::SetLocalRotation(const Mrk::Quaternion& newVal)
{
    localRotation = glm::normalize(newVal);
    SetLocalDirty();
}

// ����������ת
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

// ��ȡ���ؾ���
Mrk::Matrix4 Mrk::Transform::GetLocalMatrix()
{
    if (isLocalDirty) UpdateLocalMatrix();
    return localMatrix;
}

// ��ȡ�������
Mrk::Matrix4 Mrk::Transform::GetWorldMatrix()
{
    if (isLocalDirty) UpdateLocalMatrix();
    if (isWorldDirty) UpdateWorldMatrix();
    return worldMatrix;
}

// ����ֽ�
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

// ���ñ��ؾ���
void Mrk::Transform::SetLocalMatrix(const Mrk::Matrix4& newVal)
{
    localMatrix = newVal;
    MatrixDecompose(localMatrix, localPosition, localRotation, localScale);
    SetLocalDirty();
}

// �����������
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

// ��ȡ���ط�������
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

// ��ȡ��ͼ����
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
