#include "StreamBaseObjectHolder.h"
#include "StreamBaseServer.h"


int main()
{
	StreamBaseObjectHolder objectHolder;
	StreamBaseServer server(objectHolder);

	server.BeginListen();


	Sleep(10000000);
}