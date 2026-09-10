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

const int MAX_REGISTROS = 5000;

int main (){
    Comanda todos [MAX_REGISTROS];
    int cantTotal =0;
    int cantDias;
    cout << "Cuantos dias tiene la semana a cerrar? ";
    cin >> cantDias;

    // 1) leo cada día y junto todo en un array
    for (int i =0; i< cantDias; i++){
        char fecha [11];
        cout << "Fecha del dia " << (i + 1) << " (dd-mm-aaaa): ";
        cin >> fecha;

        char nombreArchivo[30];
        int pos = 0;

        const char* prefijo = "comandas_";
        for (int k = 0; prefijo[k] != '\0'; k++) {
                nombreArchivo[pos] = prefijo[k];
                pos++;
        }

        for (int k = 0; fecha[k] != '\0'; k++) {
            nombreArchivo[pos] = fecha[k];
            pos++;
        }

        const char* sufijo = ".dat";
        for (int k = 0; sufijo[k] != '\0'; k++) {
            nombreArchivo[pos] = sufijo[k];
            pos++;
        }

        nombreArchivo[pos] = '\0';

        FILE* fDia = fopen(nombreArchivo, "rb");
        if (fDia == NULL){
        cout << "  no hay planilla para ese dia, se saltea" << endl;
        }
        else{
        Comanda c;
        int LeidosDeEsteDia=0;
        while (fread(&c, sizeof(Comanda), 1, fDia) ==1){
            todos [cantTotal] = c;
            cantTotal++;
            LeidosDeEsteDia++;
        }
        fclose (fDia);
        cout << "  " << LeidosDeEsteDia << " ventas cargadas de " << nombreArchivo << endl;
        }
    }

// 2) Ordeno todo el array por id_Mozo
for (int i=0; i< cantTotal - 1; i++) {
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
char semana[10], mes[3];
cout << "Numero de semana (ej 1): ";
cin >> semana;
cout << "Mes (ej 06): ";
cin >> mes;

char nombreSemanal[40];
int posS = 0;

const char* prefijoSemanal = "comandas_semana_s";
for (int k = 0; prefijoSemanal[k] != '\0'; k++) {
    nombreSemanal[posS] = prefijoSemanal[k];
    posS++;
}

for (int k = 0; semana[k] != '\0'; k++) {
    nombreSemanal[posS] = semana[k];
    posS++;
}

nombreSemanal[posS] = '-';
posS++;

for (int k = 0; mes[k] != '\0'; k++) {
    nombreSemanal[posS] = mes[k];
    posS++;
}

const char* sufijoSemanal = ".dat";
for (int k = 0; sufijoSemanal[k] != '\0'; k++) {
    nombreSemanal[posS] = sufijoSemanal[k];
    posS++;
}

nombreSemanal[posS] = '\0';

FILE* fSemana = fopen(nombreSemanal, "wb");
for (int i=0; i < cantTotal; i++) {
    fwrite(&todos[i], sizeof(Comanda), 1, fSemana);
}
fclose(fSemana);

cout << endl << "Listo. Se genero " << nombreSemanal<< " con " << cantTotal << " ventas en total." << endl;

return 0;
}
