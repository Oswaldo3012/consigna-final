#include <stdio.h>
#include <string.h>
#include "funciones.h"

const float LIM_CO2 = 400.0;
const float LIM_SO2 = 20.0;
const float LIM_NO2 = 40.0;
const float LIM_PM25 = 25.0;

float pesos[NUM_DIAS_HIST];

void inicializarPesos() {
    float suma = 0;
    for (int i = 0; i < NUM_DIAS_HIST; i++) {
        pesos[i] = NUM_DIAS_HIST - i;
        suma += pesos[i];
    }
    for (int i = 0; i < NUM_DIAS_HIST; i++) {
        pesos[i] /= suma;
    }
}

float promedioPonderado(float valores[], float pesos[], int n) {
    float suma = 0;
    for (int i = 0; i < n; i++) {
        suma += valores[i] * pesos[i];
    }
    return suma;
}

void cargarDatos(FILE *file, Zona zonas[]) {
    int zonaIndex = -1;
    for (int i = 0; i < NUM_ZONAS * NUM_DIAS_HIST; i++) {
        char nombre[50];
        float CO2, SO2, NO2, PM25, temp, viento, humedad;

        fscanf(file, "%s %f %f %f %f %f %f %f",
            nombre, &CO2, &SO2, &NO2, &PM25, &temp, &viento, &humedad);

        if (i % NUM_DIAS_HIST == 0) {
            zonaIndex++;
            strcpy(zonas[zonaIndex].nombre, nombre);
        }

        int dia = i % NUM_DIAS_HIST;
        zonas[zonaIndex].historial[dia].contaminacion.CO2 = CO2;
        zonas[zonaIndex].historial[dia].contaminacion.SO2 = SO2;
        zonas[zonaIndex].historial[dia].contaminacion.NO2 = NO2;
        zonas[zonaIndex].historial[dia].contaminacion.PM25 = PM25;
        zonas[zonaIndex].historial[dia].temperatura = temp;
        zonas[zonaIndex].historial[dia].viento = viento;
        zonas[zonaIndex].historial[dia].humedad = humedad;
    }

    for (int i = 0; i < NUM_ZONAS; i++) {
        char nombre[50];
        float CO2, SO2, NO2, PM25, temp, viento, humedad;

        fscanf(file, "%s %f %f %f %f %f %f %f",
            nombre, &CO2, &SO2, &NO2, &PM25, &temp, &viento, &humedad);

        // Buscar la zona correspondiente por nombre (sin sobrescribir el nombre)
        for (int j = 0; j < NUM_ZONAS; j++) {
            if (strcmp(zonas[j].nombre, nombre) == 0) {
                zonas[j].actual.contaminacion.CO2 = CO2;
                zonas[j].actual.contaminacion.SO2 = SO2;
                zonas[j].actual.contaminacion.NO2 = NO2;
                zonas[j].actual.contaminacion.PM25 = PM25;
                zonas[j].actual.temperatura = temp;
                zonas[j].actual.viento = viento;
                zonas[j].actual.humedad = humedad;
                break;
            }
        }
    }
}

void predecir(Zona zonas[]) {
    // Coeficientes de sensibilidad (ajustables)
    const float kTemp = 0.01, kViento = -0.01, kHum = -0.005;
    for (int i = 0; i < NUM_ZONAS; i++) {
        float arr_CO2[NUM_DIAS_HIST], arr_SO2[NUM_DIAS_HIST], arr_NO2[NUM_DIAS_HIST], arr_PM25[NUM_DIAS_HIST];
        float arrTemp[NUM_DIAS_HIST], arrViento[NUM_DIAS_HIST], arrHum[NUM_DIAS_HIST];
        for (int d = 0; d < NUM_DIAS_HIST; d++) {
            arr_CO2[d] = zonas[i].historial[d].contaminacion.CO2;
            arr_SO2[d] = zonas[i].historial[d].contaminacion.SO2;
            arr_NO2[d] = zonas[i].historial[d].contaminacion.NO2;
            arr_PM25[d] = zonas[i].historial[d].contaminacion.PM25;
            arrTemp[d] = zonas[i].historial[d].temperatura;
            arrViento[d] = zonas[i].historial[d].viento;
            arrHum[d] = zonas[i].historial[d].humedad;
        }
        float promTemp = 0, promViento = 0, promHum = 0;
        for (int d = 0; d < NUM_DIAS_HIST; d++) {
            promTemp += arrTemp[d];
            promViento += arrViento[d];
            promHum += arrHum[d];
        }
        promTemp /= NUM_DIAS_HIST;
        promViento /= NUM_DIAS_HIST;
        promHum /= NUM_DIAS_HIST;

        float deltaTemp = zonas[i].actual.temperatura - promTemp;
        float deltaViento = zonas[i].actual.viento - promViento;
        float deltaHum = zonas[i].actual.humedad - promHum;

        zonas[i].prediccion.contaminacion.CO2 = promedioPonderado(arr_CO2, pesos, NUM_DIAS_HIST) + kTemp * deltaTemp + kViento * deltaViento + kHum * deltaHum;
        zonas[i].prediccion.contaminacion.SO2 = promedioPonderado(arr_SO2, pesos, NUM_DIAS_HIST) + kTemp * deltaTemp + kViento * deltaViento + kHum * deltaHum;
        zonas[i].prediccion.contaminacion.NO2 = promedioPonderado(arr_NO2, pesos, NUM_DIAS_HIST) + kTemp * deltaTemp + kViento * deltaViento + kHum * deltaHum;
        zonas[i].prediccion.contaminacion.PM25 = promedioPonderado(arr_PM25, pesos, NUM_DIAS_HIST) + kTemp * deltaTemp + kViento * deltaViento + kHum * deltaHum;

        zonas[i].prediccion.temperatura = zonas[i].actual.temperatura;
        zonas[i].prediccion.viento = zonas[i].actual.viento;
        zonas[i].prediccion.humedad = zonas[i].actual.humedad;
    }
}

int superaLimite(Contaminantes c) {
    return (c.CO2 > LIM_CO2 || c.SO2 > LIM_SO2 || c.NO2 > LIM_NO2 || c.PM25 > LIM_PM25);
}

void mostrarNivelesActuales(Zona zonas[]) {
    printf("\n--- Niveles Actuales ---\n");
    printf("%-10s | %-8s | %-8s | %-8s | %-8s\n", "Zona", "CO2", "SO2", "NO2", "PM2.5");
    printf("-----------------------------------------------------\n");
    for (int i = 0; i < NUM_ZONAS; i++) {
        printf("%-10s | %8.2f | %8.2f | %8.2f | %8.2f\n",
            zonas[i].nombre,
            zonas[i].actual.contaminacion.CO2,
            zonas[i].actual.contaminacion.SO2,
            zonas[i].actual.contaminacion.NO2,
            zonas[i].actual.contaminacion.PM25);
    }
}

void mostrarPrediccion(Zona zonas[]) {
    printf("\n--- Predicción a 24h ---\n");
    printf("%-10s | %-8s | %-8s | %-8s | %-8s\n", "Zona", "CO2", "SO2", "NO2", "PM2.5");
    printf("-----------------------------------------------------\n");
    for (int i = 0; i < NUM_ZONAS; i++) {
        printf("%-10s | %8.2f | %8.2f | %8.2f | %8.2f\n",
            zonas[i].nombre,
            zonas[i].prediccion.contaminacion.CO2,
            zonas[i].prediccion.contaminacion.SO2,
            zonas[i].prediccion.contaminacion.NO2,
            zonas[i].prediccion.contaminacion.PM25);
    }
}

void mostrarAlertas(Zona zonas[]) {
    printf("\n--- Alertas ---\n");
    printf("%-10s | %-12s | %-10s | %-10s | %-12s\n", "Zona", "Contaminante", "Valor", "Límite", "Tipo");
    printf("---------------------------------------------------------------------\n");
    for (int i = 0; i < NUM_ZONAS; i++) {
        if (zonas[i].actual.contaminacion.CO2 > LIM_CO2)
            printf("%-10s | %-12s | %10.2f | %10.2f | %-12s\n", zonas[i].nombre, "CO2", zonas[i].actual.contaminacion.CO2, LIM_CO2, "Actual");
        if (zonas[i].actual.contaminacion.SO2 > LIM_SO2)
            printf("%-10s | %-12s | %10.2f | %10.2f | %-12s\n", zonas[i].nombre, "SO2", zonas[i].actual.contaminacion.SO2, LIM_SO2, "Actual");
        if (zonas[i].actual.contaminacion.NO2 > LIM_NO2)
            printf("%-10s | %-12s | %10.2f | %10.2f | %-12s\n", zonas[i].nombre, "NO2", zonas[i].actual.contaminacion.NO2, LIM_NO2, "Actual");
        if (zonas[i].actual.contaminacion.PM25 > LIM_PM25)
            printf("%-10s | %-12s | %10.2f | %10.2f | %-12s\n", zonas[i].nombre, "PM2.5", zonas[i].actual.contaminacion.PM25, LIM_PM25, "Actual");
        if (zonas[i].prediccion.contaminacion.CO2 > LIM_CO2)
            printf("%-10s | %-12s | %10.2f | %10.2f | %-12s\n", zonas[i].nombre, "CO2", zonas[i].prediccion.contaminacion.CO2, LIM_CO2, "Predicción");
        if (zonas[i].prediccion.contaminacion.SO2 > LIM_SO2)
            printf("%-10s | %-12s | %10.2f | %10.2f | %-12s\n", zonas[i].nombre, "SO2", zonas[i].prediccion.contaminacion.SO2, LIM_SO2, "Predicción");
        if (zonas[i].prediccion.contaminacion.NO2 > LIM_NO2)
            printf("%-10s | %-12s | %10.2f | %10.2f | %-12s\n", zonas[i].nombre, "NO2", zonas[i].prediccion.contaminacion.NO2, LIM_NO2, "Predicción");
        if (zonas[i].prediccion.contaminacion.PM25 > LIM_PM25)
            printf("%-10s | %-12s | %10.2f | %10.2f | %-12s\n", zonas[i].nombre, "PM2.5", zonas[i].prediccion.contaminacion.PM25, LIM_PM25, "Predicción");
    }
}

void mostrarRecomendaciones(Zona zonas[]) {
    printf("\n--- Recomendaciones ---\n");
    for (int i = 0; i < NUM_ZONAS; i++) {
        if (superaLimite(zonas[i].actual.contaminacion) || superaLimite(zonas[i].prediccion.contaminacion)) {
            printf("Zona %s:\n", zonas[i].nombre);
            printf(" - Reducir tráfico vehicular\n");
            printf(" - Cierre temporal de industrias\n");
            printf(" - Suspender actividades al aire libre\n\n");
        } else {
            printf("Zona %s: Niveles normales.\n", zonas[i].nombre);
        }
    }
}

void guardarReporte(Zona zonas[]) {
    FILE *fout = fopen("reporte.txt", "w");
    if (!fout) {
        printf("Error al abrir archivo de salida.\n");
        return;
    }
    for (int i = 0; i < NUM_ZONAS; i++) {
        fprintf(fout, "Zona: %s\n", zonas[i].nombre);
        fprintf(fout, "ACTUAL -> CO2: %.2f, SO2: %.2f, NO2: %.2f, PM2.5: %.2f\n",
            zonas[i].actual.contaminacion.CO2,
            zonas[i].actual.contaminacion.SO2,
            zonas[i].actual.contaminacion.NO2,
            zonas[i].actual.contaminacion.PM25);
        fprintf(fout, "PREDICCIÓN -> CO2: %.2f, SO2: %.2f, NO2: %.2f, PM2.5: %.2f\n",
            zonas[i].prediccion.contaminacion.CO2,
            zonas[i].prediccion.contaminacion.SO2,
            zonas[i].prediccion.contaminacion.NO2,
            zonas[i].prediccion.contaminacion.PM25);

        if (superaLimite(zonas[i].actual.contaminacion))
            fprintf(fout, "Alerta ACTUAL: niveles sobre límites.\n");
        if (superaLimite(zonas[i].prediccion.contaminacion))
            fprintf(fout, "Alerta PREDICCIÓN: niveles sobre límites.\n");

        fprintf(fout, "-----------------------------------\n");
    }
    fclose(fout);
    printf("Reporte exportado en reporte.txt\n");
}

void buscarPorZonaYDia(Zona zonas[]) {
    char nombre[50];
    int dia;
    printf("Ingrese el nombre de la zona (ejemplo: Zona1): ");
    scanf("%s", nombre);
    printf("Ingrese el número de día (0 a %d): ", NUM_DIAS_HIST - 1);
    scanf("%d", &dia);
    if (dia < 0 || dia >= NUM_DIAS_HIST) {
        printf("Día fuera de rango.\n");
        return;
    }
    for (int i = 0; i < NUM_ZONAS; i++) {
        if (strcmp(zonas[i].nombre, nombre) == 0) {
            RegistroDia *r = &zonas[i].historial[dia];
            printf("\nZona: %s, Día: %d\n", nombre, dia);
            printf("CO2: %.2f, SO2: %.2f, NO2: %.2f, PM2.5: %.2f\n", r->contaminacion.CO2, r->contaminacion.SO2, r->contaminacion.NO2, r->contaminacion.PM25);
            printf("Temperatura: %.2f, Viento: %.2f, Humedad: %.2f\n", r->temperatura, r->viento, r->humedad);
            return;
        }
    }
    printf("Zona no encontrada.\n");
}

void mostrarPromediosHistoricos(Zona zonas[]) {
    printf("\n--- Promedios históricos (últimos 30 días) ---\n");
    printf("Límites OMS: CO2 > %.2f, SO2 > %.2f, NO2 > %.2f, PM2.5 > %.2f\n", LIM_CO2, LIM_SO2, LIM_NO2, LIM_PM25);
    printf("%-10s | %-8s | %-8s | %-8s | %-8s | %s\n", "Zona", "CO2", "SO2", "NO2", "PM2.5", "Supera OMS");
    printf("---------------------------------------------------------------------\n");
    for (int i = 0; i < NUM_ZONAS; i++) {
        float sumCO2 = 0, sumSO2 = 0, sumNO2 = 0, sumPM25 = 0;
        for (int d = 0; d < NUM_DIAS_HIST; d++) {
            sumCO2 += zonas[i].historial[d].contaminacion.CO2;
            sumSO2 += zonas[i].historial[d].contaminacion.SO2;
            sumNO2 += zonas[i].historial[d].contaminacion.NO2;
            sumPM25 += zonas[i].historial[d].contaminacion.PM25;
        }
        float promCO2 = sumCO2 / NUM_DIAS_HIST;
        float promSO2 = sumSO2 / NUM_DIAS_HIST;
        float promNO2 = sumNO2 / NUM_DIAS_HIST;
        float promPM25 = sumPM25 / NUM_DIAS_HIST;
        int supera = (promCO2 > LIM_CO2 || promSO2 > LIM_SO2 || promNO2 > LIM_NO2 || promPM25 > LIM_PM25);
        printf("%-10s | %8.2f | %8.2f | %8.2f | %8.2f | %s\n",
            zonas[i].nombre, promCO2, promSO2, promNO2, promPM25,
            supera ? "Sí" : "No");
    }
}
