/*
  MultiBlink AOScheduler Sample
  
  Blink one LED emitters with different times
 
  Attach digital pins 5, 8, 9, 11, 12, 13 to LED emitters

  This example code is in the public domain.
 
  created 16 Mar 2012 by @phcco
*/
 
#include "AOScheduler.h"

// All Tasks extends AOTask
class MultiBlink : public AOTask {

	private:

		int _pin;
		unsigned long _uptime,_downtime;
		int _state;

        public: 
		MultiBlink(int pin,unsigned long uptime, unsigned long downtime){
			_pin = pin;
			_uptime = uptime;
			_downtime = downtime;
			_state = LOW;
		}

		int setup(){
			this->task_expected_period = _uptime;
			pinMode(_pin, OUTPUT);    
			digitalWrite(_pin,_state = HIGH);
		}

		int loop(){
			if(_state==LOW){
				this->task_expected_period = _uptime;
				_state = HIGH;
			}else{
				this->task_expected_period = _downtime;
				_state = LOW;
			}
			digitalWrite(_pin, _state);
			return AO_OK; // Return AO_EXIT to not schedule this task again
		}
  
};

// Create Scheduler
AOScheduler aos;

// Create Tasks
//MultiBlink blinker1(pin, time on, time off);
MultiBlink blinker1(13,SECOND,2*SECOND);
MultiBlink blinker2(12,MILLISECOND,1*SECOND);
MultiBlink blinker3(11,MILLISECOND,1*SECOND);
MultiBlink blinker4(9,SECOND/2,3*SECOND);
MultiBlink blinker5(8,MILLISECOND,1*SECOND);
MultiBlink blinker6(5,SECOND,4*SECOND);

void setup(){
	// Add Tasks to Scheduler
	aos.add(&blinker1);
	aos.add(&blinker2);
	aos.add(&blinker3);
	aos.add(&blinker4);
	aos.add(&blinker5);
	aos.add(&blinker6);
}

void loop(){
	// Run Scheduler
	aos.run();
	// Run method not return until all tasks are performed
}