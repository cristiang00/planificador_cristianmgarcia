/**
 * @file
 * @brief Simulador de planificacion de procesos.
 * @author Erwin Meza Vega <emezav@unicauca.edu.co>
 * @copyright MIT License
 *
 * Lee la configuracion de la simulacion por un archivo o por la entrada
 * estandar, ejecuta la planificacion y produce la tabla de resultados y el
 * diagrama de Gantt.
 */
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "grafica.h"
#include "planificador.h"

namespace {

/**
 * @brief Quita el comentario que empieza en almohadilla y el blanco de los bordes.
 * @param linea Linea leida de la configuracion
 * @return La linea sin comentario y sin blanco en los extremos
 */
std::string limpiar(const std::string &linea) {
    std::string s = linea.substr(0, linea.find('#'));
    const std::size_t ini = s.find_first_not_of(" \t\r\n");
    if (ini == std::string::npos) {
        return "";
    }
    const std::size_t fin = s.find_last_not_of(" \t\r\n");
    return s.substr(ini, fin - ini + 1);
}

/**
 * @brief Pasa una cadena a minusculas.
 * @param s Cadena a convertir
 * @return La cadena con todas sus letras en minuscula
 */
std::string minusculas(std::string s) {
    for (char &c : s) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return s;
}

/**
 * @brief Asegura que existan al menos n colas de prioridad.
 * @param colas Colas de prioridad, que se amplian si hace falta
 * @param n Cantidad minima de colas que deben existir
 */
void asegurar_colas(std::vector<Cola_prioridad> &colas, std::size_t n) {
    if (colas.size() < n) {
        colas.resize(n);
    }
}

/**
 * @brief Lee la configuracion de la simulacion.
 * @param entrada Flujo de entrada
 * @param procesos Recibe los procesos leidos
 * @param colas Recibe las colas de prioridad configuradas
 * @return true si se leyo una configuracion utilizable
 */
bool leer_configuracion(std::istream &entrada, std::vector<Proceso> &procesos,
                        std::vector<Cola_prioridad> &colas) {
    std::string linea;
    int numero_linea = 0;
    int pid = 1;

    while (std::getline(entrada, linea)) {
        numero_linea++;
        const std::string util = limpiar(linea);
        if (util.empty()) {
            continue;
        }

        std::istringstream campos(util);
        std::string orden;
        campos >> orden;
        orden = minusculas(orden);

        if (orden == "define") {
            std::string parametro;
            campos >> parametro;
            parametro = minusculas(parametro);

            if (parametro == "queues") {
                int n = 0;
                if (!(campos >> n) || n <= 0) {
                    std::cerr << "Linea " << numero_linea << ": numero de colas no valido.\n";
                    return false;
                }
                asegurar_colas(colas, static_cast<std::size_t>(n));
            } else if (parametro == "scheduling") {
                int cual = 0;
                std::string nombre;
                if (!(campos >> cual >> nombre) || cual <= 0) {
                    std::cerr << "Linea " << numero_linea << ": falta la cola o el algoritmo.\n";
                    return false;
                }
                asegurar_colas(colas, static_cast<std::size_t>(cual));
                Estrategia e;
                if (!estrategia_desde_texto(nombre, e)) {
                    std::cerr << "Linea " << numero_linea << ": algoritmo desconocido " << nombre
                              << ". Se admiten FIFO, SJF, RR y SRT.\n";
                    return false;
                }
                colas[cual - 1].estrategia = e;
            } else if (parametro == "quantum") {
                int cual = 0;
                int valor = 0;
                if (!(campos >> cual >> valor) || cual <= 0 || valor <= 0) {
                    std::cerr << "Linea " << numero_linea << ": quantum no valido.\n";
                    return false;
                }
                asegurar_colas(colas, static_cast<std::size_t>(cual));
                colas[cual - 1].quantum = valor;
            } else {
                std::cerr << "Linea " << numero_linea << ": parametro desconocido " << parametro
                          << ".\n";
                return false;
            }
        } else if (orden == "process") {
            Proceso p;
            int prioridad = 1;
            if (!(campos >> p.nombre >> p.llegada >> p.ejecucion)) {
                std::cerr << "Linea " << numero_linea
                          << ": se esperaba PROCESS nombre llegada rafaga [prioridad].\n";
                return false;
            }
            if (!(campos >> prioridad) || prioridad <= 0) {
                prioridad = 1;
            }
            if (p.ejecucion <= 0) {
                std::cerr << "Linea " << numero_linea << ": la rafaga debe ser mayor que cero.\n";
                return false;
            }
            asegurar_colas(colas, static_cast<std::size_t>(prioridad));
            p.pid = pid++;
            p.prioridad = prioridad - 1;
            p.restante = p.ejecucion;
            procesos.push_back(p);
        } else if (orden == "start") {
            break;
        } else {
            std::cerr << "Linea " << numero_linea << ": orden desconocida " << orden
                      << ". Se admiten DEFINE, PROCESS y START.\n";
            return false;
        }
    }

    if (procesos.empty()) {
        std::cerr << "No se definio ningun proceso.\n";
        return false;
    }
    if (colas.empty()) {
        asegurar_colas(colas, 1);
    }
    for (Cola_prioridad &c : colas) {
        if (c.quantum <= 0) {
            c.quantum = 1;
        }
    }
    return true;
}

}  // namespace

/**
 * @brief Punto de entrada del simulador.
 * @param argc Cantidad de argumentos de linea de comandos
 * @param argv Argumentos de linea de comandos
 * @return Cero si la simulacion se realizo, distinto de cero si hubo error
 */
int main(int argc, char *argv[]) {
    std::vector<Proceso> procesos;
    std::vector<Cola_prioridad> colas;
    std::string ruta_base = "gantt";
    bool traza = false;

    std::vector<std::string> args(argv + 1, argv + argc);
    std::string archivo;
    for (const std::string &a : args) {
        if (a == "-t" || a == "--traza") {
            traza = true;
        } else {
            archivo = a;
        }
    }

    bool leido = false;
    if (!archivo.empty()) {
        std::ifstream entrada(archivo);
        if (!entrada) {
            std::cerr << "No se pudo abrir " << archivo << "\n";
            return 1;
        }
        leido = leer_configuracion(entrada, procesos, colas);
        // El guion y la imagen se nombran a partir del archivo de entrada, con
        // los separadores en barra inclinada, que las dos plataformas aceptan.
        std::filesystem::path ruta_archivo(archivo);
        ruta_base = ruta_archivo.replace_extension("").string();
        std::replace(ruta_base.begin(), ruta_base.end(), '\\', '/');
    } else {
        leido = leer_configuracion(std::cin, procesos, colas);
    }

    if (!leido) {
        return 1;
    }

    std::cout << "Colas de prioridad: " << colas.size() << "\n";
    for (std::size_t i = 0; i < colas.size(); ++i) {
        std::cout << "  Cola " << (i + 1) << ": " << texto_de_estrategia(colas[i].estrategia)
                  << ", quantum " << colas[i].quantum << "\n";
    }

    const Resultado resultado = planificar(procesos, colas, traza);
    imprimir_resultados(procesos, resultado);
    generar_grafica(ruta_base, procesos);

    return 0;
}
