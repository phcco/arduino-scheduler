/*
  Arduino Task Scheduler
 
  created 16 Mar 2012 by @phcco
 */
#ifndef AOScheduler_cpp
#define AOScheduler_cpp

#include "AOScheduler.h"

#include <Arduino.h>

int AOTask::pid_count = 1;

AOScheduler::AOScheduler() {
    next = NULL;
    start_micro = 0;
    now_micro = micros();
}

int AOScheduler::add(AOTask *task){
  #ifdef DEBUG_SCHED
  Serial.print("Added task ");
  Serial.println(task->pid);
  #endif
  task->setup();
  if(next==NULL){
     next = task;
  }else{
     AOTask *t = next;
     while(t->next!=NULL){
        t = t->next;
     }
     t->next = task;
  }
  return true;
}

int AOScheduler::run(){
  AOTask *n = next;
  AOTask *tmp,*tmp2,*nn;
  unsigned long tdiff;
  while(n!=NULL){    
    #ifdef DEBUG_SCHED
    tmp = n;
    Serial.println("---- Sched");
    while(tmp!=NULL){
      Serial.print(tmp->pid);
      Serial.print(" -> ");
      Serial.println(tmp->task_estimated_start);
      tmp = tmp->next;
    }
    #endif
    now_micro = micros();
    if(n->task_estimated_start>now_micro){
      tdiff = n->task_estimated_start-now_micro;
      if(tdiff<=16383){
        delayMicroseconds(tdiff);
      }else{
		// now_micro = tdiff%1000;
        delay(tdiff/1000);
		// if(now_micro>4){
			// delayMicroseconds(now_micro-4);
		// }
      }
    }
    now_micro = micros();
    switch(n->loop()){
      case AO_OK: case AO_YELD:
        n->task_estimated_length = micros()-now_micro;
        n->task_estimated_start = now_micro+n->task_expected_period;
        n->task_estimated_end = n->task_estimated_start+n->task_estimated_length;
        if(n->next!=NULL){
          tmp2 = NULL;
          nn = tmp = n->next;
          if(tmp->task_estimated_start < n->task_estimated_end){
            while(tmp!=NULL){
              #ifdef DEBUG_SCHED
              Serial.println(tmp->task_estimated_end);
              #endif
              if(tmp->task_estimated_end<=n->task_estimated_start){
                tmp2 = tmp;
                tmp = tmp2->next;
              }else if(tmp2!=NULL){
                n->next = tmp2->next;
                tmp2->next = n;
                break;
              }else{
                n->next = tmp->next;
                tmp->next = n;
                break;
              }
            }
            if(tmp==NULL){
              tmp2->next = n; 
              n->next = NULL;
            }
            n = nn;
          }
        }
        break;
      case AO_EXIT:
        next = n->next;
        return true;
        break;
    }
  }
}

#endif