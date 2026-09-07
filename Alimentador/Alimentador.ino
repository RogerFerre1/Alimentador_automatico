// Bibliotecas

#include <Arduino.h>
#include <ESP32Servo.h>
#include "BluetoothSerial.h"

// Instância dos componentes

Servo servo1;
Servo servo2; 

BluetoothSerial SerialBT;

// Pinos/Variáveis

const int servo1_PIN = 13;
const int servo2_PIN = 12;
const int sensorIR = 35;
const int sensorLaser = 14;

int anguloFechado = 0;
int anguloAberto = 180;

int estadoAnterior;
int estadoAtual;

String device_name = "Petsflow";

// Checando se o Bluetooth está disponível

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Checando Perfil da Porta Serial

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif

void setup(){
  // Bluetooth

  Serial.begin(115200);
  SerialBT.begin(device_name);
  //SerialBT.deleteAllBondedDevices(); // Tire do comentário para deletar dispositivos pareados. Precisa ser chamado depois do begin 
  Serial.printf("O dispositivo com nome \"%s\" foi iniciado. \n Agora você pode parear com o Bluetooth!\n", device_name.c_str());

  // pinModes

  pinMode(sensorIR, INPUT);
  pinMode(sensorLaser, INPUT);
  
  // Servo motores

  servo1.attach(servo1_PIN);
  servo2.attach(servo2_PIN);

  moverServos(anguloFechado);

  delay(500);

  // Sensor LDR

  estadoAnterior = digitalRead(sensorLaser);
}

void loop(){

  // Bluetooth

  if(Serial.available()){
    SerialBT.write(Serial.read());
  }
  if(SerialBT.available()){
    Serial.write(SerialBT.read());
  }
  delay(20);

  // Sensor IR

  int estadoSensor = digitalRead(sensorIR);

  if(estadoSensor == HIGH){
    moverServos(anguloAberto);
  } else{
    moverServos(anguloFechado);
  }

  delay(500);
  
  // Sensor LDR

  estadoAtual = digitalRead(sensorLaser);
  
  if(estadoAnterior == HIGH && estadoAtual == LOW){
    Serial.println("Nível de ração baixa, favor reabastecer o reservatório");
    SerialBT.println("Nível de ração baixa, favor reabastecer o reservatório");
  }
  estadoAnterior = estadoAtual;

  delay(500);
}

// Servo motores

void moverServos(int angulo){
  servo1.write(angulo);
  servo2.write(180 - angulo);
}