# Planificación de procesos

En un sistema operativo multitarea, los procesos aparentan ser ejecutados en forma simultánea. En realidad, las CPU disponibles deben ser compartidas entre una gran cantidad de procesos, que a su vez ejecutan diversas instrucciones durante un determinado intervalo de tiempo antes de ser desalojados para dar paso a otros.

El problema consiste en simular la tarea de planificación de procesos en un sistema operativo. Para ello, deberá implementar las primitivas necesarias para permitir la planificación de procesos utilizando colas de prioridad.
Entrada del programa
Los datos del simulador se leerán por entrada estándar, y consistirán en una serie de comandos (DEFINE, PROCESS, START), que indican la definición de un parámetro de simulación, la llegada de un nuevo proceso o el inicio de la tarea de planificación, respectivamente. Una vez que se lea el comando START (es decir que ha terminado la lectura de los datos), se deberá comenzar la simulación.

## Requerimientos

- Una versión reciente de GCC en Windows o Linux.
- Se requiere Doxygen para generar la documentación.
- Se require GnuPlot para generar las gráficas de planificación.

## Comando DEFINE

El comando DEFINE establece  un parámetro para la simulación. Los únicos parámetros que se pueden definir son el número de colas de prioridad a utilizar, la estrategia de planificación (FIFO, SJF, RR) para cada cola y el quantum para cada cola.

## Ejemplo del comando DEFINE

    #Utilizar tres colas de prioridad
    DEFINE queues 3

    #Definir la estrategia  para cada cola
    DEFINE scheduling 1 RR
    DEFINE scheduling 2 RR
    DEFINE scheduling 3 FIFO

    #Definir quantum (prioridad) para cada #cola

    DEFINE quantum 1 5
    DEFINE quantum 2 4
    DEFINE quantum 3 2

## Comando PROCESS

El comando PROCESS define la llegada de un proceso. Sus parámetros son el tiempo de llegada del proceso, el tiempo de ejecución y la prioridad (1 ..  n) a la cual pertenece.

    PROCESS process1 0 5 1
    PROCESS process2 0 3 1
    PROCESS process3 0 4 1

## Comando START

El comando START define el fin de la lectura de los datos y el inicio de la simulación. No especifica ningún parámetro.	START

    START

# Ejemplos de archivos de configuración

En el subdirectorio test se encuentran varios archivos de configuración que simulan una sola cola de prioridad con diferentes algoritmos de planificación.

\see test/fifo.txt
\see test/rr.txt
\see test/sjf.txt
\see test/srt.txt


# Salida del programa

La salida de su programa deberá ser una tabla similar a la que se muestra a continuación.


    Resultados de la simulación
    Colas de prioridad: 3
    Tiempo total de la simulación: # unidades de tiempo
    Tiempo promedio de espera: # unidades de tiempo

    # Proceso             T. Llegada    Tamaño     T. Espera    T. Finalización
    ----------------------------------------------------------------------------------------------
    1 process1               10                  30                 #                   #
    2 process2               0                    21                 #                   #
    3 process3               7                    25                 #                   #

    Secuencia de ejecución
    -- presentar la secuencia de ejecución en formato libre, por ejemplo –


Los datos presentados en la tabla son:

    •	#: Numero del proceso, se asigna en forma secuencial de acuerdo con el orden de llegada (PROCESS) de los procesos.
    •	Proceso: Nombre del proceso (nombre del archivo en el cual se encuentra el código del proceso)
    •	T. Llegada: Tiempo de llegada del proceso. Este parámetro es el tiempo especificado en el comando BEGIN dentro del código de alto nivel del proceso.
    •	Tamaño: Tamaño en unidades de tiempo del proceso. Se puede calcular como la suma de todas las instrucciones CPU que se encuentran en el código del programa.
    •	T. Espera: Tiempo total de espera para el proceso.
    •	T. Finalizacion: Unidad de tiempo en la cual el proceso finaliza la ejecución.

Un ejemplo de secuencia de ejecución para los datos de ejemplo es la siguiente:

    2(4) – 2(4) – 3(2) – 1(5) -   ….

El primer proceso en ejecutarse es el proceso 2 (con prioridad 2), ya que en el tiempo 0 la cola de prioridad 1 está vacía. Este proceso alcanza a ejecutar dos tajadas de tiempo de su prioridad (para la prioridad 2, la tajada es 4), Ya que hasta t=7 las demás colas de prioridad están vacías. En  t=8, el proceso 3 ya ha llegado, por lo cual se le da la CPU durante 2 unidades de tiempo. En t=10, le corresponde a la cola de prioridad 1, y se ejecuta el proceso 1 (que coincidencialmente llega en t=10), y así sucesivamente.

Observe también que, si una cola de prioridad se planifica con Round Robin, el proceso ingresará al final de la cola. Por el contrario, si la cola de prioridad se planifica con algún algoritmo sin expropiación como FIFO, no se podrá ejecutar ningún otro proceso hasta que el proceso que se haya bloqueado retome su ejecución y termine. Es decir, el proceso volverá al inicio de la cola de prioridad correspondiente.


# Código a implementar

Se deberá completar la implementación de la subrutina schedule (sched.c).
El algoritmo a implementar el el siguiente:


    Preparar para una nueva simulacion
    Inicializar las colas de prioridad con la informacion de la lista de los procesos leidos
    Usar prepare()

    Crear una nueva secuencia de ejecucion para guardar la lista asignacion de la cpu

    n = total de procesos a simular (tamaño de la lista de procesos)

    tiempo_actual = MINIMO TIEMPO DE LLEGADA DE TODOS LOS PROCESOS
    Usar get_next_arrival()

    Procesar las llegadas en el tiempo minimo
    Los procesos se pasan de la cola de llegadas (arrival) a la cola de listos (ready)
    Usar process_arrival()

    POST: Al menos una de las colas de prioridad tiene procesos en estado de listo

    Buscar la cola que tenga al menos un proceso en estado de listo
    Comenzando por la 0 (la de mayor prioridad)

    proceso_actual = NULO

    mientras no haya procesos por simular:

        Obtener el primer proceso listo en la cola correspondiente

        Quitar el proceso de la cola de listos

        Pasar el proceso a ejecucion

        Suponer que al proceso se le puede asignar todo el quantum

        Si el tiempo restante del proceso es menor al tiempo que se puede asignar
        Tomar solo el tiempo restante

        Validar SRT:
        Verificar si llega un proceso a la cola SRT en este quantum y tiene menor tiempo restante
        Solo dar el tiempo de CPU desde el tiempo actual hsata el tiempo de llegada del nuevo proceso

        Actualizar el tiempo de uso de CPU del proceso

        Disminuir el tiempo restante en el tiempo de CPU dado

        Si se dio CPU al proceso:
        Aumentar el tiempo de espera de los demas procesos
        Usar add_waiting_time()
        Adicionar un nuevo elemento a la secuencia de ejecucion
        usar create_slice() y adicionar a la lista de secuencias de ejecucion

        Verificar si el proceso ha finalizado
        Si el proceso finalizo:
            Levar a la lista finished de su cola de prioridad
            Restar uno al total de procesos que falta por simular
            SRT: Si el proceso finaliza justo cuando llega el otro, pasar a la siguiente cola.
            En caso contrario, no se debe cambiar de cola de prioridad!
        En caso contrario (el proceso no finalizo):
            Pasar a estado de listo
            Enviar a la cola de listos de su prioridad, de acuerdo con el algoritmo de esa cola.
        Fin si


        Avanzar el tiempo a la cantidad de CPU asignada.

        Procesar las llegadas de nuevos procesos al tiempo actual

        Terminar si ya no existen procesos por planificar


        Avanzar a la siguiente cola de prioridad
        SRT: Si el proceso que se expropio de la CPU no uso todo el quantum
        se debe asignar el tiempo restante del quantum al proceso que llego
        no se cambia de cola de prioridad!
        En cualquier otro caso, se pasa a la siguiente cola de prioridad.

        Si no existen procesos en estado de listos en ninguna cola, avanzar hasta la siguiente llegada (en cualquier cola)
    fin mientras


