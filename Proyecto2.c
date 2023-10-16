#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estructura para almacenar información de archivos en el paquete
struct FileEntry {
    char filename[100];
    long start_byte;
    long end_byte;
};

// Función para crear un nuevo archivo empacado
void create_package(const char* package_name, const char** files, int file_count) {
    FILE* package = fopen(package_name, "wb");
    if (package == NULL) {
        perror("No se pudo crear el archivo empacado");
        exit(1);
    }

    struct FileEntry* entries = (struct FileEntry*)malloc(file_count * sizeof(struct FileEntry));

    if (entries == NULL) {
        perror("Error de asignación de memoria");
        fclose(package);
        exit(1);
    }

    long current_byte = 0;

    for (int i = 0; i < file_count; i++) {
        FILE* file = fopen(files[i], "rb");
        if (file == NULL) {
            perror("No se pudo abrir el archivo");
            exit(1);
        }

        entries[i].start_byte = current_byte;
        fseek(file, 0, SEEK_END);
        entries[i].end_byte = current_byte + ftell(file);
        current_byte = entries[i].end_byte;
        rewind(file);

        strncpy(entries[i].filename, files[i], 100);

        // Escribir la información del archivo en el archivo empacado
        fwrite(&entries[i], sizeof(struct FileEntry), 1, package);

        // Copiar el contenido del archivo al archivo empacado
        char buffer[1024];
        size_t bytes_read;
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
            fwrite(buffer, 1, bytes_read, package);
        }

        fclose(file);
    }

    free(entries);
    fclose(package);
}

// Función para listar el contenido del archivo empacado
void list_package(const char* package_name) {
    FILE* package = fopen(package_name, "rb");
    if (package == NULL) {
        perror("No se pudo abrir el archivo empacado");
        exit(1);
    }

    struct FileEntry entry;

    while (fread(&entry, sizeof(struct FileEntry), 1, package) > 0) {
        printf("Nombre: %s, Tamaño: %ld bytes\n", entry.filename, entry.end_byte - entry.start_byte);
    }

    fclose(package);
}

// Función para extraer archivos de un archivo empacado
void extract_files(const char* package_name) {
    FILE* package = fopen(package_name, "rb");
    if (package == NULL) {
        perror("No se pudo abrir el archivo empacado");
        exit(1);
    }

    struct FileEntry entry;

    while (fread(&entry, sizeof(struct FileEntry), 1, package) > 0) {
        FILE* file = fopen(entry.filename, "wb");
        if (file == NULL) {
            perror("No se pudo crear el archivo");
            exit(1);
        }

        fseek(package, entry.start_byte, SEEK_SET);
        char buffer[1024];
        size_t bytes_read;
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), package) > 0) {
            fwrite(buffer, 1, bytes_read, file);
        }

        fclose(file);
    }

    fclose(package);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: %s <comando> <archivoEmpacado> [archivos...]\n", argv[0]);
        return 1;
    }

    const char* comando = argv[1];
    const char* package_name = argv[2];

    if (strcmp(comando, "-c") == 0) {
        if (argc < 4) {
            printf("Uso: %s -c <archivoEmpacado> [archivos...]\n", argv[0]);
            return 1;
        }
        create_package(package_name, &argv[3], argc - 3);
    } else if (strcmp(comando, "-x") == 0) {
        extract_files(package_name);
    } else if (strcmp(comando, "-t") == 0) {
        list_package(package_name);
    } else {
        printf("Comando no válido: %s\n", comando);
        return 1;
    }

    return 0;
}
