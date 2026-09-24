# Proyecto: simulador de planificación de procesos

En un sistema operativo multitarea los procesos aparentan ejecutarse de forma simultánea, cuando en
realidad las CPU disponibles se asignan por ráfagas entre todos ellos. Este proyecto consiste en
completar un simulador del mecanismo que usa el sistema operativo para determinar cuál proceso
recibe la CPU. Toma como entrada un conjunto de procesos con su instante de llegada y su ráfaga,
aplica un algoritmo de planificación y produce la secuencia de ejecución, la tabla de tiempos y el
diagrama de Gantt correspondiente.

**El simulador ya funciona y se puede ejecutar desde el primer momento.** La lectura de la
configuración, la contabilidad de los tiempos, la impresión de resultados y la generación del
diagrama están resueltas, de modo que el trabajo se concentra en la planificación y no en el
andamiaje que la rodea. El algoritmo `FIFO` está implementado y sirve de ejemplo.

## Punto de partida

```bash
make
./planificador test/caso_1_fifo.txt
```

En la máquina virtual, `sudo apt install build-essential gnuplot` instala lo necesario. Sin
`gnuplot` el simulador produce igual todos los resultados numéricos, de modo que lo único que no
genera es la imagen.

El nombre del directorio se cambia por los logins institucionales de quienes lo resuelven, según la
forma `planificador_login1_login2`.

## Puntos por implementar

Los puntos por resolver están marcados en `src/planificador.cpp` con la etiqueta `\todo`, y son
seis, agrupados en cuatro decisiones. Ninguno pasa de unas pocas líneas, dado que lo que se evalúa
es haber entendido qué distingue a un algoritmo de otro y no la cantidad de código.

**1. El proceso que llega a una cola.** En `procesar_llegadas`, un proceso que llega entra al final
de la cola de listos. Eso es correcto para `FIFO` y para `RR`, pero en `SJF` y en `SRT` el orden lo
decide la ráfaga.

**2. La inserción ordenada.** La función `insertar_por_restante` recibe un proceso y una cola, y debe
dejarla ordenada de menor a mayor tiempo restante. Cuando dos procesos empatan, el que ya estaba
conserva su lugar, de modo que el desempate lo decide el orden de llegada.

**3. El proceso que no ha terminado.** Cuando un proceso agota su turno sin terminar, vuelve a la
cola de listos, y **el sitio al que vuelve es lo que define cada algoritmo**: en `RR` va al final,
en `SJF` conserva la CPU porque no expropia, y en `SRT` regresa ordenado por su tiempo restante.

**4. La expropiación de `SRT`.** Es la más exigente de las cuatro. Si durante el turno del proceso
que se ejecuta llega otro cuya ráfaga es menor que el tiempo que le resta al primero, el que está en
la CPU solo la conserva hasta ese instante.

Mientras los cuatro puntos no estén resueltos, los cuatro algoritmos producen el mismo resultado que
`FIFO`, y esa coincidencia es la primera señal de que aún falta trabajo.

El enunciado del proyecto desarrolla en pseudocódigo el mecanismo completo de la simulación, dentro
del cual encajan los cuatro puntos.

## Comprobación de resultados

Cada algoritmo tiene un resultado conocido sobre el conjunto de procesos del taller —P1 con ráfaga 7
desde el instante 0, P2 con 4 desde el 2, P3 con 1 desde el 4 y P4 con 4 desde el 5—:

| Algoritmo | Espera promedio |
| --- | --- |
| `FIFO` | 4.750 |
| `SJF` | 4.000 |
| `RR` con quantum 2 | 5.000 |
| `SRT` con quantum 2 | 3.000 |

Esos archivos están en `test/`, con los nombres `taller_fifo.txt`, `taller_sjf.txt`, `taller_rr.txt`
y `taller_srt.txt`. Una implementación correcta reproduce las cuatro cifras.

Los cinco archivos que empiezan por `caso_1_` comparten un conjunto de procesos mayor y solo cambian
de algoritmo, de modo que sirven para comparar los resultados entre sí.

Además del promedio conviene revisar la **secuencia de ejecución**, porque dos planificaciones
distintas pueden dar el mismo promedio. El diagrama de Gantt muestra esa secuencia, en la cual el
bloque verde es el tiempo de CPU y la barra gris el tiempo de espera.

## Colas de prioridad

El simulador admite varias colas, cada una con su propio algoritmo y su propio quantum, y las
recorre de forma circular concediendo a cada una un turno. Los archivos de prueba del taller usan
una sola cola, mientras que `test/caso_1_prioridades.txt` usa varias.

La segunda parte del proyecto consiste en resolver un conjunto de procesos repartido en tres colas
con algoritmos distintos, y explicar por qué el reparto entre colas cambia el resultado frente a
planificar los mismos procesos en una sola.

## Configuración de la simulación

La configuración se escribe con tres órdenes. Las líneas vacías se ignoran, y lo que sigue a una
almohadilla es un comentario.

```text
DEFINE queues 1            # cuantas colas de prioridad
DEFINE scheduling 1 RR     # algoritmo de la cola 1: FIFO, SJF, RR o SRT
DEFINE quantum 1 2         # quantum de la cola 1

PROCESS P1 0 7 1           # nombre, llegada, rafaga, cola
PROCESS P2 2 4 1

START
```

La opción `-t` describe por la salida de error qué decisión se toma en cada instante, lo que permite
seguir el comportamiento de un algoritmo paso a paso:

```bash
./planificador -t test/taller_srt.txt
```

## Condiciones de entrega

- El código completo, compilando con `make` sin advertencias.
- Los resultados de los cuatro algoritmos sobre los casos del taller, con sus diagramas.
- Una bitácora que explique, para cada algoritmo, qué línea de código lo distingue de los demás y
  por qué su resultado es el que es.

## Organización del código

| Archivo | Contenido | Se modifica |
| --- | --- | --- |
| `src/planificador.cpp` | El algoritmo de planificación | **Sí** |
| `src/proceso.h` | Los tipos básicos | No |
| `src/planificador.h` | La interfaz de la simulación | No |
| `src/grafica.cpp` | El diagrama de Gantt | No |
| `src/main.cpp` | La lectura de la configuración | No |

El código sigue las convenciones de *Programming: Principles and Practice Using C++*. Los tipos
llevan inicial mayúscula y los nombres compuestos se separan con guion bajo, mientras que las
funciones y las variables van enteramente en minúscula.
