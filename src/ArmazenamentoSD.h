#ifndef ARMAZENAMENTO_SD_H
#define ARMAZENAMENTO_SD_H

#include <SD.h>
#include "BufferCircular.h"

class ArmazenamentoSD {
  private:
    const char* NOME_ARQUIVO = "/CAIXA.CSV";

  public:
    bool iniciar(uint8_t pinoCS) {
      if (!SD.begin(pinoCS)) return false;

      if (!SD.exists(NOME_ARQUIVO)) {
        Serial.println("Arquivo não encontrado. Criando novo arquivo CSV...");
        File dataFile = SD.open(NOME_ARQUIVO, FILE_WRITE);
        if (dataFile) {
          dataFile.println("Data_Hora,Milissegundos,Acel_X,Acel_Y,Acel_Z,Gyro_X,Gyro_Y,Gyro_Z,G_Total,Giro_Total,Angulo_Lateral,Angulo_Frontal,Movimento_Frontal,Lateral,Frontal,Latitude,Longitude,Velocidade_KMH,GPS_Valido,Estado");
          dataFile.close();
        }
      }
      return true;
    }

    void salvarAmostra(DadosTelemetria d, const char* estadoTexto) {
      File dataFile = SD.open(NOME_ARQUIVO, FILE_WRITE);
      if (dataFile) {
        dataFile.print(d.dataHora); dataFile.print(",");
        dataFile.print(d.milissegundos); dataFile.print(",");
        dataFile.print(d.ax); dataFile.print(",");
        dataFile.print(d.ay); dataFile.print(",");
        dataFile.print(d.az); dataFile.print(",");
        dataFile.print(d.gx); dataFile.print(",");
        dataFile.print(d.gy); dataFile.print(",");
        dataFile.print(d.gz); dataFile.print(",");
        dataFile.print(d.gTotal); dataFile.print(",");
        dataFile.print(d.giroTotal); dataFile.print(",");
        dataFile.print(d.anguloLateral); dataFile.print(",");
        dataFile.print(d.anguloFrontal); dataFile.print(",");
        dataFile.print(d.movimentoFrontal); dataFile.print(",");
        dataFile.print(d.lateralTexto); dataFile.print(",");
        dataFile.print(d.frontalTexto); dataFile.print(",");
        
        // Novas colunas salvas no CSV
        dataFile.print(d.latitude, 6); dataFile.print(",");
        dataFile.print(d.longitude, 6); dataFile.print(",");
        dataFile.print(d.velocidadeKmh, 1); dataFile.print(",");
        dataFile.print(d.gpsValido ? "SIM" : "NAO"); dataFile.print(",");
        
        dataFile.println(estadoTexto);
        dataFile.close();
      }
    }

    void salvarLogSinistro(BufferCircular& buffer) {
      for (int i = 0; i < buffer.getTamanho(); i++) {
        DadosTelemetria d = buffer.obterAmostra(i);
        salvarAmostra(d, "ACIDENTE_CONFIRMADO");
      }
    }
};

#endif