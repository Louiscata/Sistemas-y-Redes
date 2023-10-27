#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <math.h>

#define NUMERO_COMICAMENTE_GRANDE 70000000

double calcular_tiempo (struct timeval inicio, struct timeval fin) {
    return(fin.tv_sec-inicio.tv_sec+(fin.tv_usec-inicio.tv_usec)/1.e6);
}

int main () {
    pid_t pares, impares, completo, media;
    FILE* datos;
    
    //En este fichero se guardarán los datos
    if ((datos = fopen("datos.txt", "w+")) == NULL)
    {
        perror("No se pudo abrir el archivo de datos\n");
        return(EXIT_FAILURE);
    }

    printf("\nAqui el padre con PID %d, voy a crear 3 hijos.\n\n",getpid());

    impares = fork();

    if (impares == 0)
    {
        //Código del hijo 1 (cálculo de impares)
        printf("Aqui el hijo 1 con PID %d calculando la media de impares.\n",getpid());
        
        struct timeval inicio, fin;
        double suma_impares = 0.0;
        gettimeofday(&inicio,NULL);

        for (size_t i = 1; i < NUMERO_COMICAMENTE_GRANDE; i += 2)
        {
            suma_impares += tan(sqrt((double)i));
        }

        gettimeofday(&fin,NULL);

        double media_impares = suma_impares * 2.0 / NUMERO_COMICAMENTE_GRANDE;
        double tiempo = calcular_tiempo(inicio, fin);
        
        printf("El hijo 1 ha calculado la media de impares en %.20lfms, la cual vale %.20lf\n", tiempo*1000, media_impares);
        
        fprintf(datos, "IMPARES:%.20lf;%.20lf\n",media_impares,tiempo);

        fclose(datos);

        return(EXIT_SUCCESS);
    }

    pares = fork();

    if (pares == 0)
    {
        //Código del hijo 2 (cálculo de pares)
        printf("Aqui el hijo 2 con PID %d calculando la media de pares\n",getpid());

        struct timeval inicio, fin;
        double suma_pares = 0.0;
        
        gettimeofday(&inicio,NULL);

        for (size_t i = 2; i < NUMERO_COMICAMENTE_GRANDE; i+=2)
        {
            suma_pares += tan(sqrt((double)i));
        }

        gettimeofday(&fin,NULL);
        
        double media_pares = suma_pares * 2.0 / NUMERO_COMICAMENTE_GRANDE;
        double tiempo = calcular_tiempo(inicio, fin);
    
        printf("El hijo 2 ha calculado la media de pares en %.20lfms, la cual vale %.20lf\n", tiempo*1000, media_pares);
        
        fprintf(datos, "PARES:%.20lf;%.20lf\n",media_pares,tiempo);

        fclose(datos);

        return(EXIT_SUCCESS);
    }

    printf("\nAqui el padre con PID %d. Voy a crear el tercer hijo.\n\n", getpid());

    completo = fork();

    if(completo == 0)
    {
        //Código del hijo 3 (cálculo completo)
        printf("Aqui el hijo 3 con PID %d calculando la media total\n",getpid());
        
        struct timeval inicio, fin;
        double suma_completa = 0.0;
        
        gettimeofday(&inicio,NULL);

        for (size_t i = 1; i < NUMERO_COMICAMENTE_GRANDE; i += 1)
        {
            suma_completa += tan(sqrt((double)i));
        }

        gettimeofday(&fin,NULL);

        double media_completa = suma_completa / NUMERO_COMICAMENTE_GRANDE;

        double tiempo = calcular_tiempo(inicio, fin);
        
        printf("El hijo 3 ha calculado la media total en %.20lfms, la cual vale %.20lf\n", tiempo*1000, media_completa);
        
        fprintf(datos, "COMPLETA:%.20lf;%.20lf\n",media_completa,tiempo);

        fclose(datos);
        
        return(EXIT_SUCCESS);
    }
    
    //Cuando acaben los dos primeros hijos, un cuarto hijo debe calcular el resultado en base a ambas
    //contribuciones, leyéndolas del fichero auxiliar, y añadir el resultado en ese mismo fichero
    int estado;

    printf("\nAqui el padre con PID %d. Esperando a que mis dos primeros hijos terminen.\n\n", getpid());

    waitpid(impares, &estado, 0);
    if (WIFEXITED(estado)) {
        int exit_estado = WEXITSTATUS(estado);
        printf("El hijo 1 terminó con valor de salida: %d\n", exit_estado);
    }

    waitpid(pares, &estado, 0);
    if (WIFEXITED(estado)) {
        int exit_estado = WEXITSTATUS(estado);
        printf("El hijo 2 terminó con valor de salida: %d\n", exit_estado);
    }

    printf("\nAqui el padre con PID %d. Mis dos primeros hijos han terminado. Voy a crear el cuarto hijo.\n\n", getpid());

    media = fork();

    if(media == 0)
    {
        //Código del hijo 4 (media de las medias de impares y pares)
        printf("Aqui el hijo 4 con PID %d calculando la media de las medias\n",getpid());
        struct timeval inicio, fin;
        gettimeofday(&inicio,NULL);

        double media1, media2, media;

        rewind(datos);

        char linea[100];
        while (fgets(linea, sizeof(linea), datos) != NULL) {
            //Se usa sscanf para buscar y extraer los valores de las líneas que coinciden con los encabezados
            //Usamos la variable 'media' de manera temporal para almacenar el dato que no necesitamos
            if (sscanf(linea, "PARES:%lf;%lf", &media1, &media) == 1) {
                continue;
            } else if (sscanf(linea, "IMPARES:%lf;%lf", &media2, &media) == 1) {
                continue;
            }
        }
        
        media = (media1 + media2) / 2.0;

        gettimeofday(&fin,NULL);
        double tiempo = calcular_tiempo(inicio, fin);
        printf("El hijo 4 ha calculado la media de las medias en %.20lfms, la cual vale %.20lf\n", tiempo*1000, media);
        fprintf(datos, "MEDIA DE LAS MEDIAS:%.20lf;%.20lf\n",media,tiempo);

        fclose(datos);
        
        return(EXIT_SUCCESS);
    }

    //Finalmente, cuando todos acaben, el padre debe calcular y mostrar la diferencia entre los resultados calculados de las dos formas

    printf("\nAqui el padre con PID %d. Esperando a que mis hijos 3 y 4 acaben.\n\n", getpid());

    //Espera a que acaben el 3 y el 4

    waitpid(completo, &estado, 0);
    if (WIFEXITED(estado)) {
        int exit_estado = WEXITSTATUS(estado);
        printf("El hijo 3 terminó con valor de salida: %d\n", exit_estado);
    }

    waitpid(media, &estado, 0);
    if (WIFEXITED(estado)) {
        int exit_estado = WEXITSTATUS(estado);
        printf("El hijo 4 terminó con valor de salida: %d\n", exit_estado);
    }

    printf("\nAqui el padre con PID %d. Mis hijos 3 y 4 han terminado. Voy a imprimir los resultados.\n\n", getpid());

    //Volvemos al principio del fichero para leer todos los datos
    rewind(datos);

    //Mostramos todos los datos
    
    double media_pares = 0.0, tiempo_pares = 0.0, media_impares = 0.0, tiempo_impares = 0.0;
    double media_calculada = 0.0, tiempo_calculada = 0.0, media_total = 0.0, tiempo_total = 0.0;
    char linea[100];
    while (fgets(linea, sizeof(linea), datos) != NULL) {
        // Usar sscanf para buscar y extraer los valores de las líneas que coinciden con los encabezados
        if (sscanf(linea, "PARES:%lf;%lf", &media_pares, &tiempo_pares) == 2) {
            continue;
        } else if (sscanf(linea, "IMPARES:%lf;%lf", &media_impares, &tiempo_impares) == 2) {
            continue;
        } else if (sscanf(linea, "MEDIA DE LAS MEDIAS:%lf;%lf", &media_calculada, &tiempo_calculada) == 2) {
            continue;
        } else if (sscanf(linea, "COMPLETA:%lf;%lf", &media_total, &tiempo_total) == 2) {
            continue;
        }
    }
    
    printf("La diferencia entre ambos calculos es de %.20lf\n\n",media_total-media_calculada);

    printf("Tiempo del hijo 1: %.20lf\n",tiempo_impares);
    printf("Tiempo del hijo 2: %.20lf\n",tiempo_pares);
    printf("Tiempo del hijo 3: %.20lf\n",tiempo_total);
    printf("Tiempo del hijo 4: %.20lf\n",tiempo_calculada);

    fclose(datos);
    
    return 0;
}

