#include <stdio.h>
#include "funciones.h"

int main() {
    Zona zonas[NUM_ZONAS];
    int opcion;

    FILE *fin = fopen("datos.txt", "r");
    if (!fin) {
        printf("Error: no se pudo abrir datos.txt\n");
        return 1;
    }

    inicializarPesos();
    cargarDatos(fin, zonas);
    fclose(fin);

    predecir(zonas);

    do {
        printf("\n===== MENÚ =====\n");
        printf("1. Ver niveles actuales\n");
        printf("2. Ver predicción 24h\n");
        printf("3. Mostrar alertas\n");
        printf("4. Mostrar promedios históricos y comparación OMS\n");
        printf("5. Mostrar recomendaciones\n");
        printf("6. Buscar datos por zona y día\n");
        printf("7. Exportar reporte a archivo\n");
        printf("8. Salir\n");
        printf("=================\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch(opcion) {
            case 1: mostrarNivelesActuales(zonas); break;
            case 2: mostrarPrediccion(zonas); break;
            case 3: mostrarAlertas(zonas); break;
            case 4: mostrarPromediosHistoricos(zonas); break;
            case 5: mostrarRecomendaciones(zonas); break;
            case 6: buscarPorZonaYDia(zonas); break;
            case 7: guardarReporte(zonas); break;
            case 8: printf("Saliendo...\n"); break;
            default: printf("Opción inválida.\n"); break;
        }
    } while (opcion != 8);

    return 0;
}
