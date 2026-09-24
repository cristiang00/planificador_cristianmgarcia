/**
 * @file
 * @brief Tipos basicos de la simulacion de planificacion.
 * @author Erwin Meza Vega <emezav@unicauca.edu.co>
 * @copyright MIT License
 */


#ifndef PROCESO_H
#define PROCESO_H

#include <string>
#include <vector>

/** @brief Algoritmo de planificacion de una cola de prioridad. */
enum class Estrategia { FIFO, SJF, RR, SRT };

/** @brief Estado de un proceso dentro de la simulacion. */
enum class Estado { cargado, listo, ejecucion, terminado };

/** @brief Uso que un proceso hace del tiempo: CPU o espera. */
enum class Tipo_quantum { cpu, espera };

/** @brief Intervalo de tiempo atribuido a un proceso. */
struct Quantum {
    Tipo_quantum tipo;  /*!< CPU o espera */
    int desde;        /*!< Instante inicial, incluido */
    int hasta;        /*!< Instante final, excluido */
};

/** @brief Proceso de la simulacion. */
struct Proceso {
    int pid = 0;                    /*!< Identificador asignado al leer */
    std::string nombre;             /*!< Nombre del proceso */
    int prioridad = 0;              /*!< Cola a la que pertenece, contada desde 0 */
    int llegada = 0;                /*!< Instante en que llega al sistema */
    int ejecucion = 0;              /*!< Tiempo total de CPU que necesita */
    int restante = 0;               /*!< Tiempo de CPU que le falta */
    int espera = 0;                 /*!< Tiempo acumulado en la cola de listos */
    int finalizacion = 0;           /*!< Instante en que termina */
    int cpu = 0;                    /*!< Tiempo de CPU ya recibido */
    Estado estado = Estado::cargado; /*!< Estado actual */
    std::vector<Quantum> quantums;    /*!< Linea de tiempo, para la grafica */

    /**
     * @brief Devuelve el proceso a su estado inicial.
     */
    void reiniciar() {
        restante = ejecucion;
        espera = 0;
        finalizacion = 0;
        cpu = 0;
        estado = Estado::cargado;
        quantums.clear();
    }

    /**
     * @brief Tiempo de retorno del proceso.
     * @return Unidades de tiempo entre su llegada y su finalizacion
     */
    int retorno() const { return finalizacion - llegada; }
};

/** @brief Un tramo de la secuencia de ejecucion: quien tuvo la CPU y cuanto. */
struct Tramo_secuencia {
    std::string nombre; /*!< Proceso que recibio la CPU */
    int tiempo;         /*!< Unidades de tiempo asignadas */
};

#endif
