/*
 * Copyright(c) David Mandala, Designed by THEM, All Rights Reserved 2021
 * 
 * All material is proprietary to David Mandala, Designed by THEM and may be
 * used only pursuant to license rights granted by him.  Other reproduction,
 * distribution, or use is prohibited, except as licensed below:
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 2 only.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*
 * This is a very simple sketch for an Arduino Pro-Micro to control a linear
 * actuator (LA) and open a commercial door via the Makerspace RFID reader. 
 * It does a few things I would normally never do but I want the code to be
 * easily maintainable by folks that don't know the Arduino that well.  I
 * use delay extensivly in this sketch as the LA is controlled by time. 
 * Delay should normally never be used in an Arduino sketch as it locks up
 * the primary Arduino loop, until delay is done which is bad practice.
 *
 * I don't have any positioning sensors, so I have to rely on time, the LA
 * travels at 15mm a second which is just a little over 1/2 of an inch.  I
 * need to pull in exactly 1/2 inch so I use a time of 950 miliseconds to
 * extend the LA enough to take the strain off the steel cable that pulls
 * the latch open.  We may be able to take up a little strain on the cable
 * and make the door open even faster, really we just need enough slack for
 * the door to lock when closed and open rapidly and we can test for that
 * distance.
 * 
 * If we add a microswitch or two we could have exact placement of the LA
 * but until we add them this will have to do.
 *
 * The LA had end stops at both ends and will not over heat when driven to
 * its stops, which is quite handy, if you read the code you will notice I
 * drive the LA into it's stops every time I open the door, this is
 * intentional to make sure the lock opens every time it should and I am
 * starting from a known position to release the tension.
 *
 * I like to use defines to declare values that use within the program 
 * so it's easier to understand what is going on in the code.
 * 
 * This was written for TheLab.ms makerspace (I am a member) to control
 * access to the space, but anyone is welcome to use the code under the
 * GPLv2 license.
 *
 */
 
#define CODE_VERSION "0.05"

/*
 * How the voltage divider is made:
 * It is made from 3 220k resistors, 2 are in parallel and one in series
 * with the parallel ones.  The parallel resistors are equal to a single
 * 110k resistor but I was out of them.  12V is connected to the single 220k
 * resistor, the tap point is between the 220k and the 110k resistor.  The
 * far end of the 110k resistor is connected to ground.  I prefer to use
 * fairly high values to keep current flow very low.
 * 
 * But with a high value voltage divider you also need a high value resistor 
 * to ground so the SIGNAL_IN pin below uses a 220k resistor to ground so 
 * that the 12V signal can pull up the voltage to be seen as a HIGH.  If you
 * use a 10K resistor, with the 220k voltage divider the signal will never be
 * seen as HIGH, so you have to match the values.
 * 
 */

/*
 * Wiring hookups and some defines for them
 */
// Ground (color), to Ground on MotorController board, and black from 12v to 5v voltage divider
// VCC, (red) to 5V from MotorController board

// Next 2 pins are to drive the MotorController board
// Pin 10, (white) wire to IN2 on MotorController board
#define LA_CONTROL_1 10

// Pin 16, (red) wire to IN1 on MotorController board
#define LA_CONTROL_2 16

// The next pin takes input from card reader system, 12V High when door to be open
// Pin 14 (yellow) wire from 12v to 5v voltage divider with a 220k pull down resistor to ground.
#define SIGNAL_IN 14

// Match the terminology to what we want to do to the door mech
// Request to unlock the door
#define UNLOCK HIGH

// Request to lock the door
#define LOCK LOW

// Timing used to control the location of the LA
//#define TIME_TO_OPEN 1100
#define TIME_TO_CLOSE 950
#define TIME_TO_RESET_IN 2500
#define TIME_TO_RESET_OUT TIME_TO_CLOSE

/*
 * Stop the LA where it is right now.  immediate stop no matter
 * what direction it was traveling.
 */
void LA_stop(void) {
  digitalWrite(LA_CONTROL_1, LOW);
  digitalWrite(LA_CONTROL_2, LOW);
  return;
}

/*
 * Drive the LA to a closed position, either partially or
 * fully.  Motor Controller board gets LOW/HIGH, and unless you stop it, it
 * will run until it hits its internal limit switches.
 */
void LA_in(void) {
  digitalWrite(LA_CONTROL_1, LOW);
  digitalWrite(LA_CONTROL_2, HIGH);
  return;  
}

/*
 * Drive the LA to an expanded positon, either partially or
 * fully. Motor Controller board gets HIGH/LOW, and unless you stop it, it
 * will run until it hits its internal limit switches.
 */
void LA_out(void) {
  digitalWrite(LA_CONTROL_1, HIGH);
  digitalWrite(LA_CONTROL_2, LOW);
  return;  
}

/* 
 * Manually reset the LA to known position 
 * Pulling the LA in for 2.5 seconds won't hurt anything
 * because of built in endstops that cut the motor when at 
 * endstop
 */
void LA_reset(void){
  LA_in();
  delay(TIME_TO_RESET_IN);
  LA_stop();
  LA_out();
  delay(TIME_TO_RESET_OUT);
  LA_stop();  
  return;
}

void door_open(void) {
    LA_in();
    //delay(TIME_TO_OPEN);
    //LA_stop();
    return;  
}

void door_lock(void) {
    LA_out();
    delay(TIME_TO_CLOSE);
    LA_stop();
    return;  
}

// Only runs on first power up, never again.
void setup(void) {
  Serial.begin(115200);
  
  Serial.print("TheLab.ms MakerSpace front door controller. This software controls a ");
  Serial.print("linear actuator to lock/unlock the door. The signal is sent from the ");
  Serial.print("RFID key system. The signal is 12v HIGH to unlock the door.");
  Serial.print("\nSoftware version is: ");
  Serial.println(CODE_VERSION);

  // initialize digital pins as an outputs, and Signal pin as input.
  pinMode(LA_CONTROL_1, OUTPUT);
  pinMode(LA_CONTROL_2, OUTPUT);
  pinMode(SIGNAL_IN, INPUT);
  // delay really not needed, but gives Arduino time to get stable
  // just in case.
  delay(10);
  // Now put door LA into known position, since we are just starting up. 
  LA_reset();
  // Should now be 1/2" extended and the door locked.
}

// the loop function runs over and over again forever
void loop(void) {
  static bool is_door_open = false;
  int signal_state = digitalRead( SIGNAL_IN );
  
  if ( (signal_state == UNLOCK) && (is_door_open == false)){
    Serial.print("Opening Door lock, Code version: ");
    Serial.println(CODE_VERSION);
    door_open();
    is_door_open = true;
  } else if ( (signal_state == LOCK) && (is_door_open == true)){
    Serial.println("Locking Door");
    door_lock();
    is_door_open = false;
  }
}
