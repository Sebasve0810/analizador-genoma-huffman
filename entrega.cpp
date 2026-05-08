#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <cctype>
#include "Secuencia.h"
#include "Histograma.h"
#include "Genoma.h"

using namespace std;

// Estructura para almacenar información de comandos
struct Comando {
    string nombre;
    string descripcion;
    string uso;
};

// Lista de comandos disponibles
vector<Comando> comandos = {
    {"ayuda", "Lista todos los comandos o muestra ayuda para un comando específico", "ayuda [comando]"},
    {"cargar_secuencia", "Carga secuencias desde un archivo FASTA", "cargar_secuencia <nombre_archivo>"},
    {"listar_secuencia", "Lista una secuencia genética", "listar_secuencia"},
    {"histograma", "Muestra el histograma de una secuencia", "histograma <descripción_secuencia>"},
    {"es_subsecuencia", "Verifica si la subsecuencia del usuario ya existe", "es_subsecuencia <subsecuencia>"},
    {"enmascarar", "Oculta una subsecuencia", "enmascarar <subsecuencia>"},
    {"guardar", "Guarda las secuencias cargadas en memoria", "guardar <nombre_archivo>"},
    {"codificar_secuencia", "Codifica una secuencia usando Huffman", "codificar_secuencia <descripción>"},
    {"decodificar_secuencia", "Decodifica una secuencia desde un archivo", "decodificar_secuencia <archivo>"},
    {"base_remota", "Busca una base en una posición y calcula el costo de la ruta", "base_remota <descripción> <x> <y>"},
    {"salir", "Termina el programa", "salir"}
};

// Función para dividir la entrada en tokens
vector<string> dividir(const string& entrada) {
    vector<string> tokens;
    stringstream ss(entrada);
    string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// Función para mostrar todos los comandos
void mostrar_ayuda() {
    cout << "Comandos disponibles:\n";
    for (const auto& cmd : comandos) {
        cout << "  " << cmd.nombre << ": " << cmd.descripcion << "\n";
    }
}

// Función para mostrar ayuda de un comando específico
void mostrar_ayuda_comando(const string& nombre_cmd) {
    for (const auto& cmd : comandos) {
        if (cmd.nombre == nombre_cmd) {
            cout << "Comando: " << cmd.nombre << "\n";
            cout << "Descripción: " << cmd.descripcion << "\n";
            cout << "Uso: " << cmd.uso << "\n";
            return;
        }
    }
    cout << "Error: El comando '" << nombre_cmd << "' no existe.\n";
}

// Función para validar una secuencia genética (permitir bases extendidas)
bool es_secuencia_valida(const string& secuencia) {
    string validos = "ACGTURYKMSWBDHVNX-";
    for (char c : secuencia) {
        if (validos.find(toupper(c)) == string::npos) {
            return false;
        }
    }
    return true;
}

int main() {
    vector<Secuencia> secuencias;
    Genoma genoma; // Objeto para manejar la carga de archivos FASTA
    string entrada;
    cout << "$ ";
    while (getline(cin, entrada)) {
        vector<string> tokens = dividir(entrada);
        
        if (tokens.empty()) {
            cout << "Error: Comando vacío.\n";
            cout << "$ ";
            continue;
        }

        string comando = tokens[0];
        // Convertir a minúsculas para comparación insensible a mayúsculas
        transform(comando.begin(), comando.end(), comando.begin(), ::tolower);

        if (comando == "salir") {
            cout << "Saliendo del programa.\n";
            break;
        }
        else if (comando == "ayuda") {
            if (tokens.size() == 1) {
                mostrar_ayuda();
            } else if (tokens.size() == 2) {
                mostrar_ayuda_comando(tokens[1]);
            } else {
                cout << "Error: Parámetros inválidos. Uso: ayuda [comando]\n";
            }
        }
        else if (comando == "cargar_secuencia") {
            if (tokens.size() == 2) {
                string archivo = tokens[1];
                // Verificar si el archivo tiene extensión .fa o .fasta
                string extension = archivo.substr(archivo.find_last_of(".") + 1);
                if (extension != "fa" && extension != "fasta") {
                    cout << "Error: El archivo debe tener extensión .fa o .fasta\n";
                } else if (genoma.cargar(archivo)) {
                    const vector<Secuencia>& nuevas_secuencias = genoma.getSecuencias();
                    secuencias.insert(secuencias.end(), nuevas_secuencias.begin(), nuevas_secuencias.end());
                    // No imprimimos aquí, dejamos que Genoma::cargar maneje el mensaje
                } else {
                    cout << "Error: No se pudo cargar el archivo '" << archivo << "'. Asegúrate de que el archivo existe y es legible.\n";
                }
            } else {
                cout << "Error: Parámetros inválidos. Uso: cargar_secuencia <nombre_archivo>\n";
            }
        }
        else if (comando == "listar_secuencia") {
            if (tokens.size() == 1) {
                if (secuencias.empty()) {
                    cout << "No hay secuencias cargadas en memoria.\n";
                } else {
                    cout << "Hay " << secuencias.size() << " secuencias cargadas en memoria:\n";
                    for (const auto& s : secuencias) {
                        string desc = s.getDescripcion();
                        // Eliminar el ">" inicial si está presente
                        if (!desc.empty() && desc[0] == '>') {
                            desc = desc.substr(1);
                        }
                        bool incompleta = (find(s.getBases().begin(), s.getBases().end(), '-') != s.getBases().end());
                        cout << "Secuencia " << desc << " contiene ";
                        if (incompleta) {
                            cout << "al menos " << s.longitud(false) << " bases.\n";
                        } else {
                            cout << s.longitud(true) << " bases.\n";
                        }
                    }
                }
            } else {
                cout << "Error: Parámetros inválidos. Uso: listar_secuencia\n";
            }
        }
        else if (comando == "histograma") {
            if (tokens.size() == 2) {
                string desc = tokens[1];
                // Normalizar la descripción: eliminar ">" si está presente
                if (!desc.empty() && desc[0] == '>') desc = desc.substr(1);
                auto it = find_if(secuencias.begin(), secuencias.end(), [&](const Secuencia& s) {
                    string desc_s = s.getDescripcion();
                    // Normalizar la descripción almacenada
                    if (!desc_s.empty() && desc_s[0] == '>') desc_s = desc_s.substr(1);
                    return desc_s == desc;
                });
                if (it != secuencias.end()) {
                    Histograma h;
                    h.construir(*it);
                    h.mostrar();
                } else {
                    cout << "Error: No se encontró la secuencia con descripción '" << desc << "'. Usa 'listar_secuencia' para ver las descripciones disponibles.\n";
                }
            } else {
                cout << "Error: Parámetros inválidos. Uso: histograma <descripción_secuencia>\n";
            }
        }
        else if (comando == "es_subsecuencia") {
            if (tokens.size() == 2) {
                string sub = tokens[1];
                int total = 0;
                for (const auto& s : secuencias) {
                    total += s.esSubsecuencia(sub);
                }
                if (total == 0) {
                    cout << "La subsecuencia dada no existe dentro de las secuencias cargadas en memoria.\n";
                } else {
                    cout << "La subsecuencia dada se repite " << total << " veces dentro de las secuencias cargadas en memoria.\n";
                }
            } else {
                cout << "Error: Parámetros inválidos. Uso: es_subsecuencia <subsecuencia>\n";
            }
        }
        else if (comando == "enmascarar") {
            if (tokens.size() == 2) {
                string sub = tokens[1];
                int total = 0;
                for (auto& s : secuencias) {
                    total += s.enmascarar(sub);
                }
                if (total == 0) {
                    cout << "La subsecuencia dada no existe dentro de las secuencias cargadas en memoria, por tanto no se enmascara nada.\n";
                } else {
                    cout << total << " subsecuencias han sido enmascaradas dentro de las secuencias cargadas en memoria.\n";
                }
            } else {
                cout << "Error: Parámetros inválidos. Uso: enmascarar <subsecuencia>\n";
            }
        }
        else if (comando == "guardar") {
            if (tokens.size() == 2) {
                string archivo = tokens[1];
                ofstream out(archivo);
                if (!out.is_open()) {
                    cout << "Error guardando en " << archivo << ".\n";
                } else {
                    for (const auto& s : secuencias) {
                        out << ">" << s.getDescripcion() << "\n";
                        const auto& b = s.getBases();
                        for (size_t i = 0; i < b.size(); ++i) {
                            out << b[i];
                            if ((i + 1) % s.getAnchoLinea() == 0) {
                                out << "\n";
                            }
                        }
                        if (b.size() % s.getAnchoLinea() != 0) {
                            out << "\n";
                        }
                    }
                    cout << "Las secuencias han sido guardadas en " << archivo << ".\n";
                }
            } else {
                cout << "Error: Parámetros inválidos. Uso: guardar <nombre_archivo>\n";
            }
        }
        else if (comando == "codificar_secuencia") {
            if (tokens.size() == 2) {
                cout << "Codificando secuencia '" << tokens[1] << "'.\n";
            } else {
                cout << "Error: Parámetros inválidos. Uso: codificar_secuencia <descripción>\n";
            }
        }
        else if (comando == "decodificar_secuencia") {
            if (tokens.size() == 2) {
                cout << "Decodificando secuencia desde el archivo '" << tokens[1] << "'.\n";
            } else {
                cout << "Error: Parámetros inválidos. Uso: decodificar_secuencia <archivo>\n";
            }
        }
        else if (comando == "base_remota") {
            if (tokens.size() == 4) {
                string descripcion = tokens[1];
                string x = tokens[2];
                string y = tokens[3];
                try {
                    stoi(x);
                    stoi(y);
                    cout << "Procesando base_remota para la secuencia '" << descripcion << "' en la posición (" << x << ", " << y << ").\n";
                } catch (...) {
                    cout << "Error: x e y deben ser enteros.\n";
                }
            } else {
                cout << "Error: Parámetros inválidos. Uso: base_remota <descripción> <x> <y>\n";
            }
        }
        else {
            cout << "Error: Comando desconocido '" << comando << "'. Escribe 'ayuda' para ver los comandos disponibles.\n";
        }
        cout << "$ ";
    }
    return 0;
}
