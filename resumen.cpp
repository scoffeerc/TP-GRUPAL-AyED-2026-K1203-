#include <iostream>
#include <cstdio>
#include <cstring>

using namespace std;

struct Mozo {
    int idMozo;
    char nombre[50];
    char password[20];
    float totalComision;
};

struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

int main() {
    char nombreArchivo[40];
    cout << "Ingrese el nombre del archivo semanal (ej: comandas_semana_s1-06.dat): ";
    cin >> nombreArchivo;

    FILE* f = fopen(nombreArchivo, "rb");
    if (!f) {
        cout << "Error: No se pudo abrir el archivo " << nombreArchivo << endl;
        return 1;
    }

    Comanda c;
    int totalBuffet = 0;

    cout << "\n===== RESUMEN DE CIERRE SEMANAL =====\n" << endl;

    if (fread(&c, sizeof(Comanda), 1, f) == 1) {
        bool quedanRegistros = true;

        while (quedanRegistros) {
            int idActual = c.idMozo;
            int cantidadProductos = 0;
            float comisionTot = 0;

            while (quedanRegistros && idActual == c.idMozo) {
                cantidadProductos += c.cantidad;
                comisionTot += c.comision;
                totalBuffet += c.cantidad;

                if (fread(&c, sizeof(Comanda), 1, f) != 1) {
                    quedanRegistros = false;
                }
            }  
            cout << "Mozo ID: " << idActual << endl;
            cout << "  - Cantidad de productos vendidos: " << cantidadProductos << endl;
            cout << "  - Comision total a pagar: $" << comisionTot << endl;
            cout << "-------------------------------------" << endl;
        }
    } else {
        cout << "El archivo esta vacio." << endl;
    }

    cout << "\nTOTAL DE PRODUCTOS VENDIDOS POR EL BUFFET: " << totalBuffet << endl;
    fclose(f);

    return 0;
}