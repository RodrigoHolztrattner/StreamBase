# StreamBase

1 - The client should be able to connect to the server through a NamedPipe

Ok, not a big deal.

2 - The client should be able to make both sync and async calls to the server

Ok, maybe threads + different sending options.

3 - The client should be able to send trivial data (strings, numbers) to the server

Number 4 will also perform data exchange, so I think I could consider this done on number 4.

4 - The client should be able to create objects on the server (based on req-7 below), retrieve them, their attributes and call methods on them

JSON + custom class?

5 - The server should be able to receive both sync/async connection requests from clients

Threads.

6 - The server should be able to store data provided by the client via NamedPipe in a reasonable data structure

JSON again?

7 - The server should be able to register a custom class (w/ related functions, attributes) which can be used by the client (see req-4)

Use an interface maybe?

8 - The server should be able to store the custom objects created by the client for the custom class created in req-7

In disk I imagine.

TODO:

- Migrate the code to CMAKE
- Use different projects for the client and server