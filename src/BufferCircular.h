#pragma once
#include "DadosTelemetria.h"

class BufferCircular {
  private:
    static const int TAMANHO = 50; // 50 amostras a 5Hz = 10 segundos
    DadosTelemetria amostras[TAMANHO];
    int indiceAtual = 0;

  public:
    void adicionar(DadosTelemetria d) {
      amostras[indiceAtual] = d;
      indiceAtual = (indiceAtual + 1) % TAMANHO;
    }

    // Retorna os dados ordenados cronologicamente (do mais antigo para o impacto)
    DadosTelemetria obterAmostra(int i) {
      return amostras[(indiceAtual + i) % TAMANHO]; 
    }

    int getTamanho() { return TAMANHO; }
};