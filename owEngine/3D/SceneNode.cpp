#include "stdafx.h"

// General
#include "SceneNode.h"

/*SceneNode::SceneNode(std::weak_ptr<SceneNode> _parent)
	: m_Name("SceneNode")
{
	SetParent(_parent);
}*/

SceneNode::SceneNode(cmat4 localTransform)
	: m_LocalTransform(localTransform)
	, m_Name("SceneNode")
	, m_Type(SN_TYPE_NONE)
	, m_IsRotateQuat(false)
	, m_Scale(1.0f, 1.0f, 1.0f)
	, m_IsLocalDirty(false)
	, m_IsWorldDirty(false)
{
	m_InverseLocalTransform = glm::inverse(m_LocalTransform);
}

SceneNode::~SceneNode()
{
	m_Children.clear();
}

cstring SceneNode::GetName() const
{
	return m_Name;
}

void SceneNode::SetName(cstring name)
{
	m_Name = name;
}

// Translate
void SceneNode::SetTranslate(cvec3 _translate) 
{ 
	m_Translate = _translate;
	m_IsLocalDirty = true;
	m_IsWorldDirty = true;
}
cvec3 SceneNode::GetTranslation() const 
{ 
	return m_Translate; 
}

// Rotate
void SceneNode::SetRotation(cvec3 _rotate) 
{ 
	m_Rotate = _rotate;
	m_IsLocalDirty = true;
	m_IsWorldDirty = true;
}
cvec3 SceneNode::GetRotation() const 
{ 
	return m_Rotate; 
}

// Rotate Quaternion
void SceneNode::SetRotationQuaternion(cquat _rotate) 
{ 
	m_RotateQuat = _rotate; 
	m_IsRotateQuat = true;
	m_IsLocalDirty = true;
	m_IsWorldDirty = true;
}
cquat SceneNode::GetRotationQuaternion() const 
{ 
	return m_RotateQuat;
}

// Scale
void SceneNode::SetScale(cvec3 _scale) 
{ 
	m_Scale = _scale;
	m_IsLocalDirty = true;
	m_IsWorldDirty = true;
}
cvec3 SceneNode::GetScale() const 
{ 
	return m_Scale; 
}

// Bounds
void SceneNode::SetBounds(BoundingBox _bbox) 
{ 
	m_Bounds = _bbox;
}
cbbox SceneNode::GetBounds() const 
{ 
	return m_Bounds; 
}

bool SceneNode::IsDirty() const
{
	return m_IsLocalDirty || m_IsWorldDirty;
}

// Local transform

mat4 SceneNode::GetLocalTransform()
{
	UpdateLocalTransform();
	return m_LocalTransform;
}

mat4 SceneNode::GetInverseLocalTransform()
{
	UpdateLocalTransform();
	return m_InverseLocalTransform;
}

void SceneNode::SetLocalTransform(cmat4 localTransform)
{
	m_LocalTransform = localTransform;
	m_InverseLocalTransform = glm::inverse(localTransform);
	m_IsLocalDirty = true;
	m_IsWorldDirty = true;
}

// World transform

mat4 SceneNode::GetWorldTransfom() const
{
	assert1(!m_IsWorldDirty);
	return m_WorldTransform;
}

mat4 SceneNode::GetInverseWorldTransform() const
{
	assert1(!m_IsWorldDirty);
	return m_InverseWorldTransform;
}

void SceneNode::SetWorldTransform(cmat4 worldTransform)
{
	mat4 inverseParentTransform = glm::inverse(GetParentWorldTransform());
	SetLocalTransform(inverseParentTransform * worldTransform);
}


mat4 SceneNode::GetParentWorldTransform() const
{
	mat4 parentTransform(1.0f);
	if (std::shared_ptr<SceneNode> parent = m_pParentNode.lock())
	{
		parentTransform = parent->GetWorldTransfom();
	}

	return parentTransform;
}

//

void SceneNode::UpdateLocalTransform(bool _forced)
{
	if (m_IsLocalDirty || _forced)
	{
		m_LocalTransform = mat4();

		m_LocalTransform = glm::translate(m_LocalTransform, m_Translate);
		if (m_IsRotateQuat)
		{
			m_LocalTransform *= glm::toMat4(m_RotateQuat);
		}
		else
		{
			m_LocalTransform = glm::rotate(m_LocalTransform, m_Rotate.x, vec3(1, 0, 0));
			m_LocalTransform = glm::rotate(m_LocalTransform, m_Rotate.y, vec3(0, 1, 0));
			m_LocalTransform = glm::rotate(m_LocalTransform, m_Rotate.z, vec3(0, 0, 1));
		}
		m_LocalTransform = glm::scale(m_LocalTransform, m_Scale);
		m_InverseLocalTransform = glm::inverse(m_LocalTransform);
		m_IsLocalDirty = false;
		m_IsWorldDirty = true;
	}
}

void SceneNode::UpdateWorldTransform(bool _forced)
{
	if (m_IsWorldDirty || _forced)
	{
		assert1(! m_IsLocalDirty);
		m_WorldTransform = GetParentWorldTransform() * m_LocalTransform;
		m_InverseWorldTransform = glm::inverse(m_WorldTransform);
		m_IsWorldDirty = false;
	}
}

void SceneNode::SetLocalUnderty()
{
	m_IsLocalDirty = false;
}

void SceneNode::AddChild(std::shared_ptr<SceneNode> childNode)
{
	if (childNode)
	{
		NodeList::iterator iter = std::find(m_Children.begin(), m_Children.end(), childNode);
		if (iter == m_Children.end())
		{
			childNode->m_pParentNode = shared_from_this();
			childNode->m_IsWorldDirty = true;
			childNode->UpdateWorldTransform();

			m_Children.push_back(childNode);
			if (! childNode->GetName().empty())
				m_ChildrenByName.insert(NodeNameMap::value_type(childNode->GetName(), childNode));
		}
	}
}

void SceneNode::RemoveChild(std::shared_ptr<SceneNode> childNode)
{
	if (childNode)
	{
		NodeList::iterator iter = std::find(m_Children.begin(), m_Children.end(), childNode);
		if (iter != m_Children.end())
		{
			childNode->SetParent(std::weak_ptr<SceneNode>());
			childNode->UpdateWorldTransform();

			m_Children.erase(iter);
			NodeNameMap::iterator iter2 = m_ChildrenByName.find(childNode->GetName());
			if (iter2 != m_ChildrenByName.end())
				m_ChildrenByName.erase(iter2);
		}
		else
		{
			// Maybe this node appears lower in the hierarchy...
			for (auto child : m_Children)
			{
				child->RemoveChild(childNode);
			}
		}
	}
}

void SceneNode::SetParent(std::weak_ptr<SceneNode> parentNode)
{
	// Remove from current parent
	std::shared_ptr<SceneNode> currentParent = m_pParentNode.lock();
	if (currentParent != nullptr)
	{
		currentParent->RemoveChild(shared_from_this());
		m_pParentNode.reset();
	}

	// Add to new parent
	if (std::shared_ptr<SceneNode> newParent = parentNode.lock())
		newParent->AddChild(shared_from_this());
}

void SceneNode::AddMesh(std::shared_ptr<IMesh> mesh)
{
	assert(mesh);
	MeshList::iterator iter = std::find(m_Meshes.begin(), m_Meshes.end(), mesh);
	if (iter == m_Meshes.end())
		m_Meshes.push_back(mesh);
}

void SceneNode::RemoveMesh(std::shared_ptr<IMesh> mesh)
{
	assert(mesh);
	MeshList::iterator iter = std::find(m_Meshes.begin(), m_Meshes.end(), mesh);
	if (iter != m_Meshes.end())
		m_Meshes.erase(iter);
}

void SceneNode::UpdateCamera(const Camera* camera)
{
	// Do nothing...
}

bool SceneNode::Accept(IVisitor& visitor)
{
	bool visitResult = visitor.Visit(*this);
	if (!visitResult)
		return false;

	// Visit meshes.
	for (auto mesh : m_Meshes)
	{
		mesh->Accept(visitor);
	}

	// Now visit children
	for (auto child : m_Children)
	{
		child->Accept(visitor);
	}

	return visitResult;
}

void SceneNode::OnUpdate(UpdateEventArgs & e)
{
}

bool SceneNode::checkFrustum(const Camera* _camera) const
{
	assert1(_camera != nullptr);
	return !_camera->GetFrustum().cullBox(GetBounds());
}

bool SceneNode::checkDistance2D(cvec3 _camPos, float _distance) const
{
	// Check distance to camera
	float distToCamera2D = glm::length(Fix_X0Z(_camPos) - Fix_X0Z(GetBounds().getCenter())) - GetBounds().getRadius();
	return distToCamera2D < _distance;
}

bool SceneNode::checkDistance(cvec3 _camPos, float _distance) const
{
	// Check distance to camera
	float distToCamera = glm::length(_camPos - GetBounds().getCenter()) - GetBounds().getRadius();
	return distToCamera < _distance;
}