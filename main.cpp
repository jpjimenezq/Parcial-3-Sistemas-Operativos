#include "imagen.h"
#include "buddy_allocator.h"
#include <iostream>
#include <chrono>
#include <cstring>
#include <cstdlib>
#include <sys/resource.h>
#include <iomanip>
#include <omp.h>
#include <thread>

using namespace std;
using namespace std::chrono;

void mostrarUso() {
    cout << "Uso: ./programa_imagen <entrada> <salida> [-angulo <grados>] [-escalar <factor>] [-buddy|-no-buddy]\n";
}

size_t getMemoryUsage() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss;
}

void procesarArgumentos(int argc, char* argv[], bool& usarBuddy, float& angulo, bool& rotar, bool& escalar, float& factorEscala) {
    usarBuddy = false;
    angulo = 0.0f;
    factorEscala = 1.0f;
    rotar = false;
    escalar = false;

    for (int i = 3; i < argc; i++) {
        if (strcmp(argv[i], "-angulo") == 0 && i + 1 < argc) {
            angulo = atof(argv[++i]);
            rotar = true;
        } else if (strcmp(argv[i], "-escalar") == 0 && i + 1 < argc) {
            factorEscala = atof(argv[++i]);
            escalar = true;
        } else if (strcmp(argv[i], "-buddy") == 0) {
            usarBuddy = true;
        } else if (strcmp(argv[i], "-no-buddy") == 0) {
            usarBuddy = false;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        mostrarUso();
        return 1;
    }

    string archivoEntrada = argv[1];
    string archivoSalida = argv[2];
    bool usarBuddy = false;
    float angulo = 0.0f;
    bool rotar = false;
    bool escalar = false;
    float factorEscala = 1.0f;

    procesarArgumentos(argc, argv, usarBuddy, angulo, rotar, escalar, factorEscala);

    cout << "=== PROCESAMIENTO DE IMAGEN ===" << endl;
    cout << "Archivo de entrada: " << archivoEntrada << endl;
    cout << "Archivo de salida: " << archivoSalida << endl;
    cout << "Modo de asignación de memoria: " << (usarBuddy ? "Buddy System" : "new/delete") << endl;
    cout << "-------------------------------" << endl;

    // Medición de tiempo para procesamiento normal
    auto startNormal = high_resolution_clock::now();
    long tiempoNormal = 0;
    size_t memNormal = 0;

    {
        size_t memInicio = getMemoryUsage();

        Imagen imgNormal(archivoEntrada);
        if (rotar) imgNormal.rotar(angulo);
        if (escalar) imgNormal.escalar(factorEscala);

        tiempoNormal = duration_cast<milliseconds>(high_resolution_clock::now() - startNormal).count();
        memNormal = getMemoryUsage() - memInicio;

        imgNormal.mostrarInfo();
        if (rotar) cout << "Ángulo de rotación: " << angulo << " grados\n";
        if (escalar) cout << "Factor de escalado: " << factorEscala << endl;
        cout << "-------------------------------" << endl;
        imgNormal.guardarImagen("new_" + archivoSalida);
    }

    // Medición de tiempo para procesamiento con Buddy System
    long tiempoBuddy = 0;
    size_t memBuddy = 0;

    if (usarBuddy) {
        auto startBuddy = high_resolution_clock::now();
        
        size_t memInicio = getMemoryUsage();

        BuddyAllocator allocador(4 * 1024 * 1024);
        Imagen img(archivoEntrada, &allocador);
        if (rotar) img.rotar(angulo);
        if (escalar) img.escalar(factorEscala);

        tiempoBuddy = duration_cast<milliseconds>(high_resolution_clock::now() - startBuddy).count();
        memBuddy = getMemoryUsage() - memInicio;

        img.mostrarInfo();
        if (rotar) cout << "Ángulo de rotación: " << angulo << " grados\n";
        if (escalar) cout << "Factor de escalado: " << factorEscala << endl;
        cout << "-------------------------------" << endl;
        img.guardarImagen("buddy_" + archivoSalida);
    }

    cout << "\n=== RESULTADOS ===\n";
    cout << "TIEMPO DE PROCESAMIENTO:" << endl;
    cout << " - Sin Buddy System: " << tiempoNormal << " ms" << endl;
    if (usarBuddy) cout << " - Con Buddy System: " << tiempoBuddy << " ms" << endl;

    cout << "\nMEMORIA UTILIZADA:" << endl;
    cout << fixed << setprecision(2);
    cout << " - Sin Buddy System: " << memNormal / 1024.0 << " MB" << endl;
    if (usarBuddy) cout << " - Con Buddy System: " << memBuddy / 1024.0 << " MB" << endl;

    cout << "\n[INFO] Imagen guardada correctamente como ";
    if (usarBuddy) cout << "'buddy_" << archivoSalida << "' y 'new_" << archivoSalida << "'" << endl;
    else cout << "'new_" << archivoSalida << "'" << endl;

    return 0;
}
