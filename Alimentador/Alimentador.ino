// Bibliotecas

#include <ESP32Servo.h>

// Pinos/Variáveis

Servo servo1;
Servo servo2;

const int servo1_PIN = 13;
const int servo2_PIN = 12;
const int sensorIR = 35;

int anguloFechado = 0;
int anguloAberto = 180;

void setup(){
  pinMode(sensorIR, INPUT);
  
  servo1.attach(servo1_PIN);
  servo2.attach(servo2_PIN);

  moverServos(anguloFechado);

  delay(500);
}

void loop(){
  int estadoSensor = digitalRead(sensorIR);

  if(estadoSensor == HIGH){
    moverServos(anguloAberto);
  } else{
    moverServos(anguloFechado);
  }

  delay(500);
}

void moverServos(int angulo){
  servo1.write(angulo);
  servo2.write(180 - angulo);
}