#include <Pxf/Game/GameObject.h>
#include <stdio.h>
#include <Pxf/Util/String.h>

using namespace Pxf;
using namespace Game;

unsigned GameObject::m_ObjectCounter = 0;

GameObject::GameObject()
{

}

GameObject::GameObject(Pxf::Math::Vec3f _Position, const char* _ID)
	: m_ID(0)
{
	if(!_ID)
		_GenerateNewName();
	else
		m_ID = _ID;
}

GameObject::GameObject(float _X, float _Y, float _Z, const char* _ID)
{

}

void GameObject::_GenerateNewName()
{
	char _NewName[32];
	sprintf(_NewName,"GameObject%i",m_ObjectCounter);

	std::string * _StrName = new std::string(_NewName);
	m_ID = _StrName->c_str();
}

