#ifndef ANALISADOR_SINISTRO_H
#define ANALISADOR_SINISTRO_H

#include "DadosTelemetria.h"
#include <math.h>

class AnalisadorSinistro {
  private:
    // Limites (Thresholds)
    const float LIMITE_ANGULO_QUEDA_ALERTA = 45.0;
    const float LIMITE_ANGULO_QUEDA_CONFIRMAR = 65.0;
    const float LIMITE_ANGULO_RECUPERAR = 25.0;
    const float LIMITE_IMPACTO_G = 2.5;
    const float LIMITE_GIRO_PARADO = 0.7;

    const unsigned long TEMPO_CONFIRMAR_QUEDA = 3000;
    const unsigned long TEMPO_CONFIRMAR_MAXIMO = 6000;
    const unsigned long TEMPO_RECUPERACAO = 800;

    // Variáveis de Estado
    EstadoMoto estadoAtual;
    unsigned long tempoInicioQueda;
    unsigned long tempoInicioRecuperacao;
    bool impactoDuranteAlerta;

  public:
    AnalisadorSinistro();
    
    // O tempoAtual (millis) é injetado, facilitando testes automatizados
    EstadoMoto verificar(DadosTelemetria dados, unsigned long tempoAtual);
    EstadoMoto getEstadoAtual();
};

#endif