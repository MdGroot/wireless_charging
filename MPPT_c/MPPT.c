#include <stdio.h>
#include <stdlib.h>

#define step_size 0.0005
#define slow_filter_size 1000
#define medium_filter_size 500
#define fast_filter_size 100
#define lower_bound_value 0.95
#define lower_bound_timeout_value 100       //max value for the timeout
#define fast_medium_crossing 1              //change direction when fast mean is lower as medium mean (1 for on, 0 for off)
#define fast_slow_crossing 0                //change direction when fast mean is lower as slow mean (1 for on, 0 for off)
#define fast_lower_bound_crossing 0         //change direction when fast mean is lower as lower bound (1 for on, 0 for off)
#define fast_lower_bound_crossing_timeout 0 //timeout for lower bound to avoid switching direction twice because of ripple (1 for on, 0 for off)

int lower_bound_timeout = 0;
int set_fast = 0;
int set_bound = 0;
int set_slow = 0;
int direction = 0;
float filter_array[slow_filter_size];

int MPPT(int voltage, int amps, float duty_cycle_previous){

    //array shifting
    for(int i = slow_filter_size-1; i >= 0; i--){
        filter_array[i] = filter_array[i-1];
    }

    //adding power at front of arrays
    float power = voltage * amps;
    filter_array[1] = power;

    //calculating mean values
    float sum = 0;
    for(int i = 0; i < slow_filter_size; i++){
        sum = sum + filter_array[i];
    }
    float slow_filter_average = sum / slow_filter_size;
    float power_lower_bound = lower_bound_value * slow_filter_average;

    sum = 0;
    for(int i = 0; i < medium_filter_size; i++){
        sum = sum + filter_array[i];
    }
    float medium_filter_average = sum / medium_filter_size;

    sum = 0;
    for(int i = 0; i < fast_filter_size; i++){
        sum = sum + filter_array[i];
    }
    float fast_filter_average = sum / fast_filter_size;

    //change direction when fast mean is lower as medium mean
    if(fast_medium_crossing){
        if(medium_filter_average > fast_filter_average && set_fast == 0){
            set_fast = 1;
            if(direction == 1){
                direction = 0;
            }
            else{
                direction = 1;
            }
        }
    }

    //change direction when fast mean is lower as slow mean
    if(fast_slow_crossing){
        if(slow_filter_average > fast_filter_average && set_slow == 0){
            set_slow = 1;
            if(direction == 1){
                direction = 0;
            }
            else{
                direction = 1;
            }
        }
    }

    //change direction when fast mean is lower as lower bound
    if(fast_lower_bound_crossing){
        if(power_lower_bound > fast_filter_average && set_bound == 0 && lower_bound_timeout > lower_bound_timeout_value){
            set_bound = 1;
            lower_bound_timeout = 0;
            if(direction == 1){
                direction = 0;
            }
            else{
                direction = 1;
            }
        }
    }

    //timeout for lower bound to avoid switching direction twice because of ripple
    if(fast_lower_bound_crossing_timeout){
        if(power_lower_bound < fast_filter_average){
            lower_bound_timeout = lower_bound_timeout + 1;
        } 
    }

    //reset the set values
    if(power_lower_bound < fast_filter_average){
        set_slow = 0;
    }

    if(power_lower_bound < fast_filter_average){
        set_bound = 0;
    }

    if(medium_filter_average < fast_filter_average){
        set_fast = 0;
    }

    float duty_cycle = duty_cycle_previous + (1.213-duty_cycle_previous) * ((step_size * direction) - (step_size * (1 - direction)));
    return duty_cycle;
}
