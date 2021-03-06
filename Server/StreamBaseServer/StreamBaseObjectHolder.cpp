//////////////////////////////////
// CLASS StreamBaseObjectHolder //
//////////////////////////////////
#include "StreamBaseObjectHolder.h"

StreamBaseObjectHolder::StreamBaseObjectHolder()
{
	// Try to load all objects from a file
	LoadAll();
}

void StreamBaseObjectHolder::CreateNewObject()
{
	StreamBaseCommonObject newObject = {std::string("Object " + std::to_string(m_InternalObjects.size()))};
	m_InternalObjects.push_back(newObject);

	// Save all files
	SaveAll();
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

			// Save all files
			SaveAll();

			return true;
		}
	}

	return false;
}

#include <iostream>
#include <fstream>

void StreamBaseObjectHolder::SaveAll()
{
	// Transform our internal object vector into a json
	nlohmann::json j(m_InternalObjects);

	std::ofstream file;
	file.open(SERVER_OBJECT_FILENAME);
	file << j;
	file.close();
}

void StreamBaseObjectHolder::LoadAll()
{
	nlohmann::json j;

	std::ifstream file;
	file.open(SERVER_OBJECT_FILENAME);

	// Check if the file is open
	if (file.is_open())
	{
		file >> j;
		file.close();

		m_InternalObjects = j.get< std::vector<StreamBaseCommonObject>>();
	}
}