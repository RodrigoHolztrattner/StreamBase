#include "StreamBaseCommonObject.h"

StreamBaseCommonObject::StreamBaseCommonObject(std::string _name)
{
	objectName = _name;
	objectHash = rand();
}

void to_json(nlohmann::json& j, const StreamBaseCommonObject& p)
{
	j = nlohmann::json{ { "name", p.objectName },{ "hash", p.objectHash }};
}

void from_json(const nlohmann::json& j, StreamBaseCommonObject& p)
{
	p.objectName = j.at("name").get<std::string>();
	p.objectHash = j.at("hash").get<int>();
}