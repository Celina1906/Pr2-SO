/*
   Proyecto 2
   ...........................................
   Tecnologico de Costa Rica - CTCC
   IC660 Principios Sistemas Operativos
   Prof. Armando Arce
   Estudiantes:
        Andres Arias Siles - 2019157553
        Johnny Diaz Coto   - 2020042119
   I Semestre, 2023
   ...........................................
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

//  Variables globales
// --------------------
#define BLOCK_SIZE     32768    // Bloque de datos (32K)
#define FAT_SIZE       256
#define DIRECTORY_SIZE 256
#define MAX_DATA_BLOCK 256
#define MAX_NAME_LEN   256

//  Definicion de Estructuras
// ---------------------------
typedef struct {
    char  file[MAX_NAME_LEN];
    int   first;
    int   size;
} DIRECTORY;             // Estructura de la tabla directorio

typedef struct {
    int id;
    int next;
} FAT;                   // Estructura de la File-Allocation Table

typedef struct {
    int blockId;
    int data[BLOCK_SIZE];
} BLOCK;                   // Estructura de los Bloques en Disco

//   Definicion de Variables Globales
// ------------------------------------
DIRECTORY directoryTable[DIRECTORY_SIZE];
FAT       fatTable[FAT_SIZE];
BLOCK     dataBlocks[MAX_DATA_BLOCK];

//  Instanciar Tablas
// -------------------
void instanciarDirectorio(){
    for (int i=0; i<DIRECTORY_SIZE; i++){  
        memset(directoryTable[i].file, '\0', MAX_NAME_LEN);  
        directoryTable[i].first = -1;
        directoryTable[i].size =  -1;
    }
}

void instanciarFat(){
    for (int i=0; i<FAT_SIZE; i++){    
        fatTable[i].id   =  i;
        fatTable[i].next = -1;
    }
}

void instanciarData(){
    for (int i=0; i<MAX_DATA_BLOCK; i++){    
        dataBlocks[i].blockId =  i;
    }
}

//  Recorridos en FAT
// -------------------

// Retorna el primero que se encuentre, asi sea el mismo
int getBloqueDisponible(){
    for (int i=0; i < FAT_SIZE; i++){
        if (fatTable[i].next == -1){
            return fatTable[i].id;
        }
    }
    return -1;
}

// Retorna el sig bloque disponible
int getSigBloqueDisponible(int bloque){
    for (int i=0; i < FAT_SIZE; i++){
        if (fatTable[i].next == -1 && fatTable[i].id != bloque){
            return fatTable[i].id;
        }
    }
    return -1;
}

//  Comprimir archivos en TAR
// ---------------------------
int comprimir(const char* nombre, const char* archivos[], int numArchs) {

    printf("Comprimir %i archivos\n",numArchs);

    FILE* tar = fopen(nombre, "wb");              // Se abre como binario

    if (tar == NULL) {                            // Control de errores
        printf("No se pudo crear el tar\n");
        return -1;
    }

    // Se inicializa la tabla de directorio
    instanciarDirectorio();
    
    // Se inicializa File-Addresing Table
    instanciarFat();

    int contadorBloque = 0;                                     // Contador del bloque
    int contadorArchivos = 0;

    //printf("\n -> Iniciando compresion\n", nombre);
    printf("\n -> Iniciando compresion\n");

    // Itera sobre cada archivo
    for (int i = 0; i < numArchs; i++) {        

        // Ya no hay mas espacio
        if (i >= DIRECTORY_SIZE){
            printf("Capacidad max de archivos alcanzada!");
            break;
        }               

        // Abre un archivo por comprimir, en binario
        FILE* aFile = fopen(archivos[i], "rb");

        // Control de Errores
        if (aFile == NULL) {
            printf("Hubo un problema al procesar %s\n", archivos[i]);
            continue;                                           // Si hubo un error, continua empacando el siguiente archivo
        }

        // Obtener la cantidad de bytes del archivo comprimiendo
        fseek(aFile, 0, SEEK_END);                  // Mueve el puntero al final
        long lenArchivo = ftell(aFile);             // Le indica la posicion en la que esta
        fseek(aFile, 0, SEEK_SET);                  // Regresa el puntero al inicio

        // Calcular el numero de bloques necesarios para el archivo de entrada
        int numBlocks = (lenArchivo + BLOCK_SIZE - 1) / BLOCK_SIZE;

        // Guarda la informacion del archivo, inicializa entrada del directorio
        strcpy(directoryTable[contadorArchivos].file, archivos[i]);     // nombre del archivo                
        directoryTable[contadorArchivos].size = numBlocks;             //  size

        // Buscar un bloque libre en la tabla FAT para almacenar el archivo de entrada
        contadorBloque = getBloqueDisponible();
        if(contadorBloque == -1) {
            printf("Sin espacio disponible!");
            fclose(aFile);
            break;                  // Ya no queda espacio
        }

        // Para este punto, ya tiene el bloque del FAT
        directoryTable[contadorArchivos].first = contadorBloque;

        printf("    procesando %s a partir del bloque %i\n", archivos[i], contadorBloque);

        // Escribir el contenido del archivo de entrada en los bloques de datos
        for (int j = 0; j < numBlocks; j++) {

            // Actualizar la tabla FAT
            if(j == (numBlocks - 1)){
                fatTable[contadorBloque].next = 0;     // Ya se incluyo todo el archivo.
            }else{
                int temp_block = getSigBloqueDisponible(contadorBloque);
                if (temp_block == -1){
                    printf("Sin espacio disponible!");
                    fclose(aFile);
                    break;                          // Se agoto el espacio
                }
                fatTable[contadorBloque].next = temp_block;
                //printf("(cont: %i temp: %i)",contadorBloque, temp_block);
                contadorBloque = fatTable[contadorBloque].next;
            }
            
            // Leer el bloque de datos del archivo de entrada

            BLOCK unBloque;
            unBloque.blockId = contadorBloque;
            memset(unBloque.data, '\0', BLOCK_SIZE); // Inicializa un bloque de 32K
            size_t bytesRead = fread(unBloque.data, 1, BLOCK_SIZE, aFile);

            // Escribir el bloque de datos en el archivo tar
            dataBlocks[contadorBloque] = unBloque;
            fwrite(dataBlocks[contadorBloque].data, BLOCK_SIZE, 1, tar); // cambio

        }

        contadorArchivos++;
        // Cerrar el archivo de entrada
        fclose(aFile);
    }

    // Escribir la tabla FAT al final del archivo tar
    fseek(tar, 0, SEEK_END);
    fwrite(&fatTable, sizeof(fatTable), 1, tar);

    // Escribir el directorio al final del archivo tar
    fseek(tar, 0, SEEK_END);
    fwrite(&directoryTable, sizeof(directoryTable), 1, tar);

    // Cerrar el archivo tar
    fclose(tar);

    printf(" -> %s creado!\n---------\n", nombre);
    return 0;
}

//  Descomprimir archivos en TAR
// ---------------------------
void descomprimir(const char* filename) {
    
    // Se abre como binario
    FILE* tarFile = fopen(filename, "rb");

    // Control de errores
    if (tarFile == NULL) {
        printf("No se pudo leer el tar\n");
        return;
    }

    printf("\n -> Descomprimiendo %s!\n", filename);

    instanciarFat();
    instanciarDirectorio();
    instanciarData();

    // Posiciona el cursor para leer el directorio
    // Crea una estructura auxiliar, para ser usada en la lectura y recrea el directorio
    fseek(tarFile, -sizeof(directoryTable), SEEK_END);
    DIRECTORY rDir;
    int d = 0;
    while (
        fread(&rDir, sizeof(DIRECTORY), 1, tarFile)){
        directoryTable[d] = rDir;
        d += 1;
    }

    // Posiciona el cursor para leer la tabla FAT
    // Crea una estructura de fat auxiliar, para ser usada en la lectura y recrea la tabla Fat
    fseek(tarFile, -sizeof(directoryTable)-sizeof(fatTable), SEEK_END);    //-sizeof(directoryTable)
    FAT rFat;
    int contFat = 0;
    while (contFat < FAT_SIZE){
        fread(&rFat, sizeof(FAT), 1, tarFile);
        fatTable[rFat.id] = rFat;
        contFat++;
    };

    // Devuelve el puntero al inicio, porque esta al final
    // Ahora lo manda justo antes de iniciar con las tablas
    // Obtiene el desplazamiento
    // Se devuelve al inicio para cargar los datos
    fseek(tarFile, 0, SEEK_SET);                  
    fseek(tarFile, -sizeof(directoryTable)-sizeof(fatTable), SEEK_END); 
    long dataSize = ftell(tarFile);               
    fseek(tarFile, 0, SEEK_SET);
    
    BLOCK aBlock;
    int b = 0;
    while (b<contFat){
        fread(&aBlock.data, BLOCK_SIZE, 1, tarFile);
        dataBlocks[b] = aBlock;
        dataBlocks[b].blockId = b;
        //printf("%i",dataBlocks[b].blockId );
        b += 1;
    };
    
    // Visita el directorio
    for (int vdc=0; vdc < DIRECTORY_SIZE; vdc++){
        if (directoryTable[vdc].file[0] == '\0')
        {
           continue;
        }

        printf("\n(%s | ",directoryTable[vdc].file);
        printf("%i | ",directoryTable[vdc].first);
        printf("%i ) { ",directoryTable[vdc].size);

        FILE* outputFile = fopen(directoryTable[vdc].file, "wb");
        
        int i = directoryTable[vdc].first;
        //printf("i:%i - ", i);
        while (fatTable[i].next != -1){
            fwrite(&dataBlocks[i].data, BLOCK_SIZE, 1, outputFile);
            if (fatTable[i].next==0){
                printf("(%i | ",fatTable[i].id);
                printf("%i) }\n",fatTable[i].next);
                break;

            }else{
                printf("(%i | ",fatTable[i].id);
                printf("%i) -->",fatTable[i].next);
                i = fatTable[i].next;
            }

            //size_t bytesRead = fread(dataBlocks[i], 1, BLOCK_SIZE, tarFile);        
        }
        
        fclose(outputFile);   
    }

    printf("\n -> Se ha extraido el archivo!\n---------\n");
}

void listarArchivos(const char* filename) {
    
    // Se abre como binario
    FILE* tarFile = fopen(filename, "rb");

    // Control de errores
    if (tarFile == NULL) {
        printf("No se pudo leer el tar\n");
        return;
    }

    instanciarDirectorio();

    // Posiciona el cursor para leer el directorio
    // Crea una estructura auxiliar, para ser usada en la lectura y recrea el directorio
    fseek(tarFile, -sizeof(directoryTable), SEEK_END);
    DIRECTORY rDir;
    int d = 0;
    while (
        fread(&rDir, sizeof(DIRECTORY), 1, tarFile)){
        directoryTable[d] = rDir;
        d += 1;
    }

    fclose(tarFile);

    for (int vdc=0; vdc < DIRECTORY_SIZE; vdc++){
        if (directoryTable[vdc].file[0] == '\0')
        {
           continue;
        }
        
        printf("{ %s | ",directoryTable[vdc].file);
        printf("%i bloques } \n",directoryTable[vdc].size);
    
    }

}
char** obtenerListaNombresArchivos(const char* filename) {
    FILE* tarFile = fopen(filename, "rb");
    if (tarFile == NULL) {
        printf("No se pudo leer el tar\n");
        return NULL;
    }

    instanciarDirectorio();

    fseek(tarFile, -sizeof(directoryTable), SEEK_END);
    DIRECTORY rDir;
    int d = 0;
    while (fread(&rDir, sizeof(DIRECTORY), 1, tarFile)) {
        directoryTable[d] = rDir;
        d += 1;
    }

    fclose(tarFile);

    // Crear un arreglo dinámico para almacenar los nombres de los archivos
    char** listaNombres = (char**)malloc((d + 1) * sizeof(char*));

    for (int vdc = 0; directoryTable[vdc].first != -1; vdc++) {
        // Copiar el nombre del archivo a una nueva cadena en la lista
        listaNombres[vdc] = strdup(directoryTable[vdc].file);
    }

    // Agregar NULL al final del arreglo para indicar el final de la lista
    listaNombres[d] = NULL;

    return listaNombres;
}

void delete(const char* filename, const char* archivo) {

    // Se abre como binario
    FILE* tarFile = fopen(filename, "rb+");

    // Control de errores
    if (tarFile == NULL) {
        printf("No se pudo leer el tar\n");
        return;
    }

    printf("\n -> eliminando %s!\n", archivo);

    instanciarFat();
    instanciarDirectorio();
    instanciarData();

    // Posiciona el cursor para leer el directorio
    // Crea una estructura auxiliar, para ser usada en la lectura y recrea el directorio
    fseek(tarFile, -sizeof(directoryTable), SEEK_END);
    DIRECTORY rDir;
    int d = 0;
    while (
        fread(&rDir, sizeof(DIRECTORY), 1, tarFile)){
        directoryTable[d] = rDir;
        d += 1;
    }

    // Posiciona el cursor para leer la tabla FAT
    // Crea una estructura de fat auxiliar, para ser usada en la lectura y recrea la tabla Fat
    fseek(tarFile, -sizeof(directoryTable)-sizeof(fatTable), SEEK_END);    //-sizeof(directoryTable)
    FAT rFat;
    int contFat = 0;
    while (contFat < FAT_SIZE){
        fread(&rFat, sizeof(FAT), 1, tarFile);
        fatTable[rFat.id] = rFat;
        contFat++;
    };


    d=0;
    int first = 0;
    while (fread(&rDir, sizeof(DIRECTORY), 1, tarFile))
    {
        if (strcmp(directoryTable[d].file, archivo) == 0)
        {
            first = directoryTable[d].first;
            break;
        }
        d += 1;
        
    }


for (int i = 0; i < FAT_SIZE; i++)
    {
        printf("(%i | ",fatTable[i].id);
        printf("%i)\n",fatTable[i].next);
    }
   contFat = first;

   int actual = 0;
    // Ciclo para eliminar dentro del FAT los enlaces a los siguientes bloques    
   while (fatTable[contFat].next != 0)
   {
    actual = contFat;
    contFat = fatTable[contFat].next;
    fatTable[actual].next = -1;
    
   }

    // Debido a la condicion, se elimina el ultimo bloque porque dentro del while no se elimina ese enlace
   fatTable[contFat].next = -1;
   





    d=0;
    while (fread(&rDir, sizeof(DIRECTORY), 1, tarFile))
    {
        if (strcmp(directoryTable[d].file, archivo) == 0)
        {
            for (int i = 0; i < MAX_NAME_LEN; i++) {
            directoryTable[d].file[i] = '\0';
            }

            directoryTable[d].first = -1;
            directoryTable[d].size = -1;
            break;
        }
        d += 1;
        
    }

    printf("\n\nFAT table despues de eliminar: \n\n");
    for (int i = 0; i < FAT_SIZE; i++)
    {
        printf("(%i | ",fatTable[i].id);
        printf("%i)\n",fatTable[i].next);
    }

    // Escribir la tabla FAT al final del archivo tar
    fseek(tarFile, -sizeof(directoryTable) - sizeof(fatTable), SEEK_END);
    fwrite(&fatTable, sizeof(fatTable), 1, tarFile);

    // Escribir el directorio en el mismo lugar desde donde se leyó
    fseek(tarFile, -sizeof(directoryTable), SEEK_END);
    fwrite(&directoryTable, sizeof(directoryTable), 1, tarFile);

fclose(tarFile);

}

void pack(const char* filename){
    // Se abre como binario
    FILE* tarFile = fopen(filename, "rb");

    // Control de errores
    if (tarFile == NULL) {
        printf("No se pudo leer el tar\n");
        return;
    }

    instanciarDirectorio();

    // Posiciona el cursor para leer el directorio
    // Crea una estructura auxiliar, para ser usada en la lectura y recrea el directorio
    fseek(tarFile, -sizeof(directoryTable), SEEK_END);
    DIRECTORY rDir;
    int d = 0;
    while (fread(&rDir, sizeof(DIRECTORY), 1, tarFile)){
        directoryTable[d] = rDir;
        d += 1;
    }

    fclose(tarFile);
}

int checkFlags(int argc, char* argv[], const char** tar_filename) {
    int option, next;
    int create_flag = 0;
    int extract_flag = 0;
    int list_flag = 0;
    int delete_flag = 0;
    int update_flag = 0;
    int verbose_flag = 0;
    int super_verbose_flag = 0;
    int file_flag = 0;
    int append_flag = 0;
    int pack_flag = 0;

    while ((option = getopt(argc, argv, "cxtdurvfrp")) != -1) {
        switch (option) {
            case 'c':
                create_flag = 1;
                break;
            case 'x':
                extract_flag = 1;
                break;
            case 't':
                list_flag = 1;
                break;
            case 'd':
                delete_flag = 1;
                break;
            case 'u':
                update_flag = 1;
                break;
            case 'v':
                if (verbose_flag == 1) {
                    super_verbose_flag = 1;
                    verbose_flag = 0;
                    break;
                }
                verbose_flag = 1;
                break;
            case 'f':
                file_flag = 1;
                break;
            case 'r':
                append_flag = 1;
                break;
            case 'p':
                pack_flag = 1;
                break;
            default:
                printf("Opción inválida\n");
                exit(EXIT_FAILURE);
        }
    }

    // Realiza las operaciones según las flags establecidas
    if (create_flag) {
        if (!file_flag) {
            printf("Debe especificar un archivo tar con la opción -f.\n");
            return 0;
        }
        
        // Obtener los valores de los argumentos
        const char* archivo_tar = argv[2];
        const char* archivos[argc-3];
        int cantidad = 0;

        for (int i = 0; i < argc - 3; i++) {
            archivos[i] = argv[i+3];
            cantidad += 1;
        }

        comprimir(archivo_tar, archivos, cantidad);
    }
    if (extract_flag) {
        if (!file_flag) {
            printf("Debe especificar un archivo tar con la opción -f.\n");
            return 0;
        }
        const char* archivo_tar = argv[2];
        descomprimir(archivo_tar);
    }
    if (list_flag) {
        if (!file_flag) {
            printf("Debe especificar un archivo tar con la opción -f.\n");
            return 0;
        }
        const char* archivo_tar = argv[2];
        listarArchivos(archivo_tar);
    }
    if (delete_flag) {
        if (!file_flag) {
            printf("Debe especificar un archivo tar con la opción -f.\n");
            return 0;
        }

        char** nombresArchivos = obtenerListaNombresArchivos(argv[2]);
        if (nombresArchivos == NULL) {
            printf("Error al obtener la lista de nombres de archivos\n");
            return 1;
        }

        // Obtener los valores de los argumentos
        const char* archivo_tar = argv[2];
        const char* archivos[argc-3];
        int cantidad = 0;

        for (int i = 0; i < argc - 3; i++) {
            archivos[i] = argv[i+3];
            cantidad += 1;
        }
        delete(argv[2], argv[3]);
    }
    if (update_flag) {
        if (!file_flag) {
            printf("Debe especificar un archivo tar con la opción -f.\n");
            return 0;
        }
        printf("Realizando la operación de actualización\n");
    }
    if (verbose_flag) {
        printf("Habilitada la opción de reporte detallado\n");
    }
    if (super_verbose_flag) {
        printf("Habilitada la opción de reporte super detallado\n");
    }
    if (file_flag) {
        printf("Utilizando archivo de entrada: %s\n", *tar_filename);
    }
    if (append_flag) {
        printf("Realizando la operación de agregado\n");
    }
    if (pack_flag) {
        printf("Realizando la operación de desfragmentación\n");
    }

    return 0;
}

int main(int argc, char* argv[]) {
    int i;
    
    // Verificar si se proporcionaron suficientes argumentos
    if (argc < 3) {
        printf("Uso: tar <opciones> <archivoSalida> <archivo1> <archivo2> ... <archivoN>\n");
        return 1;
    }
    const char* tar_filename = argv[2]; //NULL;

    checkFlags(argc, argv, &tar_filename);
    
    return 0;
}