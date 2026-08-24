#include <iostream>
#include <cstdio>
#include <cstring>

using namespace std;

struct Mozo {
    int idMozo;
    char nombre[8];
    char password[9];
    float totalComision;
};

struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};


int main() {

    FILE* f = fopen("comandas_semana_s1-06.dat", "rb");

    if (!f) {
        cout << "Error: No se pudo abrir el archivo semanal." << endl;
        return 1;
    }


    Comanda c;

    int totalBuffet = 0;
    

    if(fread(&c, sizeof(Comanda), 1, f) == 1) {
    bool quedanRegistrosPorLeer = true;

        while(quedanRegistrosPorLeer){

            int idActual = c.idMozo;

            int cantidadProductos = 0;
            float comisionTot = 0;

            while(quedanRegistrosPorLeer && idActual == c.idMozo) {

                cantidadProductos += c.cantidad;

                comisionTot += c.comision;

                totalBuffet += c.cantidad;

                if(fread(&c, sizeof(Comanda), 1, f) != 1) {
                    quedanRegistrosPorLeer = false;
                };
            };  
            cout << "Mozo: " << idActual << endl;
            cout << "Cantidad de productos vendidos: " << cantidadProductos << endl;
            cout << "Comision total: $" << comisionTot << endl;
            cout << endl;

        };
    } else {
        cout << "El archivo esta vacio." << endl;
    }


    cout << "Total de productos vendidos por el buffet: " << totalBuffet << endl;
    fclose(f);


    return 0;
}
