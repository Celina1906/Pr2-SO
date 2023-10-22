#include <stdio.h>
#include <stdlib.h>

int main() {
    // Paso 1: Abrir el archivo no binario en modo lectura
    FILE* archivoNoBinario = fopen("Ciudades.txt", "rb");
    if (archivoNoBinario == NULL) {
        printf("No se pudo abrir el archivo no binario.\n");
        return 1;
    }

    // Paso 2: Abrir un archivo binario en modo escritura
    FILE* archivoBinario = fopen("prueba.ja", "wb");
    if (archivoBinario == NULL) {
        printf("No se pudo abrir el archivo binario.\n");
        fclose(archivoNoBinario);
        return 1;
    }

    // Paso 3: Leer del archivo no binario y escribir en el archivo binario usando fread y fwrite
    char buffer[1024];
    size_t bytesLeidos;
    while (bytesLeidos = fread(buffer, 1, sizeof(buffer), archivoNoBinario) > 0) {
        fwrite(buffer, 1, bytesLeidos, archivoBinario);
    }

    // Paso 4: Cerrar ambos archivos
    fclose(archivoNoBinario);
    fclose(archivoBinario);

    // Paso 5: Abrir un nuevo archivo en modo escritura y escribir datos en el formato del archivo original
    FILE* archivoSalida = fopen("archivoSalida.txt", "w");
    if (archivoSalida == NULL) {
        printf("No se pudo abrir el archivo de salida.\n");
        return 1;
    }

    // Aquí puedes definir la estructura y el formato del archivo de salida
    // Supongamos que estás escribiendo texto, puedes realizar algún procesamiento si es necesario

    // Vuelve a abrir el archivo binario en modo lectura
    archivoBinario = fopen("prueba.ja", "rb");
    if (archivoBinario == NULL) {
        printf("No se pudo abrir el archivo binario para lectura.\n");
        fclose(archivoSalida);
        return 1;
    }

    // Leer datos del archivo binario y escribirlos en el archivo de salida en el formato adecuado
    while (bytesLeidos = fread(buffer, 1, sizeof(buffer), archivoBinario) > 0) {
        // Realiza cualquier procesamiento necesario antes de escribir en el archivo de salida
        // En este ejemplo, asumimos que el archivo de salida es de texto
        // y simplemente escribimos los caracteres.
        fwrite(buffer, 1, bytesLeidos, archivoSalida);
    }

    // Paso 4: Cierra los archivos nuevamente
    fclose(archivoSalida);
    fclose(archivoBinario);

    printf("Proceso completado con éxito.\n");
    return 0;
}
