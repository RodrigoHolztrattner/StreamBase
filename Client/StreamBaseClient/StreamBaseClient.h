#pragma once

//////////////
// INCLUDES //
//////////////

#include "..\..\Common\StreamBaseCommonHeader.h"
#include "..\..\Common\StreamBaseCommonObject.h"
#include "..\..\Common\nlohmann\json.hpp"

// I don't like to include the windows header in a .h because this could generate a lot of conflicts
// but I think for the current purpose this is ok
#include <Windows.h>

#include <string>
#include <thread>
#include <vector>

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
	bool ConnectToServer(LPCWSTR _pipeName);

//////////////////////
public: // MESSAGES //
//////////////////////

	// Send a string to the server
	void SendServerString(std::string _string);

	// Send a number to the server
	void SendServerNumber(long long _number);

	// Create object on server
	void CreateObjectOnServer();

	// Return a json with a list of objects on the server
	nlohmann::json GetServerObjectList();

	// Retrieve an object from the server for the given index
	StreamBaseCommonObject GetObjectFromServerAtIndex(int _index);

	// This method will send the current object to be update on the server side
	void UpdateCurrentObjectOnServer(StreamBaseCommonObject _object);

///////////////////////
private: // INTERNAL //
///////////////////////

	// Send a message to the server
	void SendServerMessage(int _messageType, unsigned char* _messageData, unsigned int _dataSize);

	// Read a message from the server
	std::vector<unsigned char> GetServerMessage();

///////////////////////////////
private: // MEMBER VARIABLES //
///////////////////////////////

	// The server pipe handle
	HANDLE m_ServerPipeHandle;
};