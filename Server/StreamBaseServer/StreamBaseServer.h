#pragma once

//////////////
// INCLUDES //
//////////////

#include "..\..\Common\StreamBaseCommonHeader.h"

// I don't like to include the windows header in a .h because this could generate a lot of conflicts
// but I think for the current purpose this is ok
#include <Windows.h>

#include <string>
#include <vector>
#include <thread>

////////////////
// STRUCTURES //
////////////////

struct ConnectedClient
{
	HANDLE clientPipeHandle;
	std::string clientName; // Optional

	std::thread clientThread;
};

///////////
// CLASS //
///////////
class StreamBaseServer
{
public:

	// Constructor
	StreamBaseServer();

	// Start listening for incomming client connections
	void BeginListen();

	// Shutdown all operations
	// TODO:

	// Set if this server should accept aync client connections
	void ServerShouldAcceptAsyncClientConnections(bool _status);

private:

	// Threaded listening method
	void ListenAux();

	// Threaded handle client method
	void HandleClient(ConnectedClient* _clientInfo);

private:

	// I usually would use a custom vector to increase performance without reallocations everytime
	std::vector<ConnectedClient*> m_ConnectedClients;

	// The thread that will listen for incomming client connections
	std::thread m_ClientConnectionListenerThread;

	// If we are accepting async connections from clients this will be true
	bool m_AcceptingAsyncClientConnections;
};