//////////////////////////////////
// CLASS StreamBaseObjectHolder //
//////////////////////////////////
#include "StreamBaseObjectHolder.h"

void StreamBaseObjectHolder::CreateNewObject()
{
	StreamBaseCommonObject newObject = {std::string("Object " + std::to_string(m_InternalObjects.size()))};
	m_InternalObjects.push_back(newObject);
}

nlohmann::json StreamBaseObjectHolder::GetObjectNameList()
{
	std::vector<std::string> objectNames = {};

	// For each object
	for (auto& object : m_InternalObjects)
	{
		objectNames.push_back(object.objectName);
	}

	nlohmann::json j = objectNames;
	return j;
}

nlohmann::json StreamBaseObjectHolder::GetObjectOnIndex(int _index)
{
	nlohmann::json j = m_InternalObjects[_index];
	return j;
}

bool StreamBaseObjectHolder::UpdateInternalObject(nlohmann::json _objectJSON, StreamBaseCommonObject& _outObject)
{
	StreamBaseCommonObject receivedObject = _objectJSON.get<StreamBaseCommonObject>();

	// Find the object with the same name
	for (auto& object : m_InternalObjects)
	{
		// Compare the names
		if (object.objectName.compare(receivedObject.objectName) == 0)
		{
			// Found it, update
			object = receivedObject;
			_outObject = object;

			return true;
		}
	}

	return false;
}