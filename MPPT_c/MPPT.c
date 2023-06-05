#include <stdio.h>
#include <stdlib.h>

#define step_size 0.0005
#define slow_filter_size 5000
#define medium_filter_size 500
#define fast_filter_size 100
#define lower_bound_value 0.95

int timeout = 0;
int set_fast = 0;
int set_slow = 0;
int direction = 0;
float slow_filter_array[slow_filter_size];
float medium_filter_array[medium_filter_size];
float fast_filter_array[fast_filter_size];

int MPPT(int voltage, int amps, float duty_cycle_previous){

    for(int i = slow_filter_size-1; i >= 0; i--){
        slow_filter_array[i] = slow_filter_array[i-1];
    }

    for(int i = medium_filter_size-1; i >= 0; i--){
        medium_filter_array[i] = medium_filter_array[i-1];
    }

    for(int i = fast_filter_size-1; i >= 0; i--){
        fast_filter_array[i] = fast_filter_array[i-1];
    }


    float power = voltage * amps;
    slow_filter_array[1] = power;
    medium_filter_array[1] = power;
    fast_filter_array[1] = power;

    float sum = 0;
    for(int i = 0; i < slow_filter_size; i++){
        sum = sum + slow_filter_array[i];
    }
    float slow_filter_average = sum / slow_filter_size;

    sum = 0;
    for(int i = 0; i < medium_filter_size; i++){
        sum = sum + medium_filter_array[i];
    }
    float medium_filter_average = sum / medium_filter_size;

    sum = 0;
    for(int i = 0; i < fast_filter_size; i++){
        sum = sum + fast_filter_array[i];
    }
    float fast_filter_average = sum / fast_filter_size;
    float power_lower_bound = lower_bound_value * slow_filter_average;

    if(medium_filter_average > fast_filter_average && set_fast == 0){
        set_fast = 1;
        if(direction == 1){
            direction = 0;
        }
        else{
            direction = 1;
        }
    }

    if(power_lower_bound < fast_filter_average){
        timeout = timeout + 1;
    } 

    //if(power_lower_bound > fast_filter_average && set_slow == 0 && timeout > 100){
    //    set_slow = 1;
    //    timeout = 0;
    //    if(direction_previous == 1){
    //        direction = 0;
    //    }
    //    else{
    //        direction = 1;
    //    }
    //}

    if(power_lower_bound < fast_filter_average){
        set_slow = 0;
    }

    if(medium_filter_average < fast_filter_average){
        set_fast = 0;
    }

    float duty_cycle = duty_cycle_previous + (1.213-duty_cycle_previous) * ((step_size * direction) - (step_size * (1 - direction)));
    return duty_cycle;
}
