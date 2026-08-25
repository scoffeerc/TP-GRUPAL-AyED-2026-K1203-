#include <iostream>
#include <cstdio>
#include <cstring>

using namespace std;    

const float TASA_COMISION = 0.10f; // 10% de la venta
const int K = 5; // Constante de encriptación

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

struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

void encriptado(Mozo arr[], int len){
    Mozo m;
    for(int i = 0; m.password[i] != '\0'; i++){
        m.password[i] = m.password[i] + K;
    }
}

int main() {
    Producto p;
    Mozo m;
    Comanda c;
    FILE* e = fopen("mozos.dat", "rb+");
    FILE* f = fopen("inventario_dat", "rb");
    int n = 20;
    Mozo claves[n];
    char fecha[30];
    char claveabuscar[20];
    encriptado(claves, n);
    int codigoabuscar;
    while(fread(&m, sizeof(Mozo), 1, e) == 1 && fread(&f, sizeof(Producto), 1, f) == 1){
        cout << "Ingrese una fecha: ";
        cin >> fecha;

        if (fecha == "dd-mm-aaaa"){
            FILE* g = fopen("comandas_dd-mm-aaaa.dat", "rb+");
            int idabuscar;
            cout << "Ingrese el número de mozo: ";
            if (idabuscar != m.idMozo){
                cout << "El mozo no existe." << endl;
            }
            cin >> m.idMozo;
            cout << "Ingrese la clave del mozo: ";
            if (claveabuscar != m.password){
                cout << "Clave incorrecta." << endl;
            }
            cout << "Busque un producto: ";
            cin >> codigoabuscar;
            if (codigoabuscar != p.codigo){
                cout << "No hay producto." << endl;
            }



            
    return 0;
};
