#ifndef SECUENCIA_H
#define SECUENCIA_H

#include <string>
#include <vector>

class Secuencia {
private:
    std::string descripcion;
    std::vector<char> bases;
    int ancho_linea;

public:
    Secuencia(std::string desc = "", std::vector<char> b = {}, int ancho = 0);

    std::string getDescripcion() const;
    int getAnchoLinea() const;

    int longitud(bool incluirGuiones = true) const;
    int esSubsecuencia(const std::string& sub) const;
    int enmascarar(const std::string& sub);

    const std::vector<char>& getBases() const;
    void setBases(const std::vector<char>& nuevasBases);
};

#endif

