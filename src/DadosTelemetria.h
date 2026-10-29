#ifndef DADOS_TELEMETRIA_H
#define DADOS_TELEMETRIA_H

#include <Arduino.h>

enum EstadoMoto {
  NORMAL,
  ALERTA_QUEDA,
  ACIDENTE_CONFIRMADO
};

struct DadosTelemetria {
  char dataHora[20];
  int milissegundos;
  float ax, ay, az;
  float gx, gy, gz;
  float gTotal;
  float giroTotal;
  float anguloLateral;
  float anguloFrontal;
  float movimentoFrontal;
  const char* lateralTexto;
  const char* frontalTexto;

  // Campos do GPS
  float latitude;
  float longitude;
  float velocidadeKmh;
  bool gpsValido;
};

#endif