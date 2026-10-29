#ifndef SENSOR_IMU_H
#define SENSOR_IMU_H

#include <Adafruit_MPU6050.h>
#include <Wire.h>
#include "DadosTelemetria.h"

class SensorIMU {
  private:
    Adafruit_MPU6050 mpu;
    float baseAx = 0, baseAy = 0, baseAz = 0;
    float baseAnguloLateral = 0, baseAnguloFrontal = 0;
    float axFiltrado = 0, ayFiltrado = 0, azFiltrado = 0;
    float gxFiltrado = 0, gyFiltrado = 0, gzFiltrado = 0;
    bool filtroInicializado = false;
    
    const float ALPHA_ACEL = 0.20;
    const float ALPHA_GIRO = 0.20;
    const float RAD_PARA_GRAUS = 57.2957795;
    const float GRAVIDADE = 9.80665;

    float calcularAnguloLateral(float ax, float ay, float az) {
      float den = sqrt((ay * ay) + (az * az));
      return atan2(ax, den < 0.01 ? 0.01 : den) * RAD_PARA_GRAUS;
    }

    float calcularAnguloFrontal(float ax, float ay, float az) {
      float den = sqrt((ax * ax) + (az * az));
      return atan2(ay, den < 0.01 ? 0.01 : den) * RAD_PARA_GRAUS;
    }

  public:
    bool iniciar(uint8_t endereco) {
      if (!mpu.begin(endereco)) return false;
      mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
      mpu.setGyroRange(MPU6050_RANGE_500_DEG);
      mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
      return true;
    }

    void calibrar() {
      const int AMOSTRAS = 200;
      float somaAx = 0, somaAy = 0, somaAz = 0;
      sensors_event_t a, g, temp;

      for (int i = 0; i < AMOSTRAS; i++) {
        mpu.getEvent(&a, &g, &temp);
        somaAx += a.acceleration.x;
        somaAy += a.acceleration.y;
        somaAz += a.acceleration.z;
        delay(10);
      }

      baseAx = somaAx / AMOSTRAS;
      baseAy = somaAy / AMOSTRAS;
      baseAz = somaAz / AMOSTRAS;

      baseAnguloLateral = calcularAnguloLateral(baseAx, baseAy, baseAz);
      baseAnguloFrontal = calcularAnguloFrontal(baseAx, baseAy, baseAz);

      axFiltrado = baseAx;
      ayFiltrado = baseAy;
      azFiltrado = baseAz;
      filtroInicializado = true;
    }

    DadosTelemetria lerDados() {
      sensors_event_t a, g, temp;
      mpu.getEvent(&a, &g, &temp);

      if (!filtroInicializado) {
        axFiltrado = a.acceleration.x; ayFiltrado = a.acceleration.y; azFiltrado = a.acceleration.z;
        gxFiltrado = g.gyro.x; gyFiltrado = g.gyro.y; gzFiltrado = g.gyro.z;
        filtroInicializado = true;
      } else {
        axFiltrado = ALPHA_ACEL * a.acceleration.x + (1.0 - ALPHA_ACEL) * axFiltrado;
        ayFiltrado = ALPHA_ACEL * a.acceleration.y + (1.0 - ALPHA_ACEL) * ayFiltrado;
        azFiltrado = ALPHA_ACEL * a.acceleration.z + (1.0 - ALPHA_ACEL) * azFiltrado;

        gxFiltrado = ALPHA_GIRO * g.gyro.x + (1.0 - ALPHA_GIRO) * gxFiltrado;
        gyFiltrado = ALPHA_GIRO * g.gyro.y + (1.0 - ALPHA_GIRO) * gyFiltrado;
        gzFiltrado = ALPHA_GIRO * g.gyro.z + (1.0 - ALPHA_GIRO) * gzFiltrado;
      }

      DadosTelemetria d;
      d.ax = a.acceleration.x; d.ay = a.acceleration.y; d.az = a.acceleration.z;
      d.gx = g.gyro.x; d.gy = g.gyro.y; d.gz = g.gyro.z;

      float acelTotal = sqrt((d.ax * d.ax) + (d.ay * d.ay) + (d.az * d.az));
      d.gTotal = acelTotal / GRAVIDADE;
      d.giroTotal = sqrt((gxFiltrado * gxFiltrado) + (gyFiltrado * gyFiltrado) + (gzFiltrado * gzFiltrado));

      d.anguloLateral = calcularAnguloLateral(axFiltrado, ayFiltrado, azFiltrado) - baseAnguloLateral;
      d.anguloFrontal = calcularAnguloFrontal(axFiltrado, ayFiltrado, azFiltrado) - baseAnguloFrontal;
      d.movimentoFrontal = ayFiltrado - baseAy;

      if (d.anguloLateral > 25.0) d.lateralTexto = "Esquerda forte";
      else if (d.anguloLateral > 10.0) d.lateralTexto = "Esquerda";
      else if (d.anguloLateral < -25.0) d.lateralTexto = "Direita forte";
      else if (d.anguloLateral < -10.0) d.lateralTexto = "Direita";
      else d.lateralTexto = "Reto";

      if (d.movimentoFrontal > 1.2) d.frontalTexto = "Acelerando";
      else if (d.movimentoFrontal < -1.2) d.frontalTexto = "Freando";
      else d.frontalTexto = "Estavel";

      return d;
    }
};

#endif