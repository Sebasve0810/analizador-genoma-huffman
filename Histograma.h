#ifndef HISTOGRAMA_H
#define HISTOGRAMA_H

#include "Secuencia.h"
#include <vector>
#include <string>

class Histograma {
private:
    std::vector<char> simbolos;
    std::vector<int> frecuencias;

public:
    Histograma();
    void construir(const Secuencia& s);
    void mostrar() const;
};

#endif

