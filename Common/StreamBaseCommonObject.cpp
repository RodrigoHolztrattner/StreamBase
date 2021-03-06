#include "StreamBaseCommonObject.h"

StreamBaseCommonObject::StreamBaseCommonObject(std::string _name)
{
	objectName = _name;
	objectCounter = 0;
}

void StreamBaseCommonObject::IncrementCounter()
{
	objectCounter++;
}

void StreamBaseCommonObject::DecrementCounter()
{
	objectCounter--;
}

void StreamBaseCommonObject::ResetCounter()
{
	objectCounter = 0;
}

int StreamBaseCommonObject::GetCounter()
{
	return objectCounter;
}

std::string StreamBaseCommonObject::GetName()
{
	return objectName;
}

void to_json(nlohmann::json& j, const StreamBaseCommonObject& p)
{
	j = nlohmann::json{ { "name", p.objectName },{ "counter", p.objectCounter }};
}

void from_json(const nlohmann::json& j, StreamBaseCommonObject& p)
{
	p.objectName = j.at("name").get<std::string>();
	p.objectCounter = j.at("counter").get<int>();
}