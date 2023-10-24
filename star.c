/*
   Proyecto 2
   Estudiantes:
        Celina Madrigal Murillo - 2020059364
        María José Porras Maroto - 2019066056
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

//Variables Globales
#define MAX_FILENAME_LENGTH 256
#define MAX_FILE_SIZE 1048576 // 1 MB 
#define MAX_FILENAME_LENGTH 256
#define MAX_FILE_COUNT 100

//Estructuras
struct File {
    char name[MAX_FILENAME_LENGTH];
    size_t size;
    long start;
    long end;
};

struct PackageInfo {
    struct File files[MAX_FILE_COUNT];
    size_t file_count;
};

//Variables Globales
bool verbose_flag = false;
bool vverbose_flag = false;
bool create_flag = false;
bool delete_flag = false;
bool list_flag = false;
bool extract_flag = false;
bool update_flag = false;
bool append_flag = false;
bool pack_flag = false;
bool file_flag = false;

void verbose(char* frase, int tipo_frase) {

    if(verbose_flag == true && tipo_frase == 1){
        printf("%s\n",frase);
    }
    else if (vverbose_flag == true)
    {
       printf("%s\n",frase);
    } 
}

void crear(const char* package_name,  char** files, int file_count) {

    verbose("Inicia opcion de crear",1);
    verbose("Obtiene información del archivo",2);
    FILE* package = fopen(package_name, "wb");
    if (package == NULL) {
        perror("No se pudo crear el archivo empacado");
        exit(1);
    }
    
    struct PackageInfo package_info;
    package_info.file_count = 0; 
    long current_byte = sizeof(struct PackageInfo);

    verbose("Comienza ciclo de archivos",1);

    for (int i = 0; i < file_count; i++) {
        FILE* file = fopen(files[i], "rb");
        if (file == NULL) {
            perror("No se pudo abrir el archivo");
            exit(1);
        }

        struct File add_file; //Cada file a empacar lleva su struct
        add_file.start = current_byte; // inicializa segun el current byte - 1. si va empezando es 0 sino se actualiza a ultima posicion

        verbose("Obtiene tamaño del archivo",1);

        fseek(file, 0, SEEK_END);
        add_file.end = add_file.start + ftell(file); 
        current_byte = add_file.end;

        verbose("Reinicia puntero del archivo",2);
        rewind(file); //Reinicia el puntero del file

        verbose("Copia nombre del archivo",2);
        strncpy(add_file.name, files[i], MAX_FILENAME_LENGTH);
        add_file.size = ftell(file);

        //Agrega el archivo a Package Info
        verbose("Agrega archivo",1);
        package_info.files[package_info.file_count] = add_file;
        package_info.file_count++;

        //Copia el contenido del archivo al archivo empacado
        verbose("Copia contenido del archivo al archivo empacado",2);
        char buffer[1024];
        size_t bytes_read;
        while (bytes_read = fread(buffer, 1, sizeof(buffer), file) > 0) {
            fwrite(buffer, 1, bytes_read, package);
        }
        
        fclose(file);
    }

    //Vuelve al principio del archivo empacado para escribir la información de package_info
    fseek(package, 0, SEEK_SET);
    fwrite(&package_info, sizeof(struct PackageInfo), 1, package);
    verbose("Cierra el archivo empacado",1);
    fclose(package);
    printf("Paquete Creado con éxito. \n");
}

void recuperar_info(struct PackageInfo *empacado, const char* package_name){

    verbose("Inicia opción de recuperar info",1);
    verbose("Abre el archivo en modo lectura binaria",2);

    //Abre el archivo en modo lectura binaria
    FILE *archivo = fopen(package_name, "rb");

    if (archivo == NULL) {
        perror("Error al abrir el archivo para lectura");
        return;
    }

    verbose("Lee la estructura desde el archivo",1);

    //Lee la estructura desde el archivo
    size_t leidos = fread(empacado, sizeof(struct PackageInfo), 1, archivo);

    if (leidos != 1) {
        perror("Error al leer el archivo");
        fclose(archivo);
        return;
    }

    verbose("Cierra el archivo",1);

    //Cierra el archivo
    fclose(archivo);

    //Imprime los datos leídos
    printf("Tamaño de archivo: %lu\n", empacado->file_count);

    verbose("Imprime el tamaño de archivo",1);

    //Accede a los datos de struct File dentro de struct package_info
    for (size_t i = 0; i < empacado->file_count; i++) {

        verbose("Accede al archivo",1);

        //Imprime los datos del archivo
        printf("Nombre de archivo: %s\n", empacado->files[i].name);
        printf("Tamaño: %lu\n", empacado->files[i].size);
        printf("Inicio: %ld\n", empacado->files[i].start);
        printf("Fin: %ld\n", empacado->files[i].end);
    }
}

void extraer(struct PackageInfo *empacado, const char* package_name){
    
    verbose("Inicia opción de extraer",1);
    verbose("Abre el archivo empacado en modo lectura binaria",2);

    //Abre el archivo empacado en modo lectura binaria
    FILE *archivo = fopen(package_name, "rb");

    if (archivo == NULL) {
        perror("Error al abrir el archivo para lectura");
        return;
    }

    verbose("Lee la estructura desde el archivo",1);

    //Lee la estructura desde el archivo
    size_t leidos = fread(empacado, sizeof(struct PackageInfo), 1, archivo);

    if (leidos != 1) {
        perror("Error al leer el archivo");
        fclose(archivo);
        return;
    }

    verbose("Accede a los datos de struct File dentro de struct package_info",1);
    verbose("Comienza ciclo de archivos",2);

    //Accede a los datos de struct File dentro de struct package_info
    for (size_t i = 0; i < empacado->file_count; i++) {

        verbose("Crea un nuevo archivo con el nombre del archivo extraído",1);

        //Crea un nuevo archivo con el nombre del archivo extraído
        FILE* extracted_file = fopen(empacado->files[i].name, "wb");
        if (extracted_file == NULL) {
            perror("Error al crear el archivo extraído");
            fclose(archivo);
            return;
        }

        verbose("Mueve el puntero del archivo empacado al inicio del archivo a extraer",2);

        //Mueve el puntero del archivo empacado al inicio del archivo a extraer
        fseek(archivo, empacado->files[i].start, SEEK_SET);

        verbose("Copia el contenido del archivo empacado al archivo extraído",2);

        //Copia el contenido del archivo empacado al archivo extraído
        char buffer[1024];
        size_t bytes_read;
        while (bytes_read = fread(buffer, 1, sizeof(buffer), archivo) > 0) {
            fwrite(buffer, 1, bytes_read, extracted_file);
        }

        verbose("Cierra el archivo extraído",1);

        //Cierra el archivo extraído
        fclose(extracted_file);
    }

    verbose("Cierra el archivo empacado",1);

    //Cierra el archivo empacado
    fclose(archivo);

    verbose("Archivos extraídos",1);

    printf("Archivos extraídos con éxito.\n");
}

void listar(struct PackageInfo *empacado, const char* package_name){

    verbose("Inicia opción de listar",1);
    verbose("Abre el archivo en modo lectura binaria",2);

    //Abre el archivo en modo lectura binaria
    FILE *archivo = fopen(package_name, "rb");

    if (archivo == NULL) {
        perror("Error al abrir el archivo para lectura");
        return;
    }

    verbose("Lee la estructura desde el archivo",1);

    //Lee la estructura desde el archivo
    size_t leidos = fread(empacado, sizeof(struct PackageInfo), 1, archivo);

    if (leidos != 1) {
        perror("Error al leer el archivo");
        fclose(archivo);
        return;
    }

    verbose("Cierra el archivo",1);

    //Cierra el archivo
    fclose(archivo);

    //Accede a los datos de struct File dentro de struct package_info
    for (size_t i = 0; i < empacado->file_count; i++) {

        verbose("Imprime el nombre del archivo",1);

        //Imprime los datos del archivo
        printf("Nombre de archivo: %s\n", empacado->files[i].name);
    }
}

void encontrar_espacios_libres(struct PackageInfo *empacado, long archivo_tamano) {

    verbose("Inicia la búsqueda de espacios libres",1);

    //Inicializa el inicio del espacio libre
    long inicio = sizeof(struct PackageInfo);

    //Recorre los archivos del paquete
    for (size_t i = 0; i < empacado->file_count; i++) {

        //Calcula el final del espacio libre actual
        long fin = empacado->files[i].start;
        if (i + 1 < empacado->file_count) {
            fin = empacado->files[i + 1].start;
        }

        //Si el espacio libre actual es lo suficientemente grande, lo asigna
        if (fin - inicio >= archivo_tamano) {

            verbose("Encuentra un espacio libre de tamaño suficiente",1);

            //Asigna el espacio libre
            empacado->files[empacado->file_count].start = inicio;
            empacado->files[empacado->file_count].end = inicio + archivo_tamano;
            empacado->file_count++;

            //Termina la función
            return;
        }

        //Actualiza el inicio del espacio libre
        inicio = empacado->files[i].end;
    }

    //Si no se encontró un espacio libre lo suficientemente grande, lo crea al final
    if (inicio + archivo_tamano <= MAX_FILE_SIZE) {

        //Verbose
        verbose("No se encontró un espacio libre lo suficientemente grande, crea uno al final",1);
        verbose("Crea un espacio libre", 2);

        //Crea el espacio libre
        empacado->files[empacado->file_count].start = inicio;
        empacado->files[empacado->file_count].end = inicio + archivo_tamano;
        empacado->file_count++;
    }
}

void delete(const char* package_name, const char* file_name){

    verbose("Inicia opción de eliminar",1);
    verbose("Abre el archivo empacado en modo lectura/escritura binaria",2);

    //Abre el archivo empacado en modo lectura/escritura binaria
    FILE *archivo = fopen(package_name, "rb+");

    if (archivo == NULL) {
        perror("Error al abrir el archivo para lectura/escritura");
        return;
    }

    verbose("Lee la estructura",1);

    struct PackageInfo empacado;
    size_t leidos = fread(&empacado, sizeof(struct PackageInfo), 1, archivo);

    if (leidos != 1) {
        perror("Error al leer el archivo");
        fclose(archivo);
        return;
    }

    verbose("Busca el archivo a borrar",1);
 
    //Busca el archivo a borrar
    int indice_borrar = -1;
    for (size_t i = 0; i < empacado.file_count; i++) {
        if (strcmp(empacado.files[i].name, file_name) == 0) {
            indice_borrar = i;
            break;
        }
    }

    if (indice_borrar == -1) {
        verbose("El archivo no existe en el paquete",1);
        printf("El archivo \"%s\" no existe en el paquete.\n", file_name);
        fclose(archivo);
        return;
    }

    verbose("Actualiza el índice de archivos",2);

    //Actualiza el índice de archivos
    for (size_t i = indice_borrar; i < empacado.file_count - 1; i++) {
        empacado.files[i] = empacado.files[i + 1];
    }
    empacado.file_count--;

    verbose("Mueve el puntero al inicio del paquete y escribe la información actualizada",2);

    //Mueve el puntero al inicio del paquete y escribe la información actualizada
    fseek(archivo, 0, SEEK_SET);
    fwrite(&empacado, sizeof(struct PackageInfo), 1, archivo);

    //Verbose
    verbose("Trunca el archivo para eliminar el espacio del archivo borrado",2);

    //Trunca el archivo para eliminar el espacio del archivo borrado
    long new_size = empacado.files[empacado.file_count - 1].end;
    fclose(archivo);

    FILE* truncated_file = fopen(package_name, "rb+");
    ftruncate(fileno(truncated_file), new_size);
    fclose(truncated_file);

    verbose("El archivo ha sido eliminado del paquete",1);

    printf("El archivo \"%s\" ha sido eliminado del paquete.\n", file_name);

    //Después de eliminar, busca y añade espacios libres
    encontrar_espacios_libres(&empacado, new_size - empacado.files[empacado.file_count - 1].start);
}

void agregar(const char* package_name, const char* file_name) {

    verbose("Inicia opción de agregar",1);
    verbose("Abre el archivo empacado en modo lectura/escritura binaria",2);

    FILE* package = fopen(package_name, "rb+");
    if (package == NULL) {
        perror("No se pudo crear el archivo empacado");
        exit(1);
    }

    verbose("Lee la estructura",1);

    struct PackageInfo empacado;
    size_t leidos = fread(&empacado, sizeof(struct PackageInfo), 1, package);

    if (leidos != 1) {
        perror("Error al leer el archivo");
        fclose(package);
        return;
    }

    verbose("Abre el archivo a agregar en modo lectura binaria",2);

    FILE* file = fopen(file_name, "rb");
    if (file == NULL) {
        perror("No se pudo abrir el archivo");
        exit(1);
    }

    verbose("Obtiene el tamaño del archivo a agregar",2);

    fseek(file, 0, SEEK_END);
    size_t archivo_tamano = ftell(file);
    rewind(file);

    verbose("Verifica si hay suficiente espacio libre para agregar el archivo",2);

    if (archivo_tamano > MAX_FILE_SIZE) {
        printf("El archivo '%s' es demasiado grande para ser agregado.\n", file_name);
        fclose(file);
        fclose(package);
        return;
    }

    verbose("Encuentra espacios libres",1);

    if (empacado.file_count == 0) {
        empacado.files[0].start = sizeof(struct PackageInfo);
        empacado.files[0].end = empacado.files[0].start + archivo_tamano;
        empacado.file_count = 1;
    } else {
        encontrar_espacios_libres(&empacado, archivo_tamano);
    }

    verbose("Obtiene la posición actual del archivo empacado",2);

    fseek(package, 0, SEEK_END);
    long posicion_actual = ftell(package);

    verbose("Obtiene la nueva posición del archivo a agregar",2);

    long nueva_posicion = empacado.files[empacado.file_count - 1].start;

    verbose("Agrega espacio libre si es necesario para el archivo",1);

    if (nueva_posicion > posicion_actual) {
        char buffer[MAX_FILENAME_LENGTH];
        memset(buffer, 0, MAX_FILENAME_LENGTH);
        fwrite(buffer, 1, nueva_posicion - posicion_actual, package);
    }

    verbose("Copia el nombre del archivo a agregar a la estructura",1);

    strncpy(empacado.files[empacado.file_count - 1].name, file_name, MAX_FILENAME_LENGTH);

    verbose("Copia el tamaño del archivo a agregar a la estructura",1);

    empacado.files[empacado.file_count - 1].size = archivo_tamano;

    verbose("Escribe los datos del archivo a agregar al archivo empacado",1);

    char buffer[1024];
    size_t bytes_leidos;
    while ((bytes_leidos = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        fwrite(buffer, 1, bytes_leidos, package);
    }

    verbose("Cierra el archivo a agregar",1);

    fclose(file);

    verbose("Actualiza el archivo empacado",1);

    fseek(package, 0, SEEK_SET);
    fwrite(&empacado, sizeof(struct PackageInfo), 1, package);

    verbose("Cierra el archivo empacado",1);

    fclose(package);

    printf("Archivo agregado con éxito. \n");
}

void actualizar(const char* package_name, const char* file_name) {

    verbose("Inicia opción de actualizar",1);
    verbose("Abre el archivo empacado en modo lectura/escritura binaria",2);

    FILE* package = fopen(package_name, "rb+");
    if (package == NULL) {
        perror("No se pudo abrir el archivo empacado");
        exit(1);
    }

    verbose("Lee la estructura",1);

    struct PackageInfo empacado;
    size_t leidos = fread(&empacado, sizeof(struct PackageInfo), 1, package);

    if (leidos != 1) {
        perror("Error al leer el archivo");
        fclose(package);
        return;
    }

    verbose("Busca el archivo a actualizar",1);

    int indice_actualizar = -1;
    for (size_t i = 0; i < empacado.file_count; i++) {
        if (strcmp(empacado.files[i].name, file_name) == 0) {
            indice_actualizar = i;
            break;
        }
    }

    if (indice_actualizar == -1) {
        verbose("El archivo no existe en el paquete",1);
        printf("El archivo \"%s\" no existe en el paquete.\n", file_name);
        fclose(package);
        return;
    }

    verbose("Obtiene el tamaño del archivo", 2);

    FILE* file = fopen(file_name, "rb");
    if (file == NULL) {
        perror("No se pudo abrir el archivo");
        fclose(package);
        return;
    }

    fseek(file, 0, SEEK_END);
    size_t nuevo_tamano = ftell(file);
    rewind(file);

    verbose("Verifica si el nuevo tamaño es compatible con el espacio existente",2);

    if (nuevo_tamano > empacado.files[indice_actualizar].size) {
        printf("El nuevo archivo es demasiado grande para actualizar el existente.\n");
        fclose(file);
        fclose(package);
        return;
    }

    verbose("Mueve el puntero al inicio del archivo a actualizar en el paquete",2);

    fseek(package, empacado.files[indice_actualizar].start, SEEK_SET);

    verbose("Escribe el contenido del nuevo archivo en el archivo empacado",1);

    char buffer[1024];
    size_t bytes_leidos;
    while ((bytes_leidos = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        fwrite(buffer, 1, bytes_leidos, package);
    }

    verbose("Cierra los archivos",1);
    verbose("Cierra el archivo y el archivo empacado",2);

    fclose(file);
    fclose(package);

    printf("El archivo \"%s\" ha sido actualizado en el paquete.\n", file_name);
    }

void desfragmentar(const char* package_name) {

    verbose("Inicia opción de desfragmentar",1);
    verbose("Abre el archivo empacado en modo lectura/escritura binaria",2);

    FILE* package = fopen(package_name, "rb+");
    if (package == NULL) {
        perror("No se pudo abrir el archivo empacado");
        exit(1);
    }

    verbose("Lee la estructura",1);

    struct PackageInfo empacado;
    size_t leidos = fread(&empacado, sizeof(struct PackageInfo), 1, package);

    if (leidos != 1) {
        perror("Error al leer el archivo");
        fclose(package);
        return;
    }

    verbose("Ordena los archivos por inicio",2);

    for (size_t i = 0; i < empacado.file_count - 1; i++) {
        for (size_t j = i + 1; j < empacado.file_count; j++) {
            if (empacado.files[j].start < empacado.files[i].start) {
                struct File temp = empacado.files[i];
                empacado.files[i] = empacado.files[j];
                empacado.files[j] = temp;
            }
        }
    }

    verbose("Mueve los archivos si es necesario",2);

    long current_byte = sizeof(struct PackageInfo);
    for (size_t i = 0; i < empacado.file_count; i++) {
        if (current_byte != empacado.files[i].start) {

            verbose("Mueve el archivo",1);

            fseek(package, empacado.files[i].start, SEEK_SET);
            char buffer[MAX_FILE_SIZE];
            size_t bytes_read = fread(buffer, 1, empacado.files[i].size, package);
            fseek(package, current_byte, SEEK_SET);
            fwrite(buffer, 1, bytes_read, package);

            empacado.files[i].start = current_byte;
            empacado.files[i].end = current_byte + empacado.files[i].size;
        }

        current_byte = empacado.files[i].end;
    }

    verbose("Actualiza la información de los archivos en el paquete",1);

    fseek(package, 0, SEEK_SET);
    fwrite(&empacado, sizeof(struct PackageInfo), 1, package);

    printf("Paquete desfragmentado\n");

    verbose("Cierra el archivo empacado",1);

    fclose(package);
}

int main(int argc, char *argv[]) {
if (argc < 3) {
        printf("Uso: %s <comando> <archivoEmpacado> [archivos...]\n", argv[0]);
        return 1;
    }

    const char* comando = argv[1];
    long int longitud_comando = strlen(comando)-1;
    const char* package_name = argv[2];

    const char* archivos = argv[3];
    for (int i = 1; i <= longitud_comando; i++){
        if (comando[i] == 'c'){
            create_flag = true;
        }
        else if (comando[i] =='x'){
            extract_flag = true;
        }
        else if(comando[i] == 't'){
            list_flag = true;
        }
        else if(comando[i] == 'd'){
            delete_flag = true;
        }
        else if(comando[i] == 'u'){
            update_flag = true;
        }
        else if(comando[i] == 'v'){
            if(verbose_flag == true){
                vverbose_flag = true;
                verbose_flag = false;
            }
            verbose_flag = true;
        }
        else if(comando[i] == 'f'){
            file_flag = true;
        }
        else if(comando[i] == 'r'){ //append
            append_flag = true;
        }
        else if(comando[i] == 'p'){
            pack_flag = true;
        }
        else{
            printf("La letra ingresada no es valida");
        }
    }
        //Revisa que hay que hacer
        if (create_flag) {
            if (file_flag == false) {
                printf("Es necesario especificar el archivo con el comando -f.\n");
                return 0;
            }
            else{
            crear(package_name, &argv[3], argc - 3);
        }
        }
        
        if (delete_flag) {
            if (file_flag == false) {
                printf("Es necesario especificar el archivo con el comando -f.\n");
                return 0;
            }
            delete(package_name, argv[3]);
        }
        if (extract_flag) {
            if (!file_flag) {
                printf("Es necesario especificar el archivo con el comando -f.\n");
                return 0;
            }
            struct PackageInfo archivo;
            extraer(&archivo, package_name );
        }
        if (append_flag) {
            if (!file_flag) {
                printf("Es necesario especificar el archivo con el comando -f.\n");
                return 0;
            }
            agregar(package_name, argv[3]);
        }
        if (update_flag) {
            if (!file_flag) {
                printf("Es necesario especificar el archivo con el comando -f.\n");
                return 0;
            }
            actualizar(package_name, argv[3]);
        }
        if (list_flag) {
            if (!file_flag) {
                printf("Es necesario especificar el archivo con el comando -f.\n");
                return 0;
            }
            struct PackageInfo archivo;
            listar(&archivo, package_name);
        }
        if (pack_flag) {
            if (!file_flag) {
                printf("Es necesario especificar el archivo con el comando -f.\n");
                return 0;
            }
            desfragmentar(package_name);
        }
        
    return 0;
}