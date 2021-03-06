---------------------------------
RAPTOR - A fun webserver
=================================
_________________________________

Random Shit to Remember
=======================
* Use 'fork', not 'select' and look up: accept, bind,
listen, connect, socket, shutdown
* Research 'echo server in C'
* Beej's guide to network programming
* Look up 'nc'
* Make sure the fork is working 
* Same for the child processes
* Fix the validator
* Think about a properties file


Outline of how to make a server:

* Create a socket. This is a file that that acts as an endpoint for connecting an IP on a certain port.
* You're going to need to bind this socket to a port. This is how you communicate with the machine.
* To do this, you need a struct_info of some variety to provide the details of how your socket is going to operate.
* the getaddrinfo method is a way of getting this struct, you give it the IP, port numbers, etc.
* Once you have a socket, you'll need to run a loop that listens for attemps to connect.

Some notes from Zed's video:
===========================
* create a sigaction that hangles SIGCHILD
* listen on the port of the given host
* Accept a client connection, then fork
* if zero, then child
* else server
* do a handler, so that when children die you wait on them

* Look into killing signal process children (waitpid, etc.)

First Steps For Statistics Server
================================
1. Create a data structure for holding information from each of the commands -> Gonna need a hashamp for each one,
and a BSTree may be useful for certain numeric samples.
2. Write a protocol parse to handle it and fill in the data ->
This will more or less be a series of commands that are then linked to the 
specific stats actions. I may implement the URL structure right away.
3. Pass the data to a function that knows how to do the command -> 
This will basically be our stats engine.

Core Components
===============
- The HashMap and (maybe) BSTree
- The Stats Engine
- The URL router

Control Flow
============
1. There is an input command, the name of the stat, and a corresponding number (Check to make sure that's the case!)
2. This will then hit a parser that takes the command and data, and shoots it to the correct function
   * 2.b: This parser might take the shape of a URL router
   * 2.c: It will have to decide which CRUD command is being asked for
   * 2.d: This will necessitate functions that Insert, Retrieve, Modify, and Delete from the map.
        - The above functions will be seperate from the Stats engine
        - Although Stats has a "Create" that will probably call our Create
3. This Data will then be saved into (probably) a hashmap for quick reads.
4. A BSTree could be useful for certain calls that take place across samples


- Each input has:
    * URL (command)
    * Name (for sample)
    * Data (numbers) 

Sketch For Statistics Protocol
==============================
* Basic Required Commands:
    1. Create - add a new statistic, with a name now (may need to change the stat struct)
    2. Mean - get the mean of a statistic
    3. UpSample - what we're doing here is updating, adding a new sample
    4. Dump - this a Read of a particular Static
        - List: dump every statistic in memory

* The first three are going to need two paramters: Name of statistic, and it's values
* Let's just parse as plaintext first and then implement the URL routes (although that is better)


TODAY
=====
* The URL's are successfully routing to the right functions
* This is not utilizing the TSTree, because that turn out to be as
hard as I initially though. All the same, it's wired up, once I figure that out
* The next step is going to be to throw in the stats functions, which will be easy
since they are already there
* The next tricky part that I foresee is simple going to be saving shit to the map
* Also, it will have to pull from the map and send that back. If data isn't being pulled,
there should still be return messages though.


Working Hashmap Set and Retrieval
=================================
bstring node = Hashmap_get(map, blist->entry[1]);
printf("This one is a longshot: %s\n", bdata(node));

Stats *node = Hashmap_get(map, blist->entry[1]);
printf("This one is a longshot: %lf\n", node->max);

int data = atoi(bdata(blist->entry[2]));
printf("This is our data: %d\n", data); // such wow

