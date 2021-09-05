#ifndef PID_HPP
#define PID_HPP

class PID
{
private:
    int control_signal_MAX, control_signal_MIN;
    double total_error, previous_error;
    unsigned long last_time;
    double control_signal;
    double reference;
    double Kp, Ki, Kd;
    int sample_period;

public:
    PID(double Kp_, double Ki_, double Kd_);
    ~PID();

    void set_parameters(double Kp_, double Ki_, double Kd_);
    void set_reference(float reference);

    double get_pid(double input);
};

#endif