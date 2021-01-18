# LinearActuatorControl
Arduino C++ code to control a linear actuator via an L298N motor controller.

This is a very simple sketch for an Arduino Pro-Micro to control a linear
actuator (LA) and open a commercial door via the Makerspace RFID reader. 
It does a few things I would normally never do but I want the code to be
easily maintainable by folks that don't know the Arduino that well.  I
use delay extensivly in this sketch as the LA is controlled by time. 
Delay should normally never be used in an Arduino sketch as it locks up
the primary Arduino loop, until delay is done which is bad practice.

I don't have any positioning sensors, so I have to rely on time, the LA
travels at 15mm a second which is just a little over 1/2 of an inch.  I
need to pull in exactly 1/2 inch so I use a time of 950 miliseconds to
extend the LA enough to take the strain off the steel cable that pulls
the latch open.  We may be able to take up a little strain on the cable
and make the door open even faster, really we just need enough slack for
the door to lock when closed and open rapidly and we can test for that
distance.

If we add a microswitch or two we could have exact placement of the LA
but until we add them this will have to do.

The LA had end stops at both ends and will not over heat when driven to
its stops, which is quite handy, if you read the code you will notice I
drive the LA into it's stops every time I open the door, this is
intentional to make sure the lock opens every time it should and I am
starting from a known position to release the tension.

I like to use defines to declare values that use within the program 
so it's easier to understand what is going on in the code.

This was written for TheLab.ms makerspace (I am a member) to control
access to the space, but anyone is welcome to use the code under the
GPLv2 license.
