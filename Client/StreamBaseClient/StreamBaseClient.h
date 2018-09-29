#pragma once

//////////////
// INCLUDES //
//////////////

#include "..\..\Common\StreamBaseCommonHeader.h"

// I don't like to include the windows header in a .h because this could generate a lot of conflicts
// but I think for the current purpose this is ok
#include <Windows.h>

#include <string>
#include <thread>

////////////////
// STRUCTURES //
////////////////

///////////
// CLASS //
///////////
class StreamBaseClient
{
public:

	// Constructor
	StreamBaseClient();

	// Connect to server
	bool ConnectToServer();

	// Send a string to the server
	void SendServerString(std::string _string);

	// Send a number to the server
	void SendServerNumber(long long _number);

private:

	// Send a message to the server
	void SendServerMessage(int _messageType, unsigned char* _messageData, unsigned int _dataSize);

private:

	// The server pipe handle
	HANDLE m_ServerPipeHandle;
};