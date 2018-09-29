#pragma once

/////////////
// DEFINES //
/////////////

#define PIPENAME	"\\\\.\\pipe\\Pipe"

///////////
// ENUMS //
///////////

// All the message types
enum MessageType
{
	MessageTypeUnknow			= 0,
	MessageTypeString			= 1,
	MessageTypeNumber			= 2,
	MessageTypeCreateObject		= 3,
	MessageTypeListObjects		= 4,
	MessageTypeRetrieveObject	= 5,
	MessageTypeStoreObject		= 6
};

////////////////
// STRUCTURES //
////////////////

struct MainMessageHeader
{
	int messageType;
	int messageDataSize;
};