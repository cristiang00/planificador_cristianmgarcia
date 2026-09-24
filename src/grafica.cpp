/**
 * @file
 * @brief Generacion del diagrama de Gantt con gnuplot.
 * @author Erwin Meza Vega <emezav@unicauca.edu.co>
 * @copyright MIT License
 */

#include "grafica.h"

#include <cstdlib>
#include <fstream>
#include <iostream>

bool escribir_guion(const std::string &ruta_guion, const std::vector<Proceso> &procesos) {
    std::ofstream salida(ruta_guion);
    if (!salida) {
        std::cerr << "No se pudo escribir el guion " << ruta_guion << "\n";
        return false;
    }

    int tiempo_maximo = 0;
    for (const Proceso &p : procesos) {
        tiempo_maximo = std::max(tiempo_maximo, p.finalizacion);
    }
    const int filas = static_cast<int>(procesos.size());

    std::string ruta_imagen = ruta_guion;
    const std::size_t punto = ruta_imagen.find_last_of('.');
    if (punto != std::string::npos) {
        ruta_imagen = ruta_imagen.substr(0, punto);
    }
    ruta_imagen += ".png";

    salida << "set terminal pngcairo size 1024,";
    salida << (200 + filas * 60) << " font \"sans,10\"\n";
    // La ruta va entre comillas simples. Dentro de comillas dobles gnuplot
    // interpreta las secuencias de escape, de modo que una ruta de Windows
    // como test\caso pierde el separador.
    salida << "set output '" << ruta_imagen << "'\n";
    salida << "set title \"Diagrama de Gantt\"\n";
    salida << "set xlabel \"Tiempo\"\n";
    salida << "set ylabel \"Proceso\"\n";
    salida << "set xrange [0:" << (tiempo_maximo + 1) << "]\n";
    salida << "set yrange [0:" << (filas + 1) << "]\n";
    salida << "set xtics 1\n";
    salida << "set grid xtics\n";
    salida << "unset key\n";

    // Una etiqueta por fila con el nombre del proceso
    salida << "set ytics (";
    for (int i = 0; i < filas; ++i) {
        if (i > 0) {
            salida << ", ";
        }
        salida << "\"" << procesos[i].nombre << "\" " << (filas - i);
    }
    salida << ")\n";

    // Cada quantum es un rectangulo: relleno para la CPU, borde para la espera
    int objeto = 1;
    for (int i = 0; i < filas; ++i) {
        const double y = static_cast<double>(filas - i);
        for (const Quantum &q : procesos[i].quantums) {
            const double alto = (q.tipo == Tipo_quantum::cpu) ? 0.30 : 0.08;
            const char *color = (q.tipo == Tipo_quantum::cpu) ? "#2e7d32" : "#bdbdbd";
            salida << "set object " << objeto++ << " rect from " << q.desde << "," << (y - alto)
                   << " to " << q.hasta << "," << (y + alto)
                   << " fc rgb \"" << color << "\" fs solid 1.0 border lc rgb \"#424242\"\n";
        }
    }

    // gnuplot necesita algo que dibujar para producir la imagen
    salida << "plot -1 notitle\n";
    return true;
}

bool generar_grafica(const std::string &ruta_base, const std::vector<Proceso> &procesos) {
    const std::string ruta_guion = ruta_base + ".gpi";
    if (!escribir_guion(ruta_guion, procesos)) {
        return false;
    }

    const std::string orden = "gnuplot \"" + ruta_guion + "\"";
    if (std::system(orden.c_str()) != 0) {
        std::cerr << "gnuplot no pudo ejecutarse. El guion quedo en " << ruta_guion
                  << "; para obtener la imagen se instala gnuplot y se ejecuta ese guion.\n";
        return false;
    }
    std::cout << "Diagrama de Gantt en " << ruta_base << ".png\n";
    return true;
}
