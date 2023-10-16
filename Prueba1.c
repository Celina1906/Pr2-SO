#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILENAME_LENGTH 256

struct File {
    char name[MAX_FILENAME_LENGTH];
    size_t size;
};

int crear(const char* package_name){
    // Crear una estructura File y escribirla en un archivo binario
    struct File archivo_escrito = {"miarchivo.txt", 1000};

    FILE *archivo = fopen("file_data.jaja", "wb"); // Abrir el archivo en modo escritura binaria

    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return 1;
    }

    // Escribir la estructura en el archivo
    size_t escritos = fwrite(&archivo_escrito, sizeof(struct File), 1, archivo);

    if (escritos != 1) {
        perror("Error al escribir en el archivo");
        fclose(archivo);
        return 1;
    }

    fclose(archivo);
    
    //Lectura de archivo
    // printf("Archivo de bytes creado y escrito.\n");

    // // Leer la estructura desde el archivo
    // struct File archivo_leido;

    // archivo = fopen("file_data.bin", "rb"); // Abrir el archivo en modo lectura binaria

    // if (archivo == NULL) {
    //     perror("Error al abrir el archivo para lectura");
    //     return 1;
    // }

    // // Leer la estructura desde el archivo
    // size_t leidos = fread(&archivo_leido, sizeof(struct File), 1, archivo);

    // if (leidos != 1) {
    //     perror("Error al leer el archivo");
    //     fclose(archivo);
    //     return 1;
    // }

    // // Cerrar el archivo
    // fclose(archivo);

    // // Imprimir los datos leídos
    // printf("Nombre: %s\n", archivo_leido.name);
    // printf("Tamaño: %lu\n", archivo_leido.size);

}

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