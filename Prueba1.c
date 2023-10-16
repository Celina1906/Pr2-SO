#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>


#define MAX_FILENAME_LENGTH 256

#define MAX_FILENAME_LENGTH 256
#define MAX_FILE_COUNT 100

struct File {
    char name[MAX_FILENAME_LENGTH];
    size_t size;
    long start;
    long end;
};

struct PackageInfo {
    struct File files[MAX_FILE_COUNT];
    size_t file_count;
    // conteo de espacios libres
    // pos inicio y fin de los espacios
};

bool verbose_flag = false;

void verbose(char* reports[], int numReports) {
    for (int i = 0; i < numReports; i++) {
        printf("%s\n", reports[i]);
    }
}

void crear(const char* package_name,  char** files, int file_count) {
    char* reports[3]; // Array de punteros a cadenas para informes

    reports[0] = "Se creó el paquete de salida";
    reports[1] = "Se escribieron los archivos siguientes:";
    reports[2] = "Se terminó de escribir el paquete";
    FILE* package = fopen(package_name, "wb");
    if (package == NULL) {
        perror("No se pudo crear el archivo empacado");
        exit(1);
    }
    
    struct PackageInfo package_info;
    package_info.file_count = 0; //inicializamos en 0 --> aumenta cuenta por cada file que se escriba en el paquete
    long current_byte = sizeof(struct PackageInfo);

    for (int i = 0; i < file_count; i++) {
        FILE* file = fopen(files[i], "rb");
        if (file == NULL) {
            perror("No se pudo abrir el archivo");
            exit(1);
        }

        struct File add_file; // Cada file a empacar lleva su struct
        add_file.start = current_byte; // inicializa segun el current byte - 1. si va empezando es 0 sino se actualiza a ultima posicion

        fseek(file, 0, SEEK_END);
        add_file.end = add_file.start + ftell(file); 
        current_byte = add_file.end;

        rewind(file); // Reinicia el puntero del file

        strncpy(add_file.name, files[i], MAX_FILENAME_LENGTH);
        add_file.size = ftell(file);

        // Agregar el archivo a Package Info
        package_info.files[package_info.file_count] = add_file;
        package_info.file_count++;

        // Escribir la información del archivo en el archivo empacado
        // fwrite(&add_file, sizeof(struct File), 1, package);

        // Copiar el contenido del archivo al archivo empacado
        char buffer[1024];
        size_t bytes_read;
        while (bytes_read = fread(buffer, 1, sizeof(buffer), file) > 0) {
            fwrite(buffer, 1, bytes_read, package);
        }
        
        fclose(file);
    }

    // Vuelve al principio del archivo empacado para escribir la información de package_info
    fseek(package, 0, SEEK_SET);
    fwrite(&package_info, sizeof(struct PackageInfo), 1, package);
    
    fclose(package);

    if(verbose_flag){
        int numReports = sizeof(reports) / sizeof(reports[0]);
        verbose(reports, numReports);
    }
}

void recuperar_info(struct PackageInfo *empacado, const char* package_name){
    // Abrir el archivo en modo lectura binaria
    FILE *archivo = fopen(package_name, "rb");

    if (archivo == NULL) {
        perror("Error al abrir el archivo para lectura");
        return;
    }

    // Leer la estructura desde el archivo
    size_t leidos = fread(empacado, sizeof(struct PackageInfo), 1, archivo);

    if (leidos != 1) {
        perror("Error al leer el archivo");
        fclose(archivo);
        return;
    }

    // Cerrar el archivo
    fclose(archivo);

    // Imprimir los datos leídos
    printf("Tamaño de archivo: %lu\n", empacado->file_count);

    // Ahora, puedes acceder a los datos de struct File dentro de struct package_info
    for (size_t i = 0; i < empacado->file_count; i++) {
        printf("Nombre de archivo: %s\n", empacado->files[i].name);
        printf("Tamaño: %lu\n", empacado->files[i].size);
        printf("Inicio: %ld\n", empacado->files[i].start);
        printf("Fin: %ld\n", empacado->files[i].end);
    }
}

void file(const char* package_name, char** files, int file_count){}

void extraer(struct PackageInfo *empacado, const char* package_name){
    // Abrir el archivo empacado en modo lectura binaria
    FILE *archivo = fopen(package_name, "rb");

    if (archivo == NULL) {
        perror("Error al abrir el archivo para lectura");
        return;
    }

    // Leer la estructura desde el archivo
    size_t leidos = fread(empacado, sizeof(struct PackageInfo), 1, archivo);

    if (leidos != 1) {
        perror("Error al leer el archivo");
        fclose(archivo);
        return;
    }

    // Ahora, puedes acceder a los datos de struct File dentro de struct package_info
    for (size_t i = 0; i < empacado->file_count; i++) {
        // Crear un nuevo archivo con el nombre del archivo extraído
        FILE* extracted_file = fopen(empacado->files[i].name, "wb");
        if (extracted_file == NULL) {
            perror("Error al crear el archivo extraído");
            fclose(archivo);
            return;
        }

        // Mover el puntero del archivo empacado al inicio del archivo a extraer
        fseek(archivo, empacado->files[i].start, SEEK_SET);

        // Copiar el contenido del archivo empacado al archivo extraído
        char buffer[1024];
        size_t bytes_read;
        while (bytes_read = fread(buffer, 1, sizeof(buffer), archivo) > 0) {
            fwrite(buffer, 1, bytes_read, extracted_file);
        }

        // Cerrar el archivo extraído
        fclose(extracted_file);
    }

    // Cerrar el archivo empacado
    fclose(archivo);

    printf("Archivos extraídos con éxito.\n");
}

void listar(struct PackageInfo *empacado, const char* package_name){
    // Abrir el archivo en modo lectura binaria
    FILE *archivo = fopen(package_name, "rb");

    if (archivo == NULL) {
        perror("Error al abrir el archivo para lectura");
        return;
    }

    // Leer la estructura desde el archivo
    size_t leidos = fread(empacado, sizeof(struct PackageInfo), 1, archivo);

    if (leidos != 1) {
        perror("Error al leer el archivo");
        fclose(archivo);
        return;
    }

    // Cerrar el archivo
    fclose(archivo);


    // Ahora, puedes acceder a los datos de struct File dentro de struct package_info
    for (size_t i = 0; i < empacado->file_count; i++) {
        printf("Nombre de archivo: %s\n", empacado->files[i].name);
    }
}

void delete(const char* package_name, const char* file_name){
   // Abrir el archivo empacado en modo lectura/escritura binaria
    FILE *archivo = fopen(package_name, "rb+");

    if (archivo == NULL) {
        perror("Error al abrir el archivo para lectura/escritura");
        return;
    }

    struct PackageInfo empacado;
    size_t leidos = fread(&empacado, sizeof(struct PackageInfo), 1, archivo);

    if (leidos != 1) {
        perror("Error al leer el archivo");
        fclose(archivo);
        return;
    }

    // Buscar el archivo a borrar
    int indice_borrar = -1;
    for (size_t i = 0; i < empacado.file_count; i++) {
        if (strcmp(empacado.files[i].name, file_name) == 0) {
            indice_borrar = i;
            break;
        }
    }

    if (indice_borrar == -1) {
        printf("El archivo \"%s\" no existe en el paquete.\n", file_name);
        fclose(archivo);
        return;
    }

    // Mover el puntero al inicio del archivo a borrar
    fseek(archivo, empacado.files[indice_borrar].start, SEEK_SET);

    // Eliminar el contenido del archivo del paquete
    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), archivo)) > 0) {
        fseek(archivo, -bytes_read, SEEK_CUR);
        fwrite(buffer, 1, bytes_read, archivo);
        fseek(archivo, bytes_read, SEEK_CUR);
    }

    // Actualizar el índice de archivos
    empacado.file_count--;

    // Mover el puntero al inicio del paquete y escribir la información actualizada
    fseek(archivo, 0, SEEK_SET);
    fwrite(&empacado, sizeof(struct PackageInfo), 1, archivo);

    // Truncar el archivo para eliminar el espacio del archivo borrado
    fseek(archivo, sizeof(struct PackageInfo) + empacado.files[indice_borrar].end, SEEK_SET);
    long new_size = ftell(archivo);
    fclose(archivo);

    FILE* truncated_file = fopen(package_name, "rb+");
    ftruncate(fileno(truncated_file), new_size);
    fclose(truncated_file);

    printf("El archivo \"%s\" ha sido eliminado del paquete.\n", file_name);
}

void actualizar(){}



void agregar(){}

void desfragmentar(){}

int main(int argc, char *argv[]) {
 /*if (argc < 3) {
        printf("Uso: %s <comando> <archivoEmpacado> [archivos...]\n", argv[0]);
        return 1;
    }

    const char* comando = argv[1];
    long int longitud_comando = strlen(comando)-1;
    const char* package_name = argv[2];
    
    // argv[1][1]
    // printf("Comando: %c\n", comando[0]);
    // printf("Tam: %ld\n", longitud_comando);
    const char* archivos = argv[3];
    for (int i = 1; i <= longitud_comando; i++){
        if (comando[i] == 'c'){
            printf("Opcion de comando: Create \n");
            crear(package_name, &argv[3], argc - 3);
        }
        else if (comando[i] =='x'){
            printf("Opcion de comando: Extract \n");
        }
        else if(comando[i] == 't'){
            printf("Opcion de comando: List \n");
        }
        else if(comando[i] == 'd'){
            printf("Opcion de comando: Delete \n");
        }
        else if(comando[i] == 'u'){
            printf("Opcion de comando: Update \n");
        }
        else if(comando[i] == 'v'){
            printf("Opcion de comando: Verbose \n");
            verbose_flag = true;
        }
        else if(comando[i] == 'f'){
            printf("Opcion de comando: File \n");
            file(package_name, &argv[3], argc - 3);
        }
        else if(comando[i] == 'r'){ //append
            printf("Opcion de comando: Append \n");
        }
        else if(comando[i] == 'p'){
            printf("Opcion de comando: Pack \n");
        }
        else{
            printf("La letra ingresada no es valida");
        }
        
    }*/

    struct PackageInfo archivo;

    const char* nombre = "prueba2.jaja";

    listar(&archivo, nombre);

    //extraer(&archivo, nombre);

    delete(nombre, "Paises.txt");

    listar(&archivo, nombre);


    return 0;
}