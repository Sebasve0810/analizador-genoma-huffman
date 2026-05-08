#include "Histograma.h"
#include <iostream>

using namespace std;

Histograma::Histograma() {
    simbolos = {'A','C','G','T','U','R','Y','K','M','S','W',
                'B','D','H','V','N','X','-'};
    frecuencias.resize(simbolos.size(), 0);
}

void Histograma::construir(const Secuencia& s) {
    fill(frecuencias.begin(), frecuencias.end(), 0);

    for (char b : s.getBases()) {
        for (size_t i = 0; i < simbolos.size(); i++) {
            if (b == simbolos[i]) {
                frecuencias[i]++;
                break;
            }
        }
    }
}

void Histograma::mostrar() const {
    for (size_t i = 0; i < simbolos.size(); i++) {
        cout << simbolos[i] << " : " << frecuencias[i] << "\n";
    }
}

