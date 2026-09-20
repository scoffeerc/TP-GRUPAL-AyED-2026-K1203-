#include <iostream>
#include <cstdio>
#include <cstring>

using namespace std;

struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

const int MAX_REGISTROS = 20000;

// Global: no ocupa la pila, asi que puede ser grande.
Comanda todos[MAX_REGISTROS];

int main (){
    int cantTotal = 0;
    int cantDias;
    bool excedido = false;

    cout << "Cuantos dias tiene la semana a cerrar? ";
    cin >> cantDias;

    // 1) leo cada día y junto todo en un array
    for (int i = 0; i < cantDias; i++){
        char fecha[50];
        cout << "Fecha del dia " << (i + 1) << " (dd-mm-aaaa): ";
        cin.width(50);
        cin >> fecha;

        char nombreArchivo[100];
        strcpy(nombreArchivo, "comandas_");

        int len = 0;
        while (nombreArchivo[len] != '\0') len++;
        strcpy(nombreArchivo + len, fecha);

        len = 0;
        while (nombreArchivo[len] != '\0') len++;
        strcpy(nombreArchivo + len, ".dat");

        FILE* fDia = fopen(nombreArchivo, "rb");
        if (fDia == NULL){
            cout << "  no hay planilla para ese dia, se saltea" << endl;
        }
        else{
            Comanda c;
            int LeidosDeEsteDia = 0;
            while (!excedido && fread(&c, sizeof(Comanda), 1, fDia) == 1){
                if (cantTotal >= MAX_REGISTROS) {
                    excedido = true;
                } else {
                    todos[cantTotal] = c;
                    cantTotal++;
                    LeidosDeEsteDia++;
                }
            }
            fclose(fDia);
            cout << "  " << LeidosDeEsteDia << " ventas cargadas de " << nombreArchivo << endl;
        }
    }

    if (excedido) {
        cout << "Error: la semana tiene mas de " << MAX_REGISTROS
             << " ventas. No se genero la planilla semanal." << endl;
        return 1;
    }

    // 2) Ordeno todo el array por id_Mozo
    for (int i = 0; i < cantTotal - 1; i++) {
        int menor = i;
        for (int j = i + 1; j < cantTotal; j++) {
            if (todos[j].idMozo < todos[menor].idMozo) {
                menor = j;
            }
        }
        if (menor != i) {
            Comanda temp = todos[i];
            todos[i] = todos[menor];
            todos[menor] = temp;
        }
    }

    // 3) pido el nombre del archivo semanal y lo escribo
    char semana[50], mes[50];
    cout << "Numero de semana (ej 1): ";
    cin.width(50);
    cin >> semana;
    cout << "Mes (ej 06): ";
    cin.width(50);
    cin >> mes;

    char nombreSemanal[200];
    strcpy(nombreSemanal, "comandas_semana_s");

    int lenS = 0;
    while (nombreSemanal[lenS] != '\0') lenS++;
    strcpy(nombreSemanal + lenS, semana);

    lenS = 0;
    while (nombreSemanal[lenS] != '\0') lenS++;
    strcpy(nombreSemanal + lenS, "-");

    lenS = 0;
    while (nombreSemanal[lenS] != '\0') lenS++;
    strcpy(nombreSemanal + lenS, mes);

    lenS = 0;
    while (nombreSemanal[lenS] != '\0') lenS++;
    strcpy(nombreSemanal + lenS, ".dat");

    FILE* fSemana = fopen(nombreSemanal, "wb");
    if (fSemana == NULL) {
        cout << "Error: no se pudo crear " << nombreSemanal << endl;
        return 1;
    }
    for (int i = 0; i < cantTotal; i++) {
        fwrite(&todos[i], sizeof(Comanda), 1, fSemana);
    }
    fclose(fSemana);

    cout << endl << "Listo. Se genero " << nombreSemanal << " con " << cantTotal << " ventas en total." << endl;

    return 0;
}