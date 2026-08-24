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
        strcpy(nombreArchivo, "comandas_");

        int len = 0;
        while (nombreArchivo[len] != '\0') len++; // busco donde termina lo ya escrito
        strcpy(nombreArchivo + len, fecha);        // sigo escribiendo desde ahi

        len = 0;// hago lo mismo con el ".dat"
        while (nombreArchivo[len] != '\0') len++;
        strcpy(nombreArchivo + len, ".dat");

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

char nombreSemanal [40];
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
for (int i=0; i < cantTotal; i++) {
    fwrite(&todos[i], sizeof(Comanda), 1, fSemana);
}
fclose(fSemana);

cout << endl << "Listo. Se genero " << nombreSemanal<< " con " << cantTotal << " ventas en total." << endl;

return 0;
}
