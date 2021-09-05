#include "pid.hpp"

PID::PID(double Kp_, double Ki_, double Kd_)
{
    this->control_signal_MIN = -100.0;
    this->control_signal_MAX = 100.0;
    this->previous_error = 0.0;
    this->sample_period = 1.0;
    this->reference = 0.0;
    this->Kp = Kp_;
    this->Ki = Ki_;
    this->Kd = Kd_;
}

PID::~PID() {}

void PID::set_reference(float reference)
{
    this->reference = (double)reference;
}

void PID::set_parameters(double Kp_, double Ki_, double Kd_)
{
    this->Kp = Kp_;
    this->Ki = Ki_;
    this->Kd = Kd_;
}

double PID::get_pid(double internal_temperature)
{

    double erro = this->reference - internal_temperature;

    this->total_error += erro; // Acumula o erro (Termo Integral)

    if (this->total_error >= this->control_signal_MAX)
    {
        this->total_error = this->control_signal_MAX;
    }
    else if (this->total_error <= this->control_signal_MIN)
    {
        this->total_error = this->control_signal_MIN;
    }

    double delta_error = erro - this->previous_error; // Diferença entre os erros (Termo Derivativo)

    this->control_signal = this->Kp * erro + (this->Ki * this->sample_period) * this->total_error + (this->Kd / this->sample_period) * delta_error; // PID calcula sinal de controle

    if (this->control_signal >= this->control_signal_MAX)
    {
        this->control_signal = this->control_signal_MAX;
    }
    else if (this->control_signal <= this->control_signal_MIN)
    {
        this->control_signal = this->control_signal_MIN;
    }

    this->previous_error = erro;

    return this->control_signal;
}
