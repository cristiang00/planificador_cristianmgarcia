/**
 * @file
 * @brief Generacion del diagrama de Gantt con gnuplot.
 * @author Erwin Meza Vega <emezav@unicauca.edu.co>
 * @copyright MIT License
 */


#ifndef GRAFICA_H
#define GRAFICA_H

#include <string>
#include <vector>

#include "proceso.h"

/**
 * @brief Escribe el guion de gnuplot que dibuja el diagrama de Gantt.
 *
 * Cada proceso ocupa una fila. El tiempo de CPU se traza con linea gruesa y el
 * de espera con linea delgada, de modo que la altura de la fila muestra cuando
 * el proceso tuvo el procesador y cuando estuvo esperando.
 *
 * @param ruta_guion Archivo de guion a escribir, con extension .gpi
 * @param procesos Procesos ya simulados
 * @return true si el guion se pudo escribir
 */
bool escribir_guion(const std::string &ruta_guion, const std::vector<Proceso> &procesos);

/**
 * @brief Genera la imagen del diagrama de Gantt.
 *
 * Escribe el guion y ejecuta gnuplot sobre el. Si gnuplot no esta instalado, el
 * guion queda escrito y se informa por la salida de error.
 *
 * @param ruta_base Ruta sin extension. Produce ruta_base.gpi y ruta_base.png
 * @param procesos Procesos ya simulados
 * @return true si la imagen se genero
 */
bool generar_grafica(const std::string &ruta_base, const std::vector<Proceso> &procesos);

#endif
