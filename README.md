Simple DNS Client
=================

Written for CS3600 by Adam Olgin and William Dyer

Make Commands
=============

make - compile the code
make test - compile the code and run the given test script
make clean - remove all executables
make utest - compile and run the unittests
make debug - run gdb on the unit test executable
make cleanall - remove all associated executables
make unittests - compile the unit tests

Correct Usage
=============

Once the code is compiled, you may run the following:

./3600dns [-ns|-ms] @<server:port> <name>

  port(optional): the UDP port number of the DNS server. Default value is 53
  -ns|-ms (optional): specify whether this is a name(2) or mail server query(15). Default value is an A record (1)
  server (required): The IP address of the DNS server, in a.b.c.d format
  name (required): the name to query for

It is recommended to use 129.10.112.152 as the server, seeing as it is a test server set up
specifically for this project. However, this code will also work for other servers.

A few things to note:
* Errors will be thrown if 2 or 3 arguments are not supplied with the executable
* If the server argument is not supplied with an '@', an error will be thrown for malformed input
* If no port is specified, then the port will be set 53
* If no query type is specified, then the query type will be set to 1 (A record)
* If 3 arguments are supplied with the executable, and a flag besides '-ns' or '-mx' is given, then an error
  will be thrown about malformed input

Our Approach
============

Our approach was to make the pieces that we could into structs, such as packet headers,
and a good deal of the fields for a question and response. The latter two have the fields of variable length left out
and just memcpy'd in elsewhere in the code.

Then, we formed a basic question, parsing the server and port, and sent the question. An error is thrown if sending goes badly.

Afterwards, we wait to receive a response and then handle that.

Testing
=======

We performed tests by placing print statements in the code, using hex dumps, running the test script,
and making using of tools such as Dig and Wireshark.

Difficulties
============

The main difficulty we faced was handling a response. It took a while for us to understand
how to read and parse the response that we got from our sent packets.

Initially, we did also run into some issues sending a packet, but that was due to an improper
parsing of the name and port.
