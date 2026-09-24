/**
 * @file
 * @brief Simulacion de la planificacion de procesos con colas de prioridad.
 * @author Erwin Meza Vega <emezav@unicauca.edu.co>
 * @copyright MIT License
 */


#ifndef PLANIFICADOR_H
#define PLANIFICADOR_H

#include <deque>
#include <string>
#include <vector>

#include "proceso.h"

/** @brief Cola de prioridad: su algoritmo, su quantum y sus procesos. */
struct Cola_prioridad {
    Estrategia estrategia = Estrategia::RR; /*!< Algoritmo de esta cola */
    int quantum = 0;                        /*!< Quantum que reparte */
    std::deque<Proceso *> llegada;          /*!< Aun no han llegado, ordenados por llegada */
    std::deque<Proceso *> listos;           /*!< En espera de CPU */
    std::deque<Proceso *> terminados;       /*!< Ya finalizaron */
};

/** @brief Resultado de una simulacion. */
struct Resultado {
    std::vector<Tramo_secuencia> secuencia; /*!< Orden en que se repartio la CPU */
    int espera_total = 0;                   /*!< Suma de los tiempos de espera */
    double espera_promedio = 0.0;           /*!< Espera media por proceso */
    int tiempo_total = 0;                   /*!< Instante en que termina el ultimo proceso */
};

/**
 * @brief Convierte el nombre de un algoritmo a su valor.
 * @param texto Nombre del algoritmo, sin distinguir mayusculas
 * @param destino Recibe el valor convertido
 * @return true si el nombre corresponde a un algoritmo conocido
 */
bool estrategia_desde_texto(const std::string &texto, Estrategia &destino);

/**
 * @brief Nombre legible de un algoritmo.
 * @param estrategia Algoritmo
 * @return Su nombre en mayusculas
 */
std::string texto_de_estrategia(Estrategia estrategia);

/**
 * @brief Deja las colas listas para una simulacion nueva.
 *
 * Reinicia cada proceso y lo coloca en la cola de llegada que le corresponde
 * por su prioridad, ordenado por instante de llegada.
 *
 * @param procesos Procesos leidos de la entrada
 * @param colas Colas de prioridad
 */
void preparar(std::vector<Proceso> &procesos, std::vector<Cola_prioridad> &colas);

/**
 * @brief Instante de la proxima llegada en cualquier cola.
 * @param colas Colas de prioridad
 * @return Ese instante, o -1 si ya no queda ninguna llegada pendiente
 */
int proxima_llegada(const std::vector<Cola_prioridad> &colas);

/**
 * @brief Pasa a listos los procesos que ya llegaron.
 * @param ahora Instante actual
 * @param colas Colas de prioridad
 * @return Cantidad de procesos que cambiaron a listo
 */
int procesar_llegadas(int ahora, std::vector<Cola_prioridad> &colas);

/**
 * @brief Ejecuta la simulacion completa.
 * @param procesos Procesos a planificar
 * @param colas Colas de prioridad ya configuradas
 * @param traza Si es true, describe cada decision por la salida de error
 * @return Secuencia de ejecucion y tiempos resultantes
 */
Resultado planificar(std::vector<Proceso> &procesos, std::vector<Cola_prioridad> &colas,
                     bool traza = false);

/**
 * @brief Imprime la tabla de resultados por la salida estandar.
 * @param procesos Procesos ya simulados
 * @param resultado Resultado de la simulacion
 */
void imprimir_resultados(const std::vector<Proceso> &procesos, const Resultado &resultado);

#endif
