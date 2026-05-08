
#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

/**
 * Componente 2 - Codificación y Decodificación Huffman para archivos FASTA (.fabin)
 * 
 * Este módulo NO depende de las clases de tu Componente 1.
 * Provee un "puente" (SecuenciaData) para integrarse con tu Genoma/Secuencia.
 * 
 *  Formato del .fabin implementado (little-endian), según enunciado:
 *   n (uint16)                       -> cantidad de símbolos distintos
 *   (ci:uint8, fi:uint64) * n       -> par (código, frecuencia global)
 *   ns (uint32)                      -> número de secuencias
 *   Repetido para cada secuencia i:
 *     li (uint16)                    -> tamaño del nombre
 *     sij (li bytes)                 -> nombre
 *     wi (uint64)                    -> longitud de la secuencia
 *     xi (uint16)                    -> ancho de línea (justificación)
 *     binary_code (bits, padded a 8) -> codificación Huffman de la secuencia
 * 
 *  Decodificación: se reconstruye el árbol con las frecuencias globales y se leen
 *  bits hasta obtener 'wi' símbolos; luego se alinea al siguiente byte.
 */

struct SecuenciaData {
    std::string descripcion;   // nombre de la secuencia
    std::string bases;         // cadena con las bases (A,C,G,T,U,..., '-', 'X', etc.)
    uint16_t ancho_linea = 0;  // justificación (col width) original del FASTA
};

namespace huffman {

/**
 * Codifica las secuencias en memoria y escribe nombre_archivo.fabin
 * Devuelve true en éxito; en caso contrario, false y 'error' con descripción.
 */
bool codificar(const std::vector<SecuenciaData>& secuencias,
               const std::string& nombre_archivo,
               std::string& error);

/**
 * Decodifica nombre_archivo.fabin y carga las secuencias en 'out'.
 * Devuelve true en éxito; en caso contrario, false y 'error' con descripción.
 */
bool decodificar(const std::string& nombre_archivo,
                 std::vector<SecuenciaData>& out,
                 std::string& error);

} // namespace huffman

#endif // HUFFMAN_H
