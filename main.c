#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char marca[30];
    char tipo[20];    
    char estado[10]; 
    float precio;
    int disponible;   
} Vehiculo;
typedef struct {
    char nombreCliente[50];
    int edadCliente;
    char marcaVehiculo[30];
    float precioVenta;
} Venta;
void leerCadenaLetras(char *destino, int longitud) {
    int valido;
    do {
        valido = 1;
        fgets(destino, longitud, stdin);
        destino[strcspn(destino, "\n")] = '\0';
        if (strlen(destino) == 0) {
            printf("Error: El campo no puede estar vacio. Intente de nuevo: ");
            valido = 0;
            continue;
        }
        for (int i = 0; destino[i] != '\0'; i++) {
            if (!isalpha(destino[i]) && destino[i] != ' ') {
                printf("Error: Solo se permiten letras y espacios. Intente de nuevo: ");
                valido = 0;
                break;
            }
        }
    } while (!valido);
}
int leerEntero() {
    int valor;
    char descarte;
    while (scanf("%d%c", &valor, &descarte) != 2 || descarte != '\n') {
        printf("Error: Solo se permiten numeros enteros. Intente de nuevo: ");
        while (getchar() != '\n');
    }
    return valor;
}
float leerFloat() {
    float valor;
    char descarte;
    while (scanf("%f%c", &valor, &descarte) != 2 || descarte != '\n') {
        printf("Error: Solo se permiten numeros (decimales con '.'). Intente de nuevo: ");
        while (getchar() != '\n');
    }
    return valor;
}
void inicializarInventario() {
    FILE *archivo = fopen("vehiculos.txt", "r");
    if (archivo == NULL) {
        archivo = fopen("vehiculos.txt", "w");
        fprintf(archivo, "Chevrolet Camioneta Usado 13500.00 1\n");
        fprintf(archivo, "Chevrolet Camioneta Usado 15800.00 1\n");
        fprintf(archivo, "Toyota Sedan Nuevo 22000.00 1\n");
        fprintf(archivo, "Chevrolet Camioneta Usado 12000.00 1\n");
        fclose(archivo);
    } else {
        fclose(archivo);
    }
}
void buscarYComprarVehiculo() {
    FILE *fVehiculos = fopen("vehiculos.txt", "r");
    if (fVehiculos == NULL) {
        printf("Error al abrir el inventario.\n");
        return;
    }
    Vehiculo inventario[100];
    int totalVehiculos = 0;
    while (fscanf(fVehiculos, "%s %s %s %f %d", 
           inventario[totalVehiculos].marca, 
           inventario[totalVehiculos].tipo, 
           inventario[totalVehiculos].estado, 
           &inventario[totalVehiculos].precio, 
           &inventario[totalVehiculos].disponible) == 5) {
        totalVehiculos++;
    }
    fclose(fVehiculos);
    char tipoBuscado[20], marcaBuscada[30];
    float presupuestoMax;
    printf("\n--- BUSQUEDA DE VEHICULO ---\n");
    printf("Ingrese tipo de vehiculo de preferencia (ej. Camioneta): ");
    leerCadenaLetras(tipoBuscado, 20);
    printf("Ingrese la marca de preferencia (ej. Chevrolet): ");
    leerCadenaLetras(marcaBuscada, 30);
    printf("Ingrese el presupuesto maximo disponible (ej. 14000): ");
    presupuestoMax = leerFloat();
    printf("\nResultados que se ajustan a la busqueda:\n");
    printf("ID\tMarca\t\tTipo\t\tEstado\tPrecio\n");
    printf("----------------------------------------------------------\n");
    int encontrados = 0;
    int indicesCoincidentes[100];
    for (int i = 0; i < totalVehiculos; i++) {
        if (strcasecmp(inventario[i].tipo, tipoBuscado) == 0 &&
            strcasecmp(inventario[i].marca, marcaBuscada) == 0 &&
            inventario[i].precio <= presupuestoMax &&
            inventario[i].disponible == 1) {
            
            printf("[%d]\t%s\t\t%s\t%s\t$%.2f\n", i + 1, inventario[i].marca, inventario[i].tipo, inventario[i].estado, inventario[i].precio);
            indicesCoincidentes[encontrados] = i;
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("No se encontraron vehiculos con esas especificaciones o presupuesto.\n");
        return;
    }
    printf("\nSeleccione el ID del vehiculo que desea comprar: ");
    int opcion;
    int idValido = 0;
    
    do {
        opcion = leerEntero();
        for(int j = 0; j < encontrados; j++) {
            if(indicesCoincidentes[j] == (opcion - 1)) {
                idValido = 1;
                break;
            }
        }
        if(!idValido) printf("ID no valido. Seleccione un ID de la lista anterior: ");
    } while (!idValido);
    int indiceSeleccionado = opcion - 1;
    Venta nuevaVenta;
    printf("\n--- REGISTRO DE VENTA ---\n");
    printf("Nombre del Cliente: ");
    leerCadenaLetras(nuevaVenta.nombreCliente, 50);
    printf("Edad del Cliente: ");
    nuevaVenta.edadCliente = leerEntero();
    strcpy(nuevaVenta.marcaVehiculo, inventario[indiceSeleccionado].marca);
    nuevaVenta.precioVenta = inventario[indiceSeleccionado].precio;
    inventario[indiceSeleccionado].disponible = 0; // Marcar como vendido

    // 1. Actualizar el archivo de vehículos
    fVehiculos = fopen("vehiculos.txt", "w");
    for (int i = 0; i < totalVehiculos; i++) {
        fprintf(fVehiculos, "%s %s %s %.2f %d\n", 
                inventario[i].marca, inventario[i].tipo, inventario[i].estado, inventario[i].precio, inventario[i].disponible);
    }
    fclose(fVehiculos);

    // 2. Persistir la venta en el archivo de ventas
    FILE *fVentas = fopen("ventas.txt", "a");
    if (fVentas != NULL) {
        fprintf(fVentas, "%s %d %s %.2f\n", 
                nuevaVenta.nombreCliente, nuevaVenta.edadCliente, nuevaVenta.marcaVehiculo, nuevaVenta.precioVenta);
        fclose(fVentas);
        printf("\nVenta registrada con exito y guardada en archivos!\n");
    } else {
        printf("Error al guardar la transaccion de venta.\n");
    }
}

int main() {
    inicializarInventario();
    int opcionMenu;

    do {
        printf("\n============================================\n");
        printf("  CONCESIONARIA RUEDAS DE ORO - SISTEMA SGIC\n");
        printf("============================================\n");
        printf("1. Buscar vehiculos por preferencia y Realizar Venta\n");
        printf("2. Salir\n");
        printf("Seleccione una opcion: ");
        opcionMenu = leerEntero();

        switch (opcionMenu) {
            case 1:
                buscarYComprarVehiculo();
                break;
            case 2:
                printf("Saliendo del sistema. Gracias por usar SGIC!\n");
                break;
            default:
                printf("Opcion no valida. Intente de nuevo.\n");
        }
    } while (opcionMenu != 2);

    return 0;
}
