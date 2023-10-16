#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILENAME_LENGTH 256

#define MAX_FILENAME_LENGTH 256
#define MAX_FILE_COUNT 100

struct File {
    char name[MAX_FILENAME_LENGTH];
    size_t size;
    long start;
    long end;
};

struct Archive {
    struct File files[MAX_FILE_COUNT];
    size_t file_count;
    // conteo de espacios libres
    // pos inicio y fin de los espacios
};

void crear(const char* package_name) {
    FILE *archivo = fopen(package_name, "wb"); // Abrir el archivo en modo escritura binaria

    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return;
    }

    struct Archive archive;
    archive.file_count = 0;
    
    // Escribir la estructura Archive en el archivo
    size_t escritos = fwrite(&archive, sizeof(struct Archive), 1, archivo);

    if (escritos != 1) {
        perror("Error al escribir en el archivo");
    }

    fclose(archivo);

}

void recuperar_info(struct Archive *empacado, const char* package_name){
    // Abrir el archivo en modo lectura binaria
    FILE *archivo = fopen(package_name, "rb");

    if (archivo == NULL) {
        perror("Error al abrir el archivo para lectura");
        return;
    }

    // Leer la estructura desde el archivo
    size_t leidos = fread(empacado, sizeof(struct Archive), 1, archivo);

    if (leidos != 1) {
        perror("Error al leer el archivo");
        fclose(archivo);
        return;
    }

    // Cerrar el archivo
    fclose(archivo);

    // Imprimir los datos leídos
    printf("Tamaño de archivo: %lu\n", empacado->file_count);

    // Ahora, puedes acceder a los datos de struct File dentro de struct Archive
    for (size_t i = 0; i < empacado->file_count; i++) {
        printf("Nombre de archivo: %s\n", empacado->files[i].name);
        printf("Tamaño: %lu\n", empacado->files[i].size);
        printf("Inicio: %ld\n", empacado->files[i].start);
        printf("Fin: %ld\n", empacado->files[i].end);
    }
}

void file(const char* package_name, const char** files, int file_count){}

void extraer(){}

void listar(){}

void delete(){}

void actualizar(){}

void verbose(){}

void agregar(){}

void desfragmentar(){}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: %s <comando> <archivoEmpacado> [archivos...]\n", argv[0]);
        return 1;
    }

    const char* comando = argv[1];
    long int longitud_comando = strlen(comando)-1;
    const char* package_name = argv[2];
    // argv[1][1]
    // printf("Comando: %c\n", comando[0]);
    // printf("Tam: %ld\n", longitud_comando);

    for (int i = 1; i <= longitud_comando; i++){
        if (comando[i] == 'c'){
            printf("Opcion de comando: Create \n");
            crear(package_name);
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
        
    }


    return 0;
}