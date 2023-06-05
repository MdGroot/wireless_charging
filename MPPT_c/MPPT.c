function [D, power_out, power_out_avg_fast, power_out_avg_slow, power_out_avg_super_slow, power_out_lower_bound] = fcn(Voltage, Amps)

persistent timeout D_previous direction_previous fast_power_average super_slow_power_average slow_power_average power_lower_bound slow_power_array super_slow_power_array fast_power_array setfast setslow; %counter power_previous 
step_size = 0.0005;
super_slow_filter_size = 5000;
slow_filter_size = 500;
fast_filter_size = 100;
lower_bound_value = 0.95;


if isempty(D_previous)
    setfast = 0;
    timeout = 0;
    setslow = 0;
    D_previous = 0.9;
    direction_previous = 0;
    super_slow_power_array = zeros(super_slow_filter_size,1);
    slow_power_array = zeros(slow_filter_size,1);
    fast_power_array = zeros(fast_filter_size,1);
end

super_slow_power_array(2:end) = super_slow_power_array(1:end-1);
slow_power_array(2:end) = slow_power_array(1:end-1);
fast_power_array(2:end) = fast_power_array(1:end-1);



power = Voltage * Amps;
super_slow_power_array(1) = power;
slow_power_array(1) = power;
fast_power_array(1) = power;

super_slow_power_average = mean(super_slow_power_array);
slow_power_average = mean(slow_power_array);
fast_power_average = mean(fast_power_array);
power_lower_bound = lower_bound_value * super_slow_power_average;
direction = direction_previous;

if slow_power_average > fast_power_average && setfast == 0
    setfast = 1;
    if direction_previous == 1
        direction = 0;
    else
        direction = 1;
    end
end

if power_lower_bound < fast_power_average
    timeout = timeout + 1;
end

%if power_lower_bound > fast_power_average && setslow == 0 && timeout > 100
%    setslow = 1;
%    timeout = 0;
%    if direction_previous == 1
%        direction = 0;
%    else
%        direction = 1;
%    end
%end

if power_lower_bound < fast_power_average
    setslow = 0;
end

if slow_power_average < fast_power_average
    setfast = 0;
end

D = D_previous + (1.213-D_previous) * ((step_size * direction) - (step_size * (1 - direction)));

D_previous = D;
power_out_avg_slow = slow_power_average;
power_out = power;
power_out_avg_fast = fast_power_average;
power_out_avg_super_slow = super_slow_power_average;
power_out_lower_bound = power_lower_bound;
direction_previous = direction;
