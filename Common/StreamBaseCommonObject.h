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

	std::string objectName;
	int objectHash;
};

void to_json(nlohmann::json& j, const StreamBaseCommonObject& p);
void from_json(const nlohmann::json& j, StreamBaseCommonObject& p);