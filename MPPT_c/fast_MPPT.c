#include <stdio.h>
#include <stdlib.h>

#define step_size 0.0005
#define slow_filter_size 5000
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
int counter = 0;
float slow_filter_array[slow_filter_size];
float medium_filter_array[medium_filter_size];
float fast_filter_array[fast_filter_size];
float slow_filter_average;
float medium_filter_average;
float fast_filter_average;
float power_lower_bound;

int MPPT(int voltage, int amps, float duty_cycle_previous){

    float power = voltage * amps;
    //calculating mean values
    float sum = 0;
    
    if(slow_filter_array[counter%slow_filter_size] == 0){
        for(int i = 0; i < slow_filter_size; i++){
            sum = sum + slow_filter_array[i];
        }
        slow_filter_average = sum / slow_filter_size;
        power_lower_bound = lower_bound_value * slow_filter_average;
    }
    else {
        slow_filter_average = slow_filter_average - slow_filter_array[counter%slow_filter_size] * slow_filter_size + power * slow_filter_size;
    }

    sum = 0;
    if(medium_filter_array[counter%medium_filter_size] == 0){
        for(int i = 0; i < medium_filter_size; i++){
            sum = sum + medium_filter_array[i];
        }
        medium_filter_average = sum / medium_filter_size;
    }
    else {
        medium_filter_average = medium_filter_average - medium_filter_array[counter%medium_filter_size] * medium_filter_size + power * medium_filter_size;
    }

    sum = 0;
    if(fast_filter_array[counter%fast_filter_size] == 0){
        for(int i = 0; i < fast_filter_size; i++){
            sum = sum + fast_filter_array[i];
        }
        fast_filter_average = sum / fast_filter_size;
    }
    else {
        fast_filter_average = fast_filter_average - fast_filter_array[counter%fast_filter_size] * fast_filter_size + power * fast_filter_size;
    }

    //adding replacing value in array
    slow_filter_array[counter%slow_filter_size] = power;
    medium_filter_array[counter%medium_filter_size] = power;
    fast_filter_array[counter%fast_filter_size] = power;

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
