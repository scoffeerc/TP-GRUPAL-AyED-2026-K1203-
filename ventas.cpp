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

int main(){

    return 0;
};