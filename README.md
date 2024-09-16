# Huffman Coding Project - proyecto_1_IC6600

## Grupo

- Ávila Ramírez Paublo Alexander (2022065584)
- Reyes Rodríguez Ricardo Andrés (2022101681)
- Zúñiga Campos Omar Jesús (2022019053)

Este proyecto implementa el algoritmo Huffman para comprimir y descomprimir archivos de texto en paralelo haciendo uso de `fork` y `threads` . Además, incluye el método serial normal y un web crawler para descargar los libros en texto plano para usar en la ejecucion del proyecto.

## Tabla de Contenidos

- [Introducción](#Introducción)
- [Instalación](#Instalación)
- [Compilación](#Compilación)
- [Ejecución](#Ejecución)
- [Referencias Bibliográficas](#referencias-bibliográficas)

## Introduccion

Algoritmo de Huffman para comprimir archivos de texto. En este proyecto, se presenta varias maneras de implementar este algoritmo:

- **Fork**: Procesado en paralelo usando la funcion `fork`.
- **Thread**: Proceso multi-hilo usando hilos con la biblioteca `pthread`.
- **Serial**: Implementación normal de forma serial. 
- **Web Crawler**: Web crawler diseñado para obtener los archivos para la ejecucion del programa.

1. **Fork**:

fork() es una llamada al sistema en Unix/Linux que permite crear nuevos procesos. Esta función divide el proceso en dos: el proceso padre y el proceso hijo. Ambos procesos continúan ejecutándose desde el punto donde se invocó fork(), pero con un espacio de direcciones independiente (Hu, 2023).

- Valores de retorno: fork() devuelve 0 en el proceso hijo y el PID del hijo en el proceso padre. Si falla, retorna -1 (Hu, 2023).
- Memoria: Aunque inicialmente comparten memoria (copy-on-write), cualquier cambio en uno de los procesos hace que las páginas modificadas se copien, asegurando independencia (Hu, 2023).
- Herencia: El proceso hijo hereda descriptores de archivos, variables de entorno y otros atributos del padre, pero no hereda ciertos identificadores de procesos ni bloqueos (Hu, 2023).
- Eficiencia: La creación de procesos con fork() es más costosa que con hilos, ya que los procesos no comparten el mismo espacio de memoria. Sin embargo, proporciona un aislamiento completo entre procesos, lo que es útil para seguridad y estabilidad (Hu, 2023).
- Ejemplo de uso: fork() se combina frecuentemente con exec() para ejecutar nuevos programas en el contexto del proceso hijo (Hu, 2023).

La función es fundamental para sistemas multitarea y paralelismo, pero su mal uso puede generar problemas como la saturación de recursos (fork bomb) (Hu, 2023).

2. **Thread**:

La biblioteca pthread permite la creación de hilos para implementar concurrencia y paralelismo en programas de C. Cada hilo ejecuta una función definida por el programador y puede recibir argumentos a través de un puntero (GeeksforGeeks, 2023c).

- Creación de hilos: Se utiliza pthread_create() para crear un hilo que ejecuta una función, y pthread_join() para esperar a que un hilo termine (GeeksforGeeks, 2023c).
    ```c
    pthread_create(&thread, NULL, print_message, NULL);
    pthread_join(thread, NULL);
    ```
- Sincronización: La biblioteca incluye mecanismos como mutexes, variables de condición y semáforos para manejar el acceso a recursos compartidos y evitar condiciones de carrera. Un mutex asegura que solo un hilo acceda a una sección crítica a la vez (GeeksforGeeks, 2023c).
    ```c
    pthread_mutex_lock(&mutex);
    // Sección crítica
    pthread_mutex_unlock(&mutex);
    ```
- Variables de condición: Permiten que un hilo espere hasta que una condición específica se cumpla, coordinando la ejecución entre hilos mediante pthread_cond_wait() y pthread_cond_signal() (GeeksforGeeks, 2023c).
- Otras herramientas: También soporta bloqueos de lectura-escritura (pthread_rwlock_t) y semáforos para controlar el acceso concurrente a recursos (GeeksforGeeks, 2023c).
- Finalización de hilos: Se utiliza pthread_exit() para terminar un hilo de forma segura, permitiendo la devolución de valores si es necesario (GeeksforGeeks, 2023c).

La biblioteca pthread es esencial para desarrollar aplicaciones concurrentes y paralelas en C, proporcionando las herramientas necesarias para la sincronización y gestión eficiente de hilos (GeeksforGeeks, 2023c).


## Instalación

1. **Instale Fedora Workstation 40**:
   En el caso de no contar con Fedora Workstation 40, debe dirigirse a la página oficial e instalar el sistema operativo que se encuentra en el siguiente enlace:

```bash
     https://fedoraproject.org/es/workstation/download/
```

En el link suminstrado en el anterior punto, puede encontrar la documentación oficial que le ayudará a realizar el proceso de instalación adecuadamente.

2. **Clone el repositorio**:
   ```bash
   git clone https://github.com/ch0so/proyecto_1_IC6600.git
   ```

3. **Instale las dependencias**
    ```bash
    sudo bash dependencies.sh
    ```

## Compilación

Una vez este dentro de la carpeta del proyecto en la terminal ejecute el siguiente comando:

```bash
    make
```

Esto va a compilar el proyecto completo para dejarlo listo para su ejecución.

## Ejecución

1. **Lista de opciones**:
    Ejecute el siguiente comando:
    ```bash
    ./project_1
    ```
    Esto va a desplegar las diferentes opciónes para ejecutar el programa

2. **Download books**:
    ```bash
    ./project_1 download_books
    ```
    Esto va a activar el webcrawler para descargar los archivos necesarios para probar la funcionalidad del algoritmo huffman con sus distintas implementaciones.

3. **Clear Directory Compressed**:
    ```bash
    ./project_1 clear_directory_compressed
    ```
    Esta opción se encargar de eliminar el archivo binario que sale despues de comprimir los archivos

4. **Clear Directory Decompressed**:
    ```bash
    ./projet_1 clear_directory_decompressed
    ```
    Esta opción se encarga de eliminar los archivos que salen a la hora de descomprimir el archivo binario

5. **Compressed Fork**:
    ```bash
    ./project_1 compressed_fork
    ```
    Esta opción hace que el programa comprima los archivos de texto haciendo uso de la funcion `fork` para crear multiples procesos

6. **Decompressed Fork**:
    ```bash
    ./project_1 decompressed_fork
    ```
    Esta opción descomprime el archivo binario usando la funcion `fork`

7. **Compressed Serial**:
    ```bash
    ./project_1 compressed_serial
    ```
    Esta opción se encarga de comprimir los archivos usando la version serial del algoritmo de Huffman

8. **Decompressed Serial**:
    ```bash
    ./project_1 decompressed_serial
    ```
    Esta opción hace que el programa descomprima de forma serial el archivo binaro con los textos comprimidos

9. **Compressed Threads**:
    ```bash
    ./project_1 compressed_threads
    ```
    Esta opción hace que se compriman los archivos de texto usando el algoritmo de Huffman implementado con hilos "threads"

10. **Decompressed Threads**:
    ```bash
    ./project_1 decompressed_threads
    ```
    Esta opción descomprime el archivo comprimido utilizando hilos "threads"

## Referencias bibliográficas
- GeeksforGeeks. (2023a, abril 7). Huffman Decoding [Decodificación de Huffman]. GeeksforGeeks. https://www.geeksforgeeks.org/huffman-decoding/

- GeeksforGeeks. (2023b, septiembre 11). Huffman Coding [Codificación de Huffman]. GeeksforGeeks. https://www.geeksforgeeks.org/huffman-coding-greedy-algo-3/

- GeeksforGeeks. (2023c, 9 mayo). Thread functions in C/C++ [Funciones de Hilos en C/C++]. GeeksforGeeks. https://www.geeksforgeeks.org/thread-functions-in-c-c/

- Hu, J. (2023, 12 octubre). Usa la función fork en C. Delft Stack. https://www.delftstack.com/es/howto/c/fork-in-c/

- IBM. (2023, marzo 24). Using mutexes [Uso de Mutexes]. IBM. https://www.ibm.com/docs/pt-br/aix/7.2?topic=programming-using-mutexes

- Tanenbaum, A. S., & García, R. E. (2003). Sistemas operativos modernos. Pearson Educación. https://books.google.co.cr/books?id=g88A4rxPH3wC
