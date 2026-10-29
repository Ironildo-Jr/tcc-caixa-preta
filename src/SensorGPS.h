#ifndef SENSOR_GPS_H
#define SENSOR_GPS_H

#include <Arduino.h>
#include <TinyGPS++.h>

class SensorGPS {
  private:
    TinyGPSPlus gps;
    HardwareSerial serialGPS;

  public:
    SensorGPS() : serialGPS(2) {} // Utiliza a HardwareSerial 2 (UART2)

    void iniciar(uint8_t pinoRX = 16, uint8_t pinoTX = 17, uint32_t baudRate = 9600) {
      serialGPS.begin(baudRate, SERIAL_8N1, pinoRX, pinoTX);
    }

    // Deve ser chamada constantemente para alimentar a biblioteca
    void processar() {
      while (serialGPS.available() > 0) {
        gps.encode(serialGPS.read());
      }
    }

    void atualizarDados(DadosTelemetria &d) {
      processar();

      if (gps.location.isValid()) {
        d.latitude = gps.location.lat();
        d.longitude = gps.location.lng();
        d.gpsValido = true;
      } else {
        d.latitude = 0.0;
        d.longitude = 0.0;
        d.gpsValido = false;
      }

      if (gps.speed.isValid()) {
        d.velocidadeKmh = gps.speed.kmph();
      } else {
        d.velocidadeKmh = 0.0;
      }
    }
};

#endif