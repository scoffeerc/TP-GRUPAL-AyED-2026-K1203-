#include <iostream>
#include <cstdio>
#include <cstring>
using namespace std;


struct ComandaHistorica {
    char fecha[11];          // dd-mm-aaaa
    char nombreMozo[50];
    int codigoProducto;
    int cantidad;
    float comision;
};

struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

struct Producto {
    int codigo;
    char descripcion[50];
    float precio;
    int stockActual;
};

struct Mozo {
    int idMozo;
    char nombre[50];
    char password[20];
    float totalComision;
};


struct RegistroNormalizado {
    char fecha[11];
    Comanda comanda;
};


void otroCodigo(char password[]) {

    for(int i = 0; password[i] != '\0'; i++) {
        password[i] = password[i] + 5;
    }
};


int buscarIdMozo(Mozo lista[], int &cantidadMozos, char nombreBuscado[]){};

long buscarProducto(FILE* f, int codigoBuscado, Producto p){};

int main() {


    FILE* fHist = fopen("comandas_historicas.dat","rb");
    FILE* fInvt = fopen("inventario.dat", "rb");

    Mozo listaMozos[100];
    int cantidadMozos = 0;

    RegistroNormalizado reg[1000];
    int cantidadReg = 0;

    ComandaHistorica comandaHist;

    while(fread(&comandaHist, sizeof(ComandaHistorica), 1, fHist) == 1){
        int idMozo = buscarIdMozo(listaMozos, cantidadMozos, comandaHist.nombreMozo);
        listaMozos[idMozo - 1].totalComision += comandaHist.comision;

        Producto prod;
        
        long posProducto = buscarProducto(fInvt, comandaHist.codigoProducto, prod);

        // actuliazar stock y escribir el archivo de inventario

        // se crea la comanza normalizada 
    };

    fclose(fHist);
    fclose(fInvt);

    //1. ordenar los registros normalizados por fecha y mozo
    
    //2. generar el archivo del día (PLANTILLA DIARIA)

    //3. generar archivo MOZOS.dat 

    cout << "Proceso de normalización completado." << endl;

    return 0;
}
