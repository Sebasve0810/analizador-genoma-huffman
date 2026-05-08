
#include "Huffman.h"
#include <queue>
#include <fstream>
#include <stdexcept>
#include <algorithm>

namespace {

struct Node {
    char ch;
    uint64_t freq;
    Node* left;
    Node* right;
    Node(char c, uint64_t f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
    Node(Node* l, Node* r) : ch('\0'), freq(l->freq + r->freq), left(l), right(r) {}
};

struct Cmp {
    bool operator()(const Node* a, const Node* b) const {
        // min-heap por frecuencia; si empata, por carácter para determinismo
        if (a->freq == b->freq) return (unsigned char)a->ch > (unsigned char)b->ch;
        return a->freq > b->freq;
    }
};

void freeTree(Node* n) {
    if (!n) return;
    freeTree(n->left);
    freeTree(n->right);
    delete n;
}

void buildCodes(Node* n, const std::string& pref,
                std::unordered_map<char, std::string>& codes) {
    if (!n) return;
    if (!n->left && !n->right) {
        // hoja
        codes[n->ch] = pref.empty() ? std::string("0") : pref; // caso 1 símbolo
        return;
    }
    buildCodes(n->left,  pref + "0", codes);
    buildCodes(n->right, pref + "1", codes);
}

Node* buildTree(const std::unordered_map<char, uint64_t>& freq) {
    std::priority_queue<Node*, std::vector<Node*>, Cmp> pq;
    for (auto& kv : freq) {
        pq.push(new Node(kv.first, kv.second));
    }
    if (pq.empty()) return nullptr;
    if (pq.size() == 1) { // árbol degenerado: 1 solo símbolo
        Node* only = pq.top(); pq.pop();
        // crear un padre artificial para que haya camino
        Node* dummy = new Node(new Node(only->ch, only->freq), new Node('\0', 0));
        return dummy;
    }
    while (pq.size() > 1) {
        Node* a = pq.top(); pq.pop();
        Node* b = pq.top(); pq.pop();
        pq.push(new Node(a, b));
    }
    return pq.top();
}

// helpers de IO en little-endian fijos
template <typename T>
void writeLE(std::ofstream& os, T value) {
    for (size_t i = 0; i < sizeof(T); ++i) {
        os.put(static_cast<char>((value >> (8 * i)) & 0xFF));
    }
}

template <typename T>
bool readLE(std::ifstream& is, T& value) {
    value = 0;
    for (size_t i = 0; i < sizeof(T); ++i) {
        int c = is.get();
        if (c == EOF) return false;
        value |= (static_cast<T>((unsigned char)c) << (8 * i));
    }
    return true;
}

struct BitWriter {
    std::ofstream& os;
    uint8_t current = 0;
    int bits = 0; // en current
    BitWriter(std::ofstream& o) : os(o) {}
    void writeBit(int b) {
        current |= (uint8_t(b & 1) << (7 - bits));
        bits++;
        if (bits == 8) {
            os.put(static_cast<char>(current));
            current = 0;
            bits = 0;
        }
    }
    void writeCode(const std::string& code) {
        for (char c : code) writeBit(c == '1' ? 1 : 0);
    }
    void flushZeroPad() {
        if (bits > 0) {
            // relleno con ceros
            os.put(static_cast<char>(current));
            current = 0;
            bits = 0;
        }
    }
    int bitRemainder() const { return bits; }
};

struct BitReader {
    std::ifstream& is;
    uint8_t current = 0;
    int bitsLeft = 0; // bits restantes por consumir del byte actual
    BitReader(std::ifstream& in) : is(in) {}
    bool readBit(int& out) {
        if (bitsLeft == 0) {
            int c = is.get();
            if (c == EOF) return false;
            current = static_cast<uint8_t>(c);
            bitsLeft = 8;
        }
        out = (current >> (7 - (8 - bitsLeft))) & 1;
        bitsLeft--;
        return true;
    }
    void alignToByte() {
        bitsLeft = 0;
    }
};

} // namespace


namespace huffman {

bool codificar(const std::vector<SecuenciaData>& secuencias,
               const std::string& nombre_archivo,
               std::string& error) {
    try {
        if (secuencias.empty()) {
            error = "No hay secuencias cargadas en memoria.";
            return false;
        }

        // 1) Frecuencias globales
        std::unordered_map<char, uint64_t> freq;
        for (const auto& s : secuencias) {
            for (char c : s.bases) {
                unsigned char uc = static_cast<unsigned char>(c);
                // Permitimos cualquier símbolo presente.
                freq[(char)uc]++;
            }
        }
        if (freq.empty()) {
            error = "No hay símbolos para codificar (secuencias vacías).";
            return false;
        }

        // 2) Construir árbol y códigos
        Node* root = buildTree(freq);
        if (!root) {
            error = "No fue posible construir el árbol de Huffman.";
            return false;
        }
        std::unordered_map<char, std::string> codes;
        buildCodes(root, "", codes);

        // 3) Abrir archivo destino
        std::ofstream os(nombre_archivo, std::ios::binary);
        if (!os) {
            freeTree(root);
            error = "No se pueden guardar las secuencias en '" + nombre_archivo + "'.";
            return false;
        }

        // 4) Encabezado: n, (ci,fi)*n
        // Orden determinista por carácter
        std::vector<std::pair<char, uint64_t>> entries(freq.begin(), freq.end());
        std::sort(entries.begin(), entries.end(),
                  [](auto& a, auto& b){ return (unsigned char)a.first < (unsigned char)b.first; });

        uint16_t n = static_cast<uint16_t>(entries.size());
        writeLE<uint16_t>(os, n);
        for (auto& e : entries) {
            uint8_t ci = static_cast<uint8_t>(e.first);
            uint64_t fi = e.second;
            os.put(static_cast<char>(ci));
            writeLE<uint64_t>(os, fi);
        }

        // 5) ns
        uint32_t ns = static_cast<uint32_t>(secuencias.size());
        writeLE<uint32_t>(os, ns);

        // 6) Para cada secuencia: li, nombre, wi, xi, bits
        for (const auto& s : secuencias) {
            uint16_t li = static_cast<uint16_t>(s.descripcion.size());
            writeLE<uint16_t>(os, li);
            os.write(s.descripcion.data(), li);

            uint64_t wi = static_cast<uint64_t>(s.bases.size());
            writeLE<uint64_t>(os, wi);

            uint16_t xi = s.ancho_linea;
            writeLE<uint16_t>(os, xi);

            BitWriter bw(os);
            for (char c : s.bases) {
                auto it = codes.find(c);
                if (it == codes.end()) {
                    freeTree(root);
                    error = "Símbolo no encontrado en tabla de códigos durante codificación.";
                    return false;
                }
                bw.writeCode(it->second);
            }
            bw.flushZeroPad();
        }

        os.flush();
        os.close();
        freeTree(root);
        return true;
    } catch (const std::exception& ex) {
        error = std::string("Excepción en codificar: ") + ex.what();
        return false;
    } catch (...) {
        error = "Error desconocido en codificar.";
        return false;
    }
}

bool decodificar(const std::string& nombre_archivo,
                 std::vector<SecuenciaData>& out,
                 std::string& error) {
    try {
        std::ifstream is(nombre_archivo, std::ios::binary);
        if (!is) {
            error = "No se pueden cargar las secuencias desde '" + nombre_archivo + "'.";
            return false;
        }

        // 1) Leer n y tabla (ci, fi)
        uint16_t n = 0;
        if (!readLE<uint16_t>(is, n)) {
            error = "Archivo .fabin corrupto (no se pudo leer 'n').";
            return false;
        }
        if (n == 0) {
            error = "Archivo .fabin inválido (n=0).";
            return false;
        }
        std::unordered_map<char, uint64_t> freq;
        for (uint16_t i = 0; i < n; ++i) {
            int ci_int = is.get();
            if (ci_int == EOF) { error = "Tabla de símbolos incompleta."; return false; }
            char ci = static_cast<char>(static_cast<uint8_t>(ci_int));
            uint64_t fi = 0;
            if (!readLE<uint64_t>(is, fi)) { error = "Tabla de frecuencias incompleta."; return false; }
            freq[ci] = fi;
        }

        // 2) Reconstruir árbol
        Node* root = buildTree(freq);
        if (!root) {
            error = "No fue posible reconstruir el árbol de Huffman.";
            return false;
        }

        // 3) ns
        uint32_t ns = 0;
        if (!readLE<uint32_t>(is, ns)) {
            freeTree(root);
            error = "Archivo .fabin corrupto (no se pudo leer 'ns').";
            return false;
        }

        out.clear();
        out.reserve(ns);

        BitReader br(is);

        // 4) Por secuencia: li, nombre, wi, xi, bits
        for (uint32_t k = 0; k < ns; ++k) {
            uint16_t li = 0;
            if (!readLE<uint16_t>(is, li)) { freeTree(root); error = "No se pudo leer 'li'."; return false; }
            std::string nombre(li, '\0');
            if (!is.read(&nombre[0], li)) { freeTree(root); error = "No se pudo leer el nombre de secuencia."; return false; }

            uint64_t wi = 0;
            if (!readLE<uint64_t>(is, wi)) { freeTree(root); error = "No se pudo leer 'wi'."; return false; }
            uint16_t xi = 0;
            if (!readLE<uint16_t>(is, xi)) { freeTree(root); error = "No se pudo leer 'xi'."; return false; }

            // Decodificar wi símbolos
            std::string bases;
            bases.reserve((size_t)wi);

            Node* cur = root;
            while (bases.size() < wi) {
                // si hoja (árbol degenerado de 1 símbolo)
                if (!cur->left && !cur->right && cur->ch != '\0') {
                    bases.push_back(cur->ch);
                    cur = root;
                    continue;
                }
                int b;
                if (!br.readBit(b)) { freeTree(root); error = "Bits insuficientes al decodificar."; return false; }
                cur = (b == 0) ? cur->left : cur->right;
                if (!cur) { freeTree(root); error = "Ruta en árbol inválida durante decodificación."; return false; }
                if (!cur->left && !cur->right) {
                    bases.push_back(cur->ch);
                    cur = root;
                }
            }
            // Alinearse al próximo byte (se escribió con padding)
            br.alignToByte();

            out.push_back(SecuenciaData{nombre, bases, xi});
        }

        freeTree(root);
        return true;
    } catch (const std::exception& ex) {
        error = std::string("Excepción en decodificar: ") + ex.what();
        return false;
    } catch (...) {
        error = "Error desconocido en decodificar.";
        return false;
    }
}

} // namespace huffman
