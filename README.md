# Token-ring-communication-simulation
GVSU CIS-452 Program 2

The purpose of this programming assignment is to reinforce your understanding of the mechanisms used in process management system calls (creation, execution, suspension and termination), and in the use of the fundamental OS-level InterProcess Communication API (pipes, signals).

Your assignment is to write a program that uses multiple processes executing in parallel to simulate the operation of the token-ring protocol.  The processes should be organized in a ring configuration, using pipes for communication and coordination.  Signals may be used (depending on your design) to coordinate the behavior of processes in the ring. 

__Use Case__

Solution demonstration:: 
* your program should spawn processes to initialize a virtual local network of a user-defined size.  It should use pipes to establish the ring communication structure. 
* the original parent process (P1) should accept a message from the user (i.e. a string), and a destination process  (e.g. P3).  Note: you may use the process's virtual ID or the real process PID to identify and specify a destination.
* P1 should construct an Information token with the required fields filled in, pass it on to P2, and so on...
* the token should circulate around to the destination process, which copies the message and resets the Header field to 0
* when the token gets back to the sender, P1 should remove the message from the Information field
* you only need to transmit one message, but the empty token should continue to circulate around the ring
* when the user hits ^C, the original process (P1) should use signals to gracefully shutdown the simulation

At each step along the way, your program should output diagnostic information to communicate what is happening in the simulation (you might want to put small "sleeps" in between receiving/sending to enable a user to watch what is going on).  The idea here is to help a user understand the operation of the token-ring protocol by watching your simulation execute.
