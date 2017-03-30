/*
  Arduino Task Scheduler
 
  created 16 Mar 2012 by @phcco
 */
 
#ifndef AOScheduler_h
#define AOScheduler_h

#ifndef SECOND
#define SECOND 1000000
#endif
#ifndef MILLISECOND
#define MILLISECOND 1000
#endif

#include <Arduino.h>

#define AO_OK 1
#define AO_EXIT 2
#define AO_YELD 4

class AOTask {
  public: 
    volatile unsigned long task_estimated_length;
    volatile unsigned long task_estimated_start;
    volatile unsigned long task_estimated_end;
    volatile unsigned long task_expected_period;
    int pid;
    static int pid_count;
    int task_last_response;
    int task_type;
    AOTask *next;
   
    AOTask(){
	  task_last_response = AO_OK;
      task_expected_period = 0;
      task_estimated_start = 0;
      task_estimated_length = 0;
      task_estimated_end = 0;
      next = NULL;
      pid = AOTask::pid_count++;
    };
    
    virtual int setup(){};
    virtual int loop(){return AO_EXIT;};
	virtual int digitalPinChanged(int pin, int state, AOTask *pinmonitor){};
	virtual int analogPinChanged(int pin, int state, AOTask *pinmonitor){};
};

class AOScheduler {
  protected:
    AOTask *next;
    unsigned long start_micro;
    unsigned long now_micro;

  public:
    AOScheduler();
  
    int add(AOTask *task);
    
    int run();
    
};


class DigitalPinMonitor : public AOTask{
  
  protected:
  
    int _last_state;
    int _pin;
	unsigned long _period;
    AOTask *_task;
    
   public:
    DigitalPinMonitor (int pin, AOTask *task){
        _pin = pin;
        _task = task;
		_period = MILLISECOND/4;
    }
    DigitalPinMonitor (int pin, AOTask *task, unsigned long period){
        _pin = pin;
        _task = task;
		_period = period;
    }
    int setup(){
      pinMode(_pin,INPUT);
      _last_state = 0;
      this->task_expected_period = _period;
    }
    int loop(){
      int new_state = digitalRead(_pin);
      if(_last_state!=new_state){
        _last_state = new_state;
        _task->digitalPinChanged(_pin,new_state,this);
      }
      return task_last_response;
    }
  
};

class AnalogPinMonitor : public AOTask{
  
  protected:
  
    int _last_state;
    int _pin;
	int _sensibility;
	unsigned long _period;
    AOTask *_task;
    
  public:
    AnalogPinMonitor (int pin, AOTask *task){
        _pin = pin;
        _task = task;
		_period = MILLISECOND/4;
		_sensibility = 1;
    }
    AnalogPinMonitor (int pin, AOTask *task, unsigned long period){
        _pin = pin;
        _task = task;
		_period = period;
		_sensibility = 1;
    }
    int setup(){
      _last_state = 0;
      this->task_expected_period = _period;
    }
	
	int sensibility(int sens){
		int old_sens = _sensibility;
		_sensibility = sens;
		return old_sens;
	}
	
    int loop(){
      int new_state = analogRead(_pin);
      if(_last_state+_sensibility<new_state || _last_state-_sensibility>new_state){
        _last_state = new_state;
        _task->digitalPinChanged(_pin,new_state,this);
      }
      return task_last_response;
    }
};



#endif
