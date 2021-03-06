#pragma once

//////////////
// INCLUDES //
//////////////

/////////////
// DEFINES //
/////////////

#define SYNCPIPENAME				"\\\\.\\pipe\\SyncPipe"
#define ASYNCPIPENAME				"\\\\.\\pipe\\AsyncPipe"
#define SERVER_OBJECT_FILENAME		"custom_objects.dat"
#define PIPE_BUFFER_SIZE			1024
#define MAXIMUM_SYNC_CLIENTS		4

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