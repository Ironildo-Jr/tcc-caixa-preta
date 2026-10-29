#include <Arduino.h>
#include <Wire.h>

#include "DadosTelemetria.h"
#include "SensorIMU.h"
#include "SensorGPS.h"
#include "AnalisadorSinistro.h"
#include "BufferCircular.h"
#include "ArmazenamentoSD.h"

SensorIMU imu;
SensorGPS gps;
AnalisadorSinistro analisador;
BufferCircular buffer;
ArmazenamentoSD cartaoSD;

// Flag para garantir que o SD só será gravado UMA vez após o acidente
bool sinistroGravado = false; 

void setup() {
  Serial.begin(115200);

  Serial.println("\n=================================");
  Serial.println("--- CAIXA PRETA: INICIANDO ---");
  Serial.println("=================================");
  
  Wire.begin(21, 22);

  Serial.println("Inicializando Acelerometro...");
  imu.iniciar(0x69);
  Serial.println("Calibrando Acelerometro...");
  imu.calibrar();
  Serial.println("Inicializando GPS...");
  gps.iniciar(16, 17, 9600); // Inicializa UART2 (RX2=16, TX2=17)
  Serial.println("Inicializando Cartão SD...");
  cartaoSD.iniciar(5);
}

void loop() {
  // 1. Lê a IMU
  DadosTelemetria dados = imu.lerDados();

  // 2. Atualiza e injeta os dados de GPS
  gps.atualizarDados(dados);

  // 2. Se o acidente JÁ FOI GRAVADO, não atualiza o buffer e nem grava mais nada
  if (sinistroGravado) {
    // Opcional: manter apenas um aviso no Serial ou acionar um LED de emergência
    delay(1000); 
    return; // Sai do loop sem alterar a memória
  }

  // 3. Se ainda não houve acidente gravado, atualiza o buffer circular na RAM
  buffer.adicionar(dados);

  // 4. Avalia a física do veículo
  EstadoMoto estado = analisador.verificar(dados, millis());

  // 5. Se o acidente for confirmado AGORA, realiza a gravação do buffer
  if (estado == ACIDENTE_CONFIRMADO) {
    Serial.println("ACIDENTE CONFIRMADO! Gravando histórico no Cartão SD...");
    
    // Grava as 50 amostras dos 10 segundos anteriores
    cartaoSD.salvarLogSinistro(buffer); 
    
    // Marca a flag para bloquear gravações posteriores
    sinistroGravado = true; 
    
    Serial.println("Logs protegidos contra sobrescrita.");
  }

  delay(100); // Taxa de amostragem (5 Hz)
}