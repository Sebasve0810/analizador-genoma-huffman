#ifndef GENOMA_H
#define GENOMA_H

#include "Secuencia.h"
#include <vector>
#include <string>

class Genoma {
private:
    std::string nombre_archivo;
    std::vector<Secuencia> secuencias;

public:
    bool cargar(const std::string& archivo);       // cargar archivo FASTA
    void listarSecuencias() const;                 // listar todas las secuencias
    int buscarSubsecuencia(const std::string& sub) const; // buscar subsecuencia
    int enmascarar(const std::string& sub);        // enmascarar subsecuencia
    bool guardar(const std::string& archivo) const; // guardar archivo FASTA

    const std::vector<Secuencia>& getSecuencias() const;
};

#endif

