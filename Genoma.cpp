#include "Genoma.h"
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;

bool Genoma::cargar(const string& archivo) {
    ifstream in(archivo);
    if (!in.is_open()) {
        cout << archivo << " no se encuentra o no puede leerse.\n";
        return false;
    }

    secuencias.clear();
    nombre_archivo = archivo;

    string line, descripcion;
    vector<char> bases;
    int ancho = 0;

    while (getline(in, line)) {
        if (line.empty()) continue;

        if (line[0] == '>') {
            // Guardar la secuencia anterior antes de iniciar una nueva
            if (!descripcion.empty()) {
                secuencias.emplace_back(descripcion, bases, ancho);
                bases.clear();
                ancho = 0;
            }
            descripcion = line.substr(1); // quitar el '>'
        } else {
            if (ancho == 0) ancho = line.size();
            bases.insert(bases.end(), line.begin(), line.end());
        }
    }

    // Guardar la última secuencia
    if (!descripcion.empty())
        secuencias.emplace_back(descripcion, bases, ancho);

    if (secuencias.empty()) {
        cout << archivo << " no contiene ninguna secuencia.\n";
        return false;
    }

    if (secuencias.size() == 1)
        cout << "1 secuencia cargada correctamente desde " << archivo << ".\n";
    else
        cout << secuencias.size() << " secuencias cargadas correctamente desde " << archivo << ".\n";

    return true;
}

void Genoma::listarSecuencias() const {
    if (secuencias.empty()) {
        cout << "No hay secuencias cargadas en memoria.\n";
        return;
    }
    cout << "Hay " << secuencias.size() << " secuencias cargadas en memoria:\n";
    for (const auto& s : secuencias) {
        bool incompleta = (find(s.getBases().begin(), s.getBases().end(), '-') != s.getBases().end());
        if (incompleta) {
            cout << "Secuencia " << s.getDescripcion()
                      << " contiene al menos " << s.longitud(false) << " bases.\n";
        } else {
            cout << "Secuencia " << s.getDescripcion()
                      << " contiene " << s.longitud(true) << " bases.\n";
        }
    }
}

int Genoma::buscarSubsecuencia(const string& sub) const {
    if (secuencias.empty()) {
        cout << "No hay secuencias cargadas en memoria.\n";
        return 0;
    }
    int total = 0;
    for (const auto& s : secuencias) total += s.esSubsecuencia(sub);

    if (total == 0)
        cout << "La subsecuencia dada no existe dentro de las secuencias cargadas en memoria.\n";
    else
        cout << "La subsecuencia dada se repite " << total << " veces dentro de las secuencias cargadas en memoria.\n";

    return total;
}

int Genoma::enmascarar(const string& sub) {
    if (secuencias.empty()) {
        cout << "No hay secuencias cargadas en memoria.\n";
        return 0;
    }
    int total = 0;
    for (auto& s : secuencias) total += s.enmascarar(sub);

    if (total == 0)
        cout << "La subsecuencia dada no existe dentro de las secuencias cargadas en memoria, por tanto no se enmascara nada.\n";
    else
        cout << total << " subsecuencias han sido enmascaradas dentro de las secuencias cargadas en memoria.\n";

    return total;
}

bool Genoma::guardar(const string& archivo) const {
    if (secuencias.empty()) {
        cout << "No hay secuencias cargadas en memoria.\n";
        return false;
    }

    ofstream out(archivo);
    if (!out.is_open()) {
        cout << "Error guardando en " << archivo << ".\n";
        return false;
    }

    for (const auto& s : secuencias) {
        out << ">" << s.getDescripcion() << "\n";
        const auto& b = s.getBases();
        for (size_t i = 0; i < b.size(); i++) {
            out << b[i];
            if ((i+1) % s.getAnchoLinea() == 0) out << "\n";
        }
        if (b.size() % s.getAnchoLinea() != 0) out << "\n";
    }

    cout << "Las secuencias han sido guardadas en " << archivo << ".\n";
    return true;
}

const vector<Secuencia>& Genoma::getSecuencias() const { return secuencias; }

