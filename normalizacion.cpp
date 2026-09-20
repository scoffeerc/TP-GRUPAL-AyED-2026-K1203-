#include <iostream>
#include <cstdio>
#include <cstring>
using namespace std;

const int MAX_REGISTROS = 10000;      // maximo de ventas historicas que se procesan
const int MAX_MOZOS = MAX_REGISTROS;  // cada venta agrega a lo sumo un mozo nuevo: nunca se desborda

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

// fechaNum solo se usa en memoria para ordenar cronologicamente; no se graba.
struct RegistroNormalizado {
    char fecha[11];
    int fechaNum;
    Comanda comanda;
};

// Arrays globales: no ocupan la pila, asi que pueden ser grandes.
Mozo listaMozos[MAX_MOZOS];
RegistroNormalizado reg[MAX_REGISTROS];

// "dd-mm-aaaa" -> aaaammdd, para comparar fechas como numeros.
int fechaAEntero(const char* f) {
    int d = 0, m = 0, a = 0;
    sscanf(f, "%d-%d-%d", &d, &m, &a);
    return a * 10000 + m * 100 + d;
}

// Suma 5 a cada caracter (se deshace restando 5).
void otroCodigo(char password[]) {
    for (int i = 0; password[i] != '\0'; i++) {
        password[i] = password[i] + 5;
    }
}

int buscarIdMozo(Mozo lista[], int &cantidadMozos, char nombreBuscado[]) {
    for (int i = 0; i < cantidadMozos; i++) {
        if (strcmp(lista[i].nombre, nombreBuscado) == 0) {
            return lista[i].idMozo;  // ya existe: devuelve su id
        }
    }
    // si el mozo es nuevo lo agrego con el siguiente id
    int nuevoid = cantidadMozos + 1;
    lista[cantidadMozos].idMozo = nuevoid;
    strcpy(lista[cantidadMozos].nombre, nombreBuscado);
    lista[cantidadMozos].totalComision = 0;
    cantidadMozos++;

    return nuevoid;
}

// inventario.dat esta ordenado por codigo (con huecos): busqueda binaria.
long buscarProducto(FILE* f, int codigoBuscado, Producto &p) {
    fseek(f, 0, SEEK_END);
    long n = ftell(f) / sizeof(Producto);   // cantidad de productos en el archivo

    long pri = 0, ult = n - 1, pos = -1;

    while (pri <= ult && pos == -1) {
        long med = (pri + ult) / 2;
        fseek(f, med * sizeof(Producto), SEEK_SET);
        fread(&p, sizeof(Producto), 1, f);

        if (p.codigo == codigoBuscado) {
            pos = med;
        } else if (codigoBuscado > p.codigo) {
            pri = med + 1;
        } else {
            ult = med - 1;
        }
    }

    return pos;   // -1 si no se encuentra, o la posicion en el archivo
}

int main() {

    FILE* fHist = fopen("comandas_historicas.dat", "rb");
    FILE* fInvt = fopen("inventario.dat", "rb+");

    if (fHist == NULL || fInvt == NULL) {
        cout << "No se pudieron abrir los archivos base." << endl;
        if (fHist) fclose(fHist);
        if (fInvt) fclose(fInvt);
        return 1;
    }

    // Antes de tocar nada, verifico que el historico entre en memoria.
    fseek(fHist, 0, SEEK_END);
    long totalHist = ftell(fHist) / sizeof(ComandaHistorica);
    fseek(fHist, 0, SEEK_SET);
    if (totalHist > MAX_REGISTROS) {
        cout << "Error: el historico tiene " << totalHist << " ventas y el maximo es "
             << MAX_REGISTROS << ". No se modifico ningun archivo." << endl;
        fclose(fHist);
        fclose(fInvt);
        return 1;
    }

    int cantidadMozos = 0;
    int cantidadReg = 0;

    ComandaHistorica comandaHist;
    // leer todo el historico: armar mozos + registros + actualizar stock
    while (fread(&comandaHist, sizeof(ComandaHistorica), 1, fHist) == 1) {
        int idMozo = buscarIdMozo(listaMozos, cantidadMozos, comandaHist.nombreMozo);
        listaMozos[idMozo - 1].totalComision += comandaHist.comision;

        Producto prod;
        long posProducto = buscarProducto(fInvt, comandaHist.codigoProducto, prod);

        if (posProducto != -1) {
            if (prod.stockActual < comandaHist.cantidad) {
                cout << "Aviso: stock insuficiente del producto " << prod.codigo
                     << " (habia " << prod.stockActual << ", se vendieron "
                     << comandaHist.cantidad << "). Queda en 0." << endl;
                prod.stockActual = 0;
            } else {
                prod.stockActual -= comandaHist.cantidad;
            }
            fseek(fInvt, posProducto * sizeof(Producto), SEEK_SET);   // vuelvo a esa posicion
            fwrite(&prod, sizeof(Producto), 1, fInvt);                // grabo el stock actualizado
        } else {
            cout << "Aviso: el producto " << comandaHist.codigoProducto
                 << " no esta en el inventario." << endl;
        }

        // armo el registro normalizado y lo guardo en memoria
        strcpy(reg[cantidadReg].fecha, comandaHist.fecha);
        reg[cantidadReg].fechaNum = fechaAEntero(comandaHist.fecha);
        reg[cantidadReg].comanda.idMozo = idMozo;
        reg[cantidadReg].comanda.codigoProducto = comandaHist.codigoProducto;
        reg[cantidadReg].comanda.cantidad = comandaHist.cantidad;
        reg[cantidadReg].comanda.comision = comandaHist.comision;
        cantidadReg++;
    }

    fclose(fHist);
    fclose(fInvt);

    // password inicial = su id como texto, ya encriptada
    for (int i = 0; i < cantidadMozos; i++) {
        char passwordInicial[20];
        sprintf(passwordInicial, "%d", listaMozos[i].idMozo);   // numero a texto
        otroCodigo(passwordInicial);                            // lo encripto
        strcpy(listaMozos[i].password, passwordInicial);
    }

    FILE* fMozos = fopen("mozos.dat", "wb");
    if (fMozos == NULL) {
        cout << "Error: no se pudo crear mozos.dat" << endl;
        return 1;
    }
    for (int i = 0; i < cantidadMozos; i++) {
        fwrite(&listaMozos[i], sizeof(Mozo), 1, fMozos);
    }
    fclose(fMozos);

    // ordenar TODOS los registros por fecha (cronologicamente) y, dentro de cada fecha, por mozo
    for (int i = 0; i < cantidadReg - 1; i++) {
        for (int j = 0; j < cantidadReg - 1 - i; j++) {
            bool debeIntercambiar = false;

            if (reg[j].fechaNum > reg[j + 1].fechaNum) {
                debeIntercambiar = true;
            } else if (reg[j].fechaNum == reg[j + 1].fechaNum &&
                       reg[j].comanda.idMozo > reg[j + 1].comanda.idMozo) {
                debeIntercambiar = true;
            }

            if (debeIntercambiar) {
                RegistroNormalizado temp = reg[j];
                reg[j] = reg[j + 1];
                reg[j + 1] = temp;
            }
        }
    }

    // generar un archivo por cada dia distinto
    int i = 0;
    while (i < cantidadReg) {
        char fechaActual[11];
        strcpy(fechaActual, reg[i].fecha);

        char nombreArchivo[30];
        sprintf(nombreArchivo, "comandas_%s.dat", fechaActual);

        FILE* fDia = fopen(nombreArchivo, "wb");
        if (fDia == NULL) {
            cout << "Error: no se pudo crear " << nombreArchivo << endl;
            return 1;
        }

        while (i < cantidadReg && strcmp(reg[i].fecha, fechaActual) == 0) {
            fwrite(&reg[i].comanda, sizeof(Comanda), 1, fDia);
            i++;
        }

        fclose(fDia);
    }

    cout << "Proceso de normalizacion completado." << endl;

    return 0;
}