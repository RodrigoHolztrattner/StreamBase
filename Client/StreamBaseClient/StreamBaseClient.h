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
#include <functional>

////////////////
// STRUCTURES //
////////////////

///////////
// CLASS //
///////////
class StreamBaseClient
{
	// This is an event structure used to handle incomming asynchronous operations
	struct AsynchronousEvent
	{
		int expectedMessage;
		OVERLAPPED overlappedStructure;
	};

public:

	// Constructor
	StreamBaseClient();

	// Connect to server
	bool ConnectToServer(LPCWSTR _pipeName);

//////////////////////
public: // MESSAGES //
//////////////////////

	// Send a string to the server
	void SendServerString(std::string _string, bool _asynchronous);

	// Send a number to the server
	void SendServerNumber(long long _number, bool _asynchronous);

	// Create object on server
	void CreateObjectOnServer(bool _asynchronous);

	// Return a json with a list of objects on the server
	void GetServerObjectList(bool _asynchronous);

	// Retrieve an object from the server for the given index
	void GetObjectFromServerAtIndex(int _index, bool _asynchronous);

	// This method will send the current object to be update on the server side
	void UpdateCurrentObjectOnServer(StreamBaseCommonObject _object, bool _asynchronous);

///////////////////////
public: // CALLBACKS //
///////////////////////

	// Register the callbacks
	void RegisterObjectListCallback(std::function<void(nlohmann::json)> _callback);
	void RegisterObjectRetrievalCallback(std::function<void(StreamBaseCommonObject)> _callback);

///////////////////////
private: // INTERNAL //
///////////////////////

	// An auxiliar thread that will handle any asynchronous operations
	void AsynchronousAuxiliarMethod();

///////////////////////
private: // INTERNAL //
///////////////////////

	// Send a message to the server
	void SendServerMessage(int _messageType, unsigned char* _messageData, unsigned int _dataSize, bool _asynchronous);

	// Read a message from the server
	std::vector<unsigned char> GetServerMessage();

///////////////////////////////
private: // MEMBER VARIABLES //
///////////////////////////////

	// The server pipe handle
	HANDLE m_ServerPipeHandle;

	// Our registered callbacks
	std::function<void(nlohmann::json)> m_ReceivedObjectListCallback;
	std::function<void(StreamBaseCommonObject)> m_ReceivedObjectCallback;

	// All expected asynchronous events
	std::vector<AsynchronousEvent> m_AsynchronousEvents;
	std::vector<HANDLE> m_AsynchronousEventHandles;

	// The thread that will manage asynchronous completion operations
	std::thread m_AsynchronousManagerThread;
};