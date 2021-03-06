#pragma once

//////////////
// INCLUDES //
//////////////

#include "..\..\Common\StreamBaseCommonHeader.h"
#include "..\..\Common\StreamBaseCommonObject.h"
#include "..\..\Common\nlohmann\json.hpp"

#include <vector>

////////////////
// STRUCTURES //
////////////////

///////////
// CLASS //
///////////
class StreamBaseObjectHolder
{
public:

	// Constructor
	StreamBaseObjectHolder();

	// This method will create a new object (with dummy initial values)
	void CreateNewObject();

	// This method will return a json that contains the names of each internal object
	nlohmann::json GetObjectNameList();

	// This method will return a json that contains a entire object by index
	nlohmann::json GetObjectOnIndex(int _index);

	// This method will receive a json that contains a entire object and update the internal one using its name
	bool UpdateInternalObject(nlohmann::json _objectJSON, StreamBaseCommonObject& _outObject);

private:

	// Save all objects into a file
	void SaveAll();

	// Try to load all objects from a file
	void LoadAll();

private:

	// All internal objects
	std::vector<StreamBaseCommonObject> m_InternalObjects;
};