#include <Arduino.h>
// tenemos 3 botones 
// 4 leds de decremento y incremento

#define BINCREMENTO 12
#define BDECREMENTO 14
#define BREINICIO 33
#define LED1_B 16
#define LED2_B 17
#define LED3_B 18
#define LED4_B 19
//4 leds para el timer 
#define LED1_T 21
#define LED2_T 22
#define LED3_T 23
#define LED4_T 25
//la alarma se define con otra led en vez de buzzer
#define ALARMA 26
//volatil es para que inicie desde 0 cada uno
volatile int contadorBotones = 0;
volatile int contadorTemporizador = 0;
hw_timer_t * temporizador = NULL;

// Función para actualizar los LEDs de los botones
void actualizarLEDsBotones() {
  digitalWrite(LED1_B, (contadorBotones & 0x01) ? HIGH : LOW);
  digitalWrite(LED2_B, (contadorBotones & 0x02) ? HIGH : LOW);
  digitalWrite(LED3_B, (contadorBotones & 0x04) ? HIGH : LOW);
  digitalWrite(LED4_B, (contadorBotones & 0x08) ? HIGH : LOW);
}

// Función para actualizar los LEDs del temporizador
void actualizarLEDsTemporizador() {
  digitalWrite(LED1_T, (contadorTemporizador & 0x01) ? HIGH : LOW);
  digitalWrite(LED2_T, (contadorTemporizador & 0x02) ? HIGH : LOW);
  digitalWrite(LED3_T, (contadorTemporizador & 0x04) ? HIGH : LOW);
  digitalWrite(LED4_T, (contadorTemporizador & 0x08) ? HIGH : LOW);
}

// Interrupción para el botón de incremento
void IRAM_ATTR incrementarContador() {
  contadorBotones = (contadorBotones + 1) % 16; // Contador de 4 bits
  actualizarLEDsBotones();
}

// Interrupción para el botón de decremento
void IRAM_ATTR decrementarContador() {
  contadorBotones = (contadorBotones - 1 + 16) % 16; // Evitar valores negativos
  actualizarLEDsBotones();
}

// Interrupción del temporizador
void IRAM_ATTR enTemporizador() {
  contadorTemporizador = (contadorTemporizador + 1) % 16;
  actualizarLEDsTemporizador();
  if (contadorTemporizador == contadorBotones) {
    digitalWrite(INDICADOR, !digitalRead(INDICADOR)); // Cambiar el estado del pin
    contadorTemporizador = 0; // Reiniciar el contador del temporizador
  }
}

// Interrupción para el botón capacitivo
void IRAM_ATTR reiniciarTemporizador() {
  contadorTemporizador = 0;
  timerAlarmWrite(temporizador, 250000, true); // Reiniciar el temporizador
}

void setup() {
  pinMode(BINCREMENTO, INPUT_PULLUP);
  pinMode(BDECREMENTO, INPUT_PULLUP);
  pinMode(LED1_B, OUTPUT);
  pinMode(LED2_B, OUTPUT);
  pinMode(LED3_B, OUTPUT);
  pinMode(LED4_B, OUTPUT);
  pinMode(LED1_T, OUTPUT);
  pinMode(LED2_T, OUTPUT);
  pinMode(LED3_T, OUTPUT);
  pinMode(LED4_T, OUTPUT);
  pinMode(ALARMA, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(BINCREMENTO), incrementarContador, FALLING);
  attachInterrupt(digitalPinToInterrupt(BDECREMENTO), decrementarContador, FALLING);
  touchAttachInterrupt(BREINICIO, reiniciarTemporizador, 40); // El umbral se puede ajustar según sea necesario

  temporizador = timerBegin(0, 80, true); // 80 prescaler, incrementa cada 1us
  timerAttachInterrupt(temporizador, &enTemporizador, true);
  timerAlarmWrite(temporizador, 250000, true); // 250ms
  timerAlarmEnable(temporizador);

  actualizarLEDsBotones();
  actualizarLEDsTemporizador();
}

void loop() {
  // No se necesita hacer nada aquí
}