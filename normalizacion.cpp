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


int buscarIdMozo(Mozo lista[], int &cantidadMozos, char nombreBuscado[]){
    for (int i = 0; i< cantidadMozos; i++){
        if (strcmp(lista[i].nombre, nombreBuscado) ==0)
        {
           return lista[i].idMozo;  // encontro el mozo devuelve su id. /* code */
        }
        
    }
    // si el mozo es nuevo tengo que agregarlo.
    int nuevoid = cantidadMozos + 1; 
    lista[cantidadMozos].idMozo = nuevoid;  // le da un id al mozo intercambiandolo por su nombre 
    strcpy(lista[cantidadMozos].nombre, nombreBuscado);  
    lista[cantidadMozos].totalComision = 0; 
    cantidadMozos++;

    return nuevoid; 
};

long buscarProducto(FILE* f, int codigoBuscado, Producto &p){

    fseek(f, 0, SEEK_END);
    long n = ftell(f) / sizeof(Producto);   // Es la cantidad de productos en el archivo 

    long pri = 0, ult = n - 1, pos = -1;

    while (pri <= ult && pos == -1)
    {
        long med = (pri + ult) / 2;
        fseek(f, med * sizeof(Producto), SEEK_SET);
        fread(&p, sizeof(Producto), 1, f);

        if (p.codigo == codigoBuscado)
        {
            pos = med;
        }
        else if (codigoBuscado > p.codigo)
        {
            pri = med + 1;
        }
        else
        {
            ult = med - 1;
        }
    }

    return pos;   // -1 si no se encuntra, o la posicion del producto en el archivo en caso contrario 
};

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

        if (posProducto != -1)
{
    prod.stockActual -= comandaHist.cantidad;
    fseek(fInvt, posProducto * sizeof(Producto), SEEK_SET);   // vuelvo a esa posición
    fwrite(&prod, sizeof(Producto), 1, fInvt);                  // grabo el stock actualizado
}

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
    