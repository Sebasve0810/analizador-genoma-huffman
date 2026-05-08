# Analizador de Genomas con Compresión Huffman

Herramienta de línea de comandos en **C++** para análisis, procesamiento y compresión de secuencias genómicas en formato **FASTA**. Implementa el algoritmo de **codificación Huffman** desde cero para comprimir y descomprimir archivos genómicos a un formato binario propio (`.fabin`).

![C++](https://img.shields.io/badge/C++-17-00599C?style=flat&logo=cplusplus&logoColor=white)
![Bioinformatics](https://img.shields.io/badge/Bioinformatics-FASTA-2ea44f?style=flat)
![Algorithm](https://img.shields.io/badge/Algorithm-Huffman-orange?style=flat)

---

## Descripción

El sistema permite cargar archivos FASTA con secuencias de ADN reales, realizar análisis sobre ellas (búsqueda, enmascaramiento, histograma de bases) y comprimirlas usando codificación Huffman en un formato binario personalizado. Probado con el genoma completo de levadura (*Saccharomyces cerevisiae*).

---

## Funcionalidades

| Comando | Descripción |
|---|---|
| `cargar_secuencia ` | Carga secuencias desde un archivo FASTA |
| `listar_secuencia` | Lista todas las secuencias cargadas en memoria |
| `histograma ` | Muestra frecuencia de cada base (A, C, G, T) |
| `es_subsecuencia ` | Verifica si una cadena existe como subsecuencia |
| `enmascarar ` | Enmascara todas las ocurrencias de una subsecuencia |
| `guardar ` | Guarda las secuencias procesadas en formato FASTA |
| `codificar_secuencia ` | Comprime una secuencia a formato `.fabin` con Huffman |
| `decodificar_secuencia ` | Decodifica un archivo `.fabin` de vuelta a FASTA |
| `base_remota   ` | Busca una base en posición y calcula costo de ruta |
| `ayuda [comando]` | Muestra ayuda general o de un comando específico |

---

## Arquitectura

```
├── src/
│   ├── entrega.cpp       # CLI principal — punto de entrada e interpretador de comandos
│   ├── Genoma.h/.cpp     # Carga, consulta y gestión de archivos FASTA
│   ├── Secuencia.h/.cpp  # Modelado de una secuencia genómica individual
│   ├── Histograma.h/.cpp # Análisis de frecuencia de bases nitrogenadas
│   └── Huffman.h/.cpp    # Codificación y decodificación Huffman (.fabin)
└── tests/
    ├── dnaExample.fa         # Ejemplo básico de ADN
    ├── multiSequence.fa      # Múltiples secuencias en un archivo
    ├── multisizeSequences.fa # Secuencias de distintos tamaños
    └── test_genome.fa        # Genoma de prueba
```

---

## Formato de compresión `.fabin`

El algoritmo de Huffman genera un formato binario propio en little-endian:

```
n         (uint16)          → cantidad de símbolos distintos
(ci, fi)  (uint8, uint64)*n → par (carácter, frecuencia global)
ns        (uint32)          → número de secuencias
Por cada secuencia:
  li      (uint16)          → longitud del nombre
  nombre  (li bytes)        → nombre de la secuencia
  wi      (uint64)          → longitud de la secuencia
  xi      (uint16)          → ancho de línea original
  bits    (padded a 8)      → secuencia codificada en Huffman
```

---

## Compilación y ejecución

### Prerrequisitos

- Compilador C++17 o superior (`g++` o `clang++`)
- Linux / macOS / Windows (WSL)

### Compilar

```bash
git clone https://github.com/Sebasve0810/analizador-genoma-huffman.git
cd analizador-genoma-huffman
g++ -std=c++17 -O2 src/*.cpp -o genoma
```

### Ejecutar

```bash
./genoma
```

### Ejemplo de uso

```
> cargar_secuencia tests/dnaExample.fa
> listar_secuencia
> histograma seq1
> es_subsecuencia ATCG
> codificar_secuencia seq1
> decodificar_secuencia seq1.fabin
> salir
```

---

## Datos de prueba

El proyecto incluye archivos FASTA reales para testing, incluyendo secuencias del genoma de *Saccharomyces cerevisiae* (levadura de panadería) — uno de los organismos modelo más estudiados en biología molecular.

---

## Autor

**Sebastián Velasquez**

Systems Engineering @ Pontificia Universidad Javeriana

[![LinkedIn](https://img.shields.io/badge/LinkedIn-0077B5?style=flat&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/sebastian-velasquez-73662721a)
[![GitHub](https://img.shields.io/badge/GitHub-100000?style=flat&logo=github&logoColor=white)](https://github.com/Sebasve0810)

---

*Proyecto académico — Análisis y Diseño de Algoritmos | Ingeniería de Sistemas, Javeriana 2025*
