#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_STR 50
#define FILE_INVENTARIO "vehiculos.txt"
#define FILE_VENTAS "ventas.txt"

// ==========================================
// A. CAPA DE MODELADO DE DATOS (ESTRUCTURAS)
// ==========================================
typedef struct {
    int id;
    char tipo[MAX_STR];     // Camioneta, Sedan, etc.
    char marca[MAX_STR];    // Chevrolet, Toyota, etc.
    char modelo[MAX_STR];
    char estado[MAX_STR];    // Nuevo, Usado
    float precio;
    int disponible;         // 1 = Disponible, 0 = No disponible
} Vehiculo;

typedef struct {
    int idVenta;
    int idVehiculo;
    char clienteNombre[MAX_STR];
    int clienteEdad;
    char vendedorNombre[MAX_STR];
    float montoFinal;
} Venta;

// ==========================================
// B. CAPA DE VALIDACIONES ESTRICTAS
// ==========================================

// REGLA: Solo acepta letras y espacios. Rechaza numeros y simbolos.
void leerCadenaLetras(char* destino, int longitud) {
    int valido;
    do {
        valido = 1;
        if (fgets(destino, longitud, stdin) != NULL) {
            // Eliminar el salto de línea generado por fgets
            destino[strcspn(destino, "\n")] = '\0';
            
            if (strlen(destino) == 0) {
                valido = 0;
            }
            
            // Validar carácter por carácter
            for (int i = 0; destino[i] != '\0'; i++) {
                if (!isalpha(destino[i]) && destino[i] != ' ') {
                    valido = 0; // Se detectó un número o símbolo
                    break;
                }
            }
        } else {
            valido = 0;
        }

        if (!valido) {
            printf("[ERROR] Entrada invalida. No use numeros ni simbolos. Intente de nuevo: ");
        }
    } while (!valido);
}

// REGLA: Solo acepta números enteros. Si ingresan letras, limpia el búfer.
int leerEntero() {
    int valor;
    char c;
    // Patrón de lectura combinada de control de flujo
    while (scanf("%d%c", &valor, &c) != 2 || c != '\n') {
        // Ciclo getchar() para vaciar y limpiar el búfer del teclado
        while ((c = getchar()) != '\n' && c != EOF);
        printf("[ERROR] Ingrese unicamente un numero entero valido: ");
    }
    return valor;
}

// REGLA: Solo acepta números decimales para precios o presupuestos.
float leerFloat() {
    float valor;
    char c;
    while (scanf("%f%c", &valor, &c) != 2 || c != '\n') {
        while ((c = getchar()) != '\n' && c != EOF);
        printf("[ERROR] Ingrese unicamente un valor numerico decimal: ");
    }
    return valor;
}

// ==========================================
// C. CAPA DE PERSISTENCIA (MANEJO DE ARCHIVOS)
// ==========================================

// Inicializa el archivo txt usando modo "w" y fprintf()
void inicializarInventarioPredefinido() {
    FILE *F = fopen(FILE_INVENTARIO, "r"); // Modo "r": lectura
    if (F != NULL) {
        fclose(F); // Si el archivo ya existe, se cierra formalmente
        return; 
    }

    F = fopen(FILE_INVENTARIO, "w"); // Modo "w": crea para escritura
    if (F == NULL) return;

    // Se guardan los datos iniciales estructurados con punto y coma usando fprintf
    fprintf(F, "1;Camioneta;Chevrolet;D-Max;Usado;13500.00;1\n");
    fprintf(F, "2;Camioneta;Chevrolet;Colorado;Usado;16000.00;1\n");
    fprintf(F, "3;Camioneta;Chevrolet;Luv;Usado;11200.00;1\n");
    fprintf(F, "4;Sedan;Chevrolet;Sail;Nuevo;15000.00;1\n");
    fprintf(F, "5;Camioneta;Toyota;Hilux;Usado;14000.00;1\n");
    
    fclose(F); // Cierre formal a nivel de Sistema Operativo
}

// Muestra el inventario completo leyendo con fscanf() desde el archivo plano
void mostrarInventario() {
    Vehiculo v;
    FILE *F = fopen(FILE_INVENTARIO, "r");
    if (F == NULL) {
        printf("No se encontro el archivo de inventario.\n");
        return;
    }

    printf("\n--- INVENTARIO DE VEHICULOS (PERSISTENCIA TXT) ---\n");
    printf("%-4s %-12s %-12s %-12s %-8s %-10s %-14s\n", "ID", "Tipo", "Marca", "Modelo", "Estado", "Precio", "Disponibilidad");
    printf("-----------------------------------------------------------------------------------\n");

    // Uso de fscanf() para archivos de texto plano
    while (fscanf(F, "%d;%[^;];%[^;];%[^;];%[^;];%f;%d\n", 
           &v.id, v.tipo, v.marca, v.modelo, v.estado, &v.precio, &v.disponible) != EOF) {
        
        printf("%-4d %-12s %-12s %-12s %-8s $%-9.2f %-14s\n", 
               v.id, v.tipo, v.marca, v.modelo, v.estado, v.precio, 
               v.disponible ? "Disponible" : "No disponible");
    }
    fclose(F);
}

// Filtra buscando coincidencias en texto plano mediante fscanf()
void buscarVehiculosParaCliente(const char* tipo, const char* marca, float presupuestoMax) {
    Vehiculo v;
    FILE *F = fopen(FILE_INVENTARIO, "r");
    if (F == NULL) return;

    int encontrados = 0;

    printf("\n--- RESULTADOS DE BUSQUEDA PARA EL CLIENTE (Max: $%.2f) ---\n", presupuestoMax);
    printf("%-4s %-12s %-12s %-12s %-8s %-10s\n", "ID", "Tipo", "Marca", "Modelo", "Estado", "Precio");
    printf("----------------------------------------------------------------------\n");

    while (fscanf(F, "%d;%[^;];%[^;];%[^;];%[^;];%f;%d\n", 
           &v.id, v.tipo, v.marca, v.modelo, v.estado, &v.precio, &v.disponible) != EOF) {
        
        if (strcasecmp(v.tipo, tipo) == 0 && 
            strcasecmp(v.marca, marca) == 0 && 
            v.precio <= presupuestoMax && 
            v.disponible == 1) {
            
            printf("%-4d %-12s %-12s %-12s %-8s $%-9.2f\n", 
                   v.id, v.tipo, v.marca, v.modelo, v.estado, v.precio);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("No se encontraron opciones disponibles que cumplan los criterios.\n");
    }
    fclose(F);
}

// Procesa la transacción reescribiendo con "w" y añadiendo al histórico con "a"
void registrarVenta(int idVehiculo, const char* cliente, int edad, const char* vendedor) {
    FILE *F = fopen(FILE_INVENTARIO, "r");
    if (F == NULL) return;

    Vehiculo inventario[100];
    int totalVehiculos = 0;
    int encontrado = 0;
    float montoVenta = 0;

    // 1. Leer registros existentes hacia la memoria transitoria
    while (fscanf(F, "%d;%[^;];%[^;];%[^;];%[^;];%f;%d\n", 
           &inventario[totalVehiculos].id, inventario[totalVehiculos].tipo, 
           inventario[totalVehiculos].marca, inventario[totalVehiculos].modelo, 
           inventario[totalVehiculos].estado, &inventario[totalVehiculos].precio, 
           &inventario[totalVehiculos].disponible) != EOF) {
        
        if (inventario[totalVehiculos].id == idVehiculo) {
            if (inventario[totalVehiculos].disponible == 0) {
                printf("\n[ERROR] Operacion denegada. El vehiculo ya fue vendido.\n");
                fclose(F);
                return;
            }
            inventario[totalVehiculos].disponible = 0; // Modificación de bandera
            montoVenta = inventario[totalVehiculos].precio;
            encontrado = 1;
        }
        totalVehiculos++;
    }
    fclose(F);

    if (!encontrado) {
        printf("\n[ERROR] El ID especificado no pertenece al inventario.\n");
        return;
    }

    // 2. Sobrescribir el archivo original usando el modo "w"
    F = fopen(FILE_INVENTARIO, "w");
    for (int i = 0; i < totalVehiculos; i++) {
        fprintf(F, "%d;%s;%s;%s;%s;%.2f;%d\n", 
                inventario[i].id, inventario[i].tipo, inventario[i].marca, 
                inventario[i].modelo, inventario[i].estado, inventario[i].precio, 
                inventario[i].disponible);
    }
    fclose(F);

    // 3. Registrar auditoría en ventas.txt usando el modo "a" (Añadir)
    FILE *FVentas = fopen(FILE_VENTAS, "a");
    if (FVentas != NULL) {
        int idVenta = rand() % 9000 + 1000;
        fprintf(FVentas, "%d;%d;%s;%d;%s;%.2f\n", 
                idVenta, idVehiculo, cliente, edad, vendedor, montoVenta);
        fclose(FVentas);

        printf("\n=============================================\n");
        printf("¡VENTA REGISTRADA EXITOSAMENTE EN ARCHIVO TXT!\n");
        printf("Ticket Auditoria: %d\n", idVenta);
        printf("Cliente: %s (%d anos) | Asesor: %s\n", cliente, edad, vendedor);
        printf("Monto Final Recaudado: $%.2f\n", montoVenta);
        printf("=============================================\n");
    }
}

// ==========================================
// D. FLUJO PRINCIPAL INTERACTIVO
// ==========================================
int main() {
    srand(time(NULL));
    inicializarInventarioPredefinido();

    int opcion;
    int idSeleccionado;

    do {
        printf("\n===== SISTEMA SGIC - CONCESIONARIA RUEDAS DE ORO =====\n");
        printf("1. Ver Inventario Completo (`vehiculos.txt`)\n");
        printf("2. Ejecutar Simulación del Caso (Rubén y José)\n");
        printf("3. Salir del Sistema\n");
        printf("Seleccione una opcion: ");
        opcion = leerEntero(); // Validación estricta contra caracteres de menú

        switch (opcion) {
            case 1:
                mostrarInventario();
                break;
            case 2:
                printf("\n--- CASO DE PRUEBA: Busqueda Orientada ---\n");
                buscarVehiculosParaCliente("Camioneta", "Chevrolet", 14000.00);

                printf("\nSeleccione el ID del vehiculo para concretar la venta (0 para cancelar): ");
                idSeleccionado = leerEntero();

                if (idSeleccionado != 0) {
                    registrarVenta(idSeleccionado, "Ruben", 56, "Jose");
                } else {
                    printf("Venta cancelada.\n");
                }
                break;
            case 3:
                printf("Cerrando sesión en el SGIC de la Concesionaria. ¡Adiós!\n");
                break;
            default:
                printf("[ERROR] Opción fuera de rango (1-3).\n");
        }
    } while (opcion != 3);

    return 0;
}