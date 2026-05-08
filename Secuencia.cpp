#include "Secuencia.h"
#include <algorithm>

using namespace std;

Secuencia::Secuencia(string desc, vector<char> b, int ancho)
    : descripcion(desc), bases(b), ancho_linea(ancho) {}

string Secuencia::getDescripcion() const { return descripcion; }
int Secuencia::getAnchoLinea() const { return ancho_linea; }

int Secuencia::longitud(bool incluirGuiones) const {
    if (incluirGuiones) return bases.size();
    return count_if(bases.begin(), bases.end(), [](char c){ return c != '-'; });
}

int Secuencia::esSubsecuencia(const string& sub) const {
    if (sub.empty()) return 0;
    int count = 0;
    for (size_t i = 0; i + sub.size() <= bases.size(); i++) {
        bool match = true;
        for (size_t j = 0; j < sub.size(); j++) {
            if (bases[i+j] != sub[j]) { match = false; break; }
        }
        if (match) count++;
    }
    return count;
}

int Secuencia::enmascarar(const std::string& sub) {
    if (sub.empty()) return 0;
    int count = 0;
    for (size_t i = 0; i + sub.size() <= bases.size(); i++) {
        bool match = true;
        for (size_t j = 0; j < sub.size(); j++) {
            if (bases[i+j] != sub[j]) { match = false; break; }
        }
        if (match) {
            for (size_t j = 0; j < sub.size(); j++) bases[i+j] = 'X';
            count++;
        }
    }
    return count;
}

const std::vector<char>& Secuencia::getBases() const { return bases; }
void Secuencia::setBases(const std::vector<char>& nuevasBases) { bases = nuevasBases; }

