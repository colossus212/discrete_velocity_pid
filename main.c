/* Name: main.c
 * Author: Michael de Silva <michael@mwdesilva.com>
 * Copyright: MIT License
 */

#include <iostream>
#include <fstream>
using namespace std;

class PID
{
  // Gain parameters
  public:
    float Kp;            // Loop gain parameter
    float Ti;            // Integrator time constant
    float Td;            // Differentiator time constant
    float delT;          // Update time interval

    // Setpoint parameters
    float input;         // Input from A/D converter
    float setpt;         // Regulated level to maintain
    float output;
    float output_max;
    float control0;
    float control1;

    float e0, e1, e2;

    // Controller state
    float integral;      // Summation of setpoint errors
    float deriv;         // Previous setpoint error
};

void calcStep(PID *pid);

void calcStep(PID *pid) {
  float q0, q1, q2 = 0;

  pid->e2 = pid->e1;
  pid->e1 = pid->e0;
  // Assume the A/D reading is exactly that of the previous output
  pid->e0 = pid->setpt - (pid->output * 0.1);
  cout << "e0: " << pid->e0 << ", e1: " << pid->e1 << ", e2: " << pid->e2 << '\n';

  q0 = pid->Kp * (1 + (pid->delT/pid->Ti) + (pid->Td/pid->delT));
  q1 = (-1 * (pid->Kp)) * (1 + ((2 * pid->Td)/pid->delT));
  q2 = (pid->Kp * pid->Td) / pid->delT;
  cout << "q0: " << q0 << ", q1: " << q1 << ", q2: " << q2 << '\n';

  pid->control1 = pid->control0;

  pid->control0 = ((q0 * pid->e0)+(q1 * pid->e1)+(q2 * pid->e2));
  cout << "control0: " << pid->control0 << ", control1: " << pid->control1 << '\n';

  pid->output = pid->control0 + pid->control1;

  // Set max saturation of output
  if (pid->output > pid->output_max){
    pid->output = pid->output_max;
  } else if (pid->output < 0) {
    pid->output = 0;
  }
  cout << "absolute control: " << pid->output << "\n\n";
}

int main (){
  PID pid;
  pid.Kp = 4;
  pid.Ti = 72;
  pid.Td = 0.01;
  pid.delT = 10;
  pid.e0 = pid.e1 = pid.e2 = 0;

  pid.setpt = 1000;
  pid.input = 20;
  pid.output = 80;
  pid.output_max = 2000;

  ofstream plot;
  plot.open("plot.dat");

  for(int i = 0; i < 40; i++){
    calcStep(&pid);
    plot << i << '\t' << pid.output << '\t' << pid.e0 << '\t' << pid.e1 << '\t' << pid.e2  << '\n';
  }

  plot.close();

  return 0;
}



