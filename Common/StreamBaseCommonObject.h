#pragma once

//////////////
// INCLUDES //
//////////////
#include <string>
#include "nlohmann/json.hpp"

///////////
// CLASS //
///////////
class StreamBaseCommonObject
{
public:

	StreamBaseCommonObject() = default;
	StreamBaseCommonObject(std::string _name);

	// Increment the counter
	void IncrementCounter();

	// Decrement counter
	void DecrementCounter();

	// Reset the counter
	void ResetCounter();

	// Return the counter
	int GetCounter();

	// Return the name
	std::string GetName();

// Public just for the simplicity on using the nlohmann::json lib without a lot of protected method
public: 

	std::string objectName;
	int objectCounter;
};

void to_json(nlohmann::json& j, const StreamBaseCommonObject& p);
void from_json(const nlohmann::json& j, StreamBaseCommonObject& p);