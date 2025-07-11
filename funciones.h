#include <stdio.h>

#ifndef FUNCIONES_H
#define FUNCIONES_H

#define NUM_ZONAS 5
#define NUM_DIAS_HIST 30

typedef struct {
    float CO2;
    float SO2;
    float NO2;
    float PM25;
} Contaminantes;

typedef struct {
    Contaminantes contaminacion;
    float temperatura;
    float viento;
    float humedad;
} RegistroDia;

typedef struct {
    char nombre[50];
    RegistroDia historial[NUM_DIAS_HIST];
    RegistroDia actual;
    RegistroDia prediccion;
} Zona;

void inicializarPesos();
float promedioPonderado(float valores[], float pesos[], int n);
void cargarDatos(FILE *file, Zona zonas[]);
void predecir(Zona zonas[]);
int superaLimite(Contaminantes c);
void mostrarNivelesActuales(Zona zonas[]);
void mostrarPrediccion(Zona zonas[]);
void mostrarAlertas(Zona zonas[]);
void mostrarRecomendaciones(Zona zonas[]);
void guardarReporte(Zona zonas[]);
void buscarPorZonaYDia(Zona zonas[]);
void mostrarPromediosHistoricos(Zona zonas[]);

#endif
