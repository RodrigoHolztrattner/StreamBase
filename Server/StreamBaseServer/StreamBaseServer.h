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
#include <array>

////////////////
// STRUCTURES //
////////////////

struct ConnectedClientBase
{
	HANDLE clientPipeHandle;
	std::string clientName; // Optional
	std::thread clientThread;
};

struct ConnectedClientSync : public ConnectedClientBase
{
};

struct ConnectedClientAsync : public ConnectedClientBase
{
	OVERLAPPED clientOverlappedHandle;
	bool isPendingConnection;
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

private:

	// This method will listen for incomming client connections asynchronously
	void AsyncListenAux();

	// This method will initialize the data necessary for each client that connects synchronously to us, also
	// this method will block until new connection requests arrive
	void SyncListenAux();

	// The handle client method, this will run on a separated thread for the given client
	void HandleClient(ConnectedClientBase* _clientInfo);

private:

	// I usually would use a custom vector to increase performance without reallocations everytime
	std::vector<ConnectedClientSync*> m_SyncConnectedClients;
	std::array<ConnectedClientAsync, MAXIMUM_SYNC_CLIENTS> m_AsyncConnectedClients;

	// All the asynchronous wait event handles
	std::array<HANDLE, MAXIMUM_SYNC_CLIENTS> m_AsyncEventHandles;

	// The thread that will listen for incomming client connections
	std::thread m_ClientConnectionListenerThread;
};