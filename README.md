# Huffman Coding Project - proyecto_1_IC6600

## Grupo

- Ávila Ramírez Paublo Alexander (2022065584)
- Reyes Rodríguez Ricardo Andrés (2022101681)
- Zúñiga Campos Omar Jesús (2022019053)

Este proyecto implementa el algoritmo Huffman para comprimir y descomprimir archivos de texto en paralelo haciendo uso de `fork` y `threads. Ademas del metodo serial normal y un web crawler para descargar los libros en texto plano para usar en la ejecucion del proyecto.


## Introduccion

Algoritmo de Huffman para comprimir archivos de texto. En este proyecto, se presenta varias maneras de implementar este algoritmo:

- **Fork**: Procesado en paralelo usando la funcion `fork`.
- **Thread**: Proceso multi-hilo usando hilos con la biblioteca `pthread`.
- **Serial**: Implementacion normal de forma serial. 
- **Web Crawler**: Web crawler diseñado para obtener los archivos para la ejecucion del programa.

1. **Fork**:

fork() es una llamada al sistema en Unix/Linux que permite crear nuevos procesos. Esta función divide el proceso en dos: el proceso padre y el proceso hijo. Ambos procesos continúan ejecutándose desde el punto donde se invocó fork(), pero con un espacio de direcciones independiente.

- Valores de retorno: fork() devuelve 0 en el proceso hijo y el PID del hijo en el proceso padre. Si falla, retorna -1.
- Memoria: Aunque inicialmente comparten memoria (copy-on-write), cualquier cambio en uno de los procesos hace que las páginas modificadas se copien, asegurando independencia.
- Herencia: El proceso hijo hereda descriptores de archivos, variables de entorno y otros atributos del padre, pero no hereda ciertos identificadores de procesos ni bloqueos.
- Eficiencia: La creación de procesos con fork() es más costosa que con hilos, ya que los procesos no comparten el mismo espacio de memoria. Sin embargo, proporciona un aislamiento completo entre procesos, lo que es útil para seguridad y estabilidad.
- Ejemplo de uso: fork() se combina frecuentemente con exec() para ejecutar nuevos programas en el contexto del proceso hijo.

La función es fundamental para sistemas multitarea y paralelismo, pero su mal uso puede generar problemas como la saturación de recursos (fork bomb).

2. **Thread**:

La biblioteca pthread permite la creación de hilos para implementar concurrencia y paralelismo en programas de C. Cada hilo ejecuta una función definida por el programador y puede recibir argumentos a través de un puntero.

- Creación de hilos: Se utiliza pthread_create() para crear un hilo que ejecuta una función, y pthread_join() para esperar a que un hilo termine.
    ```c
    pthread_create(&thread, NULL, print_message, NULL);
    pthread_join(thread, NULL);
    ```
- Sincronización: La biblioteca incluye mecanismos como mutexes, variables de condición y semáforos para manejar el acceso a recursos compartidos y evitar condiciones de carrera. Un mutex asegura que solo un hilo acceda a una sección crítica a la vez.
    ```c
    pthread_mutex_lock(&mutex);
    // Sección crítica
    pthread_mutex_unlock(&mutex);
    ```
- Variables de condición: Permiten que un hilo espere hasta que una condición específica se cumpla, coordinando la ejecución entre hilos mediante pthread_cond_wait() y pthread_cond_signal().
- Otras herramientas: También soporta bloqueos de lectura-escritura (pthread_rwlock_t) y semáforos para controlar el acceso concurrente a recursos.
- Finalización de hilos: Se utiliza pthread_exit() para terminar un hilo de forma segura, permitiendo la devolución de valores si es necesario.

La biblioteca pthread es esencial para desarrollar aplicaciones concurrentes y paralelas en C, proporcionando las herramientas necesarias para la sincronización y gestión eficiente de hilos.

## Implementación de Huffman en Modo Fork

La implementación del algoritmo de Huffman utilizando \texttt{fork()} involucra las siguientes funciones clave:

\begin{itemize}
    \item \texttt{compress\_files\_fork}: Esta función principal crea múltiples procesos usando \texttt{fork()} y asigna a cada uno la tarea de comprimir una parte del archivo de entrada.
    \item \texttt{decompress\_files\_fork}: Realiza la descompresión en paralelo utilizando \texttt{fork()}. Cada proceso hijo descomprime una parte del archivo y escribe el resultado en archivos temporales.
    \item \texttt{process\_part\_fork}: Esta función se encarga de realizar la compresión o descompresión de una sección específica del archivo que le es asignada a cada proceso hijo.
    \item \texttt{wait\_for\_children}: Se utiliza para que el proceso padre espere a que todos los procesos hijos hayan terminado antes de unir los resultados parciales.
\end{itemize}

## Implementación de Huffman en Modo Threads

La implementación utilizando hilos (\texttt{pthread}) incluye las siguientes funciones:

\begin{itemize}
    \item \texttt{compress\_files\_threads}: Función principal que inicia múltiples hilos (\texttt{pthread\_create()}) para dividir el archivo y comprimirlo en paralelo. 
    \item \texttt{decompress\_files\_threads}: Similar a la versión de compresión, esta función inicia múltiples hilos para descomprimir el archivo en paralelo.
    \item \texttt{compress\_part\_threads}: Esta función es ejecutada por cada hilo y se encarga de comprimir la parte asignada del archivo.
    \item \texttt{decompress\_part\_threads}: Función encargada de descomprimir la parte del archivo que le es asignada a cada hilo.
    \item \texttt{pthread\_mutex\_lock} y \texttt{pthread\_mutex\_unlock}: Estas funciones se utilizan para sincronizar el acceso a los recursos compartidos entre los hilos, como el archivo de salida.
    \item \texttt{pthread\_join}: Se asegura de que el hilo principal espere a que todos los hilos terminen su trabajo antes de continuar.
\end{itemize}

## Implementación Serial de Huffman

La versión serial del algoritmo de Huffman incluye las siguientes funciones:

\begin{itemize}
    \item \texttt{compress\_file\_serial}: Función principal que realiza la compresión de manera secuencial. Calcula las frecuencias, construye el árbol de Huffman y codifica el archivo.
    \item \texttt{decompress\_file\_serial}: Función que reconstruye el árbol de Huffman a partir de la información almacenada en el archivo comprimido y realiza la descompresión.
    \item \texttt{build\_huffman\_tree}: Construye el árbol de Huffman utilizando las frecuencias de los caracteres.
    \item \texttt{generate\_huffman\_codes}: Genera los códigos de Huffman a partir del árbol y los almacena para la compresión.
    \item \texttt{write\_compressed\_file}: Escribe el archivo comprimido utilizando los códigos de Huffman generados.
    \item \texttt{read\_compressed\_file}: Lee el archivo comprimido para su descompresión.
    \item \texttt{rebuild\_huffman\_tree}: Reconstruye el árbol de Huffman durante la descompresión usando los datos codificados en el archivo.
\end{itemize}


## Instalacion

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

## Compilacion

Una vez este dentro de la carpeta del proyecto en la terminal ejecute el siguiente comando:

```bash
    make
```

Esto va a compilar el proyecto completo para dejarlo listo para su ejecucion.

## Ejecucion

1. **Lista de opciones**:
    Ejecute el siguiente comando:
    ```bash
    ./project_1
    ```
    Esto va a desplegar las diferentes opciones para ejecutar el programa

2. **Download books**:
    ```bash
    ./project_1 download_books
    ```
    Esto va a activar el webcrawler para descargar los archivos necesarios para probar la funcionalidad del algoritmo huffman con sus distintas implementaciones.

3. **Clear Directory Compressed**:
    ```bash
    ./project_1 clear_directory_compressed
    ```
    Esta opcion se encargar de eliminar el archivo binario que sale despues de comprimir los archivos

4. **Clear Directory Decompressed**:
    ```bash
    ./projet_1 clear_directory_decompressed
    ```
    Esta opcion se encarga de eliminar los archivos que salen a la hora de descomprimir el archivo binario

5. **Compressed Fork**:
    ```bash
    ./project_1 compressed_fork
    ```
    Esta opcion hace que el programa comprima los archivos de texto haciendo uso de la funcion `fork` para crear multiples procesos

6. **Decompressed Fork**:
    ```bash
    ./project_1 decompressed_fork
    ```
    Esta opcion descomprime el archivo binario usando la funcion `fork`

7. **Compressed Serial**:
    ```bash
    ./project_1 compressed_serial
    ```
    Esta opcion se encarga de comprimir los archivos usando la version serial del algoritmo de Huffman

8. **Decompressed Serial**:
    ```bash
    ./project_1 decompressed_serial
    ```
    Esta opcion hace que el programa descomprima de forma serial el archivo binaro con los textos comprimidos

9. **Compressed Threads**:
    ```bash
    ./project_1 compressed_threads
    ```
    Esta opcion hace que se compriman los archivos de texto usando el algoritmo de Huffman implementado con hilos "threads"

10. **Decompressed Threads**:
    ```bash
    ./project_1 decompressed_threads
    ```
    Esta opcion descomprime el archivo comprimido utilizando hilos "threads"

