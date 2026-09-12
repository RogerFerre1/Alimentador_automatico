// PETSFLOW - Alimentador Automático

// Bibliotecas

#include <Arduino.h>
#include <ESP32Servo.h>
#include "BluetoothSerial.h"
#include <Wire.h>
#include <RTClib.h>

// Componentes

Servo servo1;
Servo servo2;

BluetoothSerial SerialBT;

RTC_DS3231 rtc;

// Pinos

const int servo1_PIN = 13;
const int servo2_PIN = 12;

const int sensorIR = 35;
const int sensorLaser = 14;

// Variáveis

// 1 - Servos

int anguloFechado = 0;
int anguloAberto = 180;

// 2 - Sensor LDR

int estadoAnteriorLDR;
int estadoAtualLDR;

// 3 - Sensor IR

int estadoAnteriorIR;
int estadoAtualIR;

// 4 - Alimentação

unsigned long tempoInicio;
bool alimentando = false;

unsigned long tempoAlimentacao = 2000;

// 5 - Bluetooth

String device_name = "Petsflow";

// 6 - Temporizador

int horaProgramada1 = 8;
int minutoProgramado1 = 0;

int horaProgramada2 = 14;
int minutoProgramado2 = 51;

int horaProgramada3 = 18;
int minutoProgramado3 = 0;

bool horario1Executado = false;
bool horario2Executado = false;
bool horario3Executado = false;

int ultimoDia = -1;

// Declaração das Funções

void iniciarBluetooth();
void iniciarRTC();

void configurarSensores();
void configurarServos();

void verificarBluetooth();

void verificarIR();
void verificarNivelRacao();

void iniciarAlimentacao();
void verificarAlimentacao();

void verificarHorario();
//void verificarHorarioProgramado(int hora, int minuto, bool &executado);

// Setup 

void setup(){

  Serial.begin(115200);

  iniciarRTC();

  iniciarBluetooth();

  configurarSensores();

  configurarServos();
}

// Loop

void loop(){

  verificarBluetooth();

  verificarIR();

  verificarNivelRacao();

  verificarAlimentacao();

  delay(500);

  verificarHorario();
}

// Inicialização do RTC

void iniciarRTC(){

  Wire.begin(22, 23);

  if(!rtc.begin()){
    Serial.println("DS3231 não encontrado!");
    while(1);
  }

  Serial.println("DS3231 encontrado!");

  // Usar apenas uma vez para sincronizar o horário RTC com o PC
  // Comentar após a primeira vez compilado para não sobrescrever
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)) + TimeSpan(0, 0, 0, 26)); 
}

// Inicialização do Bluetooth

void iniciarBluetooth(){
  
  #if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
  #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
  #endif

  #if !defined(CONFIG_BT_SPP_ENABLED)
  #error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
  #endif

  SerialBT.begin(device_name);

  // SerialBT.deleteAllBondeDevices();
  // Use apenas se quiser apagar os dispositivos pareados.

  Serial.printf("O dispositivo com nome \"%s\" foi iniciado. \n", device_name.c_str());
  Serial.println("Agora você pode parear com o Bluetooth!");
}

// Configuração dos sensores

void configurarSensores(){

  pinMode(sensorIR, INPUT);
  pinMode(sensorLaser, INPUT);

  estadoAnteriorLDR = digitalRead(sensorLaser);
  estadoAnteriorIR = digitalRead(sensorIR);
}

// Configuração dos servos

void configurarServos(){

  servo1.attach(servo1_PIN);
  servo2.attach(servo2_PIN);

  // Inicia o alimentador fechado

  servo1.write(anguloFechado);
  servo2.write(180 - anguloFechado);

  delay(500);
}

// Verificação do bluetooth

void verificarBluetooth(){
  
  // Serial -> Bluetooth
  if(Serial.available()){
    SerialBT.write(Serial.read());
  }

  // Bluetooth -> Serial
  if(SerialBT.available()){
    Serial.write(SerialBT.read());
  }

  delay(20);
}

// Verificação do sensor IR

void verificarIR(){
  
  estadoAtualIR = digitalRead(sensorIR);

  if((estadoAnteriorIR == HIGH) && (estadoAtualIR == LOW) && !alimentando){
    iniciarAlimentacao();
  }

  estadoAnteriorIR = estadoAtualIR;
}

// Verificação do nível de ração

void verificarNivelRacao(){

  estadoAtualLDR = digitalRead(sensorLaser);

  if((estadoAnteriorLDR == HIGH) && (estadoAtualLDR == LOW)){
    Serial.println("Nível de ração baixa, favor reabastecer o reservatório");
    SerialBT.println("Nível de ração baixa, favor reabastecer o reservatório");
  }

  estadoAnteriorLDR = estadoAtualLDR;
}

// Iniciar alimentação

void iniciarAlimentacao(){

  alimentando = true;
  tempoInicio = millis();

  // Abre o alimentador

  servo1.write(anguloAberto);
  servo2.write(180 - anguloAberto);
}

// Verificar alimentação

void verificarAlimentacao(){

  if(alimentando && (millis() - tempoInicio >= tempoAlimentacao)){

    // Fecha o alimentador

    servo1.write(anguloFechado);
    servo2.write(180 - anguloFechado);

    alimentando = false;
  }
}

// Verificar o horário programado

void verificarHorario(){
  
  DateTime agora = rtc.now();

  if(agora.day() != ultimoDia){

    horario1Executado = false;
    horario2Executado = false;
    horario3Executado = false;

    ultimoDia = agora.day();
  }

  // Horário 1
  if((horaProgramada1 == agora.hour()) && (minutoProgramado1 == agora.minute()) && !horario1Executado){
    
    iniciarAlimentacao();

    horario1Executado = true;
  }

  // Horário 2
  if((horaProgramada2 == agora.hour()) && (minutoProgramado2 == agora.minute()) && !horario2Executado){
    
    iniciarAlimentacao();

    horario2Executado = true;
  }

  // Horário 3
  if((horaProgramada3 == agora.hour()) && (minutoProgramado3 == agora.minute()) && !horario3Executado){
    iniciarAlimentacao();

    horario3Executado = true;
  }
}

/*
void verificarHorarioProgramado(int hora, int minuto, bool &executado){

  DateTime agora = rtc.now();

  if(hora)
}
*/















