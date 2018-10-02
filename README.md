# StreamBase

# Made by Rodrigo Holztrattner Reis

----

Hello, this is my attempt to implement the StreamBase client/server application, following the given requirements.
I would like to say that I never used the named pipe API but I found it really similar to network messaging (UDP/TCP), so 
this helped a lot.

I'm used to write a comment as a placeholder instead the real code first, this helps to 
structure the project since I can suppose that in the future I will implement the behaviour listed on the comment, the 
downside of this is that some lines contains a comment that is almost exactly as the code right down it.

I will point what I understanded by your requirements and further below explain what I've implemented: 

1 - The client should be able to connect to the server through a NamedPipe

Ok, not a big deal, just create a NamedPipe on the server and make the client get a handle to it.
One point here is the support for multiple clients, so each client will have its own unique handle.

2 - The client should be able to make both sync and async calls to the server

Same as networking, client can send/write to the server and if it should be done synchronously the operation must block
until it finish, in case of an asynchronous operation I need to register a callback and the operation itself will not block.

3 - The client should be able to send trivial data (strings, numbers) to the server

Receive input from the user and send to the server, simple.

4 - The client should be able to create objects on the server (based on req-7 below), retrieve them, their attributes and call methods on them

JSON or something similar for sure. I will create a common class between the client and the server with support for serialization
and pass the object serialized between the pipe.

5 - The server should be able to receive both sync/async connection requests from clients

I think it's impossible to have the same named pipe listen for both synchronous and asynchronous requests so I will use 2
named pipes and ask the client what it wants to connect to.
On the server side I probably will have one separated thread that will listen to synchronous connection requests (this one 
will block until receive a request) and another that will wait for an event for a given asynchronous request.

6 - The server should be able to store data provided by the client via NamedPipe in a reasonable data structure

Save the strings/numbers using a pair with the client ID + string/number. I don't know if these data should be stored on
disk, probably not.

7 - The server should be able to register a custom class (w/ related functions, attributes) which can be used by the client (see req-4)

Register like creating an interface with virtual methods or just use a common class? I think the common class is more 
simple then using the interfaced ideia for this test so I will keep it for simplicity.

8 - The server should be able to store the custom objects created by the client for the custom class created in req-7

Store the JSON into a file and read from it whenever the server starts.

----

* How to build:

Both the client and the server folders have a "CMakeLists.txt" file, just use cmake on each of them.

* Common between server and client:

All the messages have a header (defined on the **StreamBaseCommonHeader.h** file) + the data, the header contains the 
message ID and the data size and the data is operation-dependent.

There are some defines of the common header, here you can change the maximum number of asynchronous connections (MAXIMUM_SYNC_CLIENTS), 
the maximum pipe buffer size (PIPE_BUFFER_SIZE), the server object filename (SERVER_OBJECT_FILENAME) and the pipe names for both
synchronous and asynchronous connections (SYNCPIPENAME and ASYNCPIPENAME). Also here you can find the message IDs.

The StreamBaseCommonObject is the class that I'm using to implement the *req-4 and req-7*, it's serializable and has
both a name and a counter.

I'm using a JSON library (with a MIT license) that I love, it allows me to serialize and create json much more easily with
c++, the library github can be found here: https://github.com/nlohmann/json

* How the server works:

The server will begin listening for both synchronous and asynchronous connection requests, it will create a separated thread
for incomming synchronous requests and wait for them unsing a blocking call to **ConnectNamedPipe** without an OVERLAPPED 
argument. The asynchronous is done on the main thread, it will create n slots for asynchronous clients (defined on the 
common header) and create one event to each of them, it will wait for one of these events signal and proceed with the connection.

The connection for both cases (async and sync) are handled by the same method **HandleClient**, each client will have its
own thread that will run that method. The method will wait for incomming messages and proceed with the correct operation 
for each of them, this wait is done using a blocking call to **ReadFile** (as no asynchronous read was requested and 
is much simple to do that way).

There is a **StreamBaseObjectHolder** class that will hold all objects that were created and updated by clients, also this
class will attempt to save those objects on disk and load them on start, do the data here is persistent.

* How the client works:

First the user will be prompted to choose between connection to the server using the synchronous pipe or the assynchronous 
pipe.

Then the user will be asked what operation he wants, each of those operations is self explanatory so the user just 
need to keep selecting the desired choice until he decides to leave.

Every operation can be done using a synchronous send request or an asynchronous one, the user can toggle the current mode
(synch/asynch) choosing the option 7 from the console.

For each operation the client will collect the necessary data and send it to the server, if the synchronous mode is active
the **WriteFile** operation will block until complete, in case of an asynchronous operation an OVERLAPPED structure will be
passed, creating a new event that is handled by a separated thread. This thread will check for incomming events and will
call a callback, if necessary, to complete the main operation.

* Other aspects

I used something around 7 hours to implement the code and perform researches.

Both projects were made using the Visual Studio Cummunity 2017 initially.

SourceTree + Github was used from the beginning, I'm very familiar at using github (you can see at my github page).

No security checks were made, like checking if the input number is a number and not a string, also there is no client 
disconnection checks, they could be easly added but that of course would take a time that would possible increase the 
current one much more then what is expected to be.


Thank you for the opportunity! Hope to hear from you soon!
Rodrigo Holztrattner Reis


Also I think that is important to add that the client needs to register 2 callback methods, one to receive the object name
list from the server and another to update the current selected object when retrieving it by index.
 