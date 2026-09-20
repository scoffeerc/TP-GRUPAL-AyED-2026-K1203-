#include <iostream>
#include <cstdio>
#include <cstring>

using namespace std;

const float TASA_COMISION = 0.10f; // 10% de la venta
const int K = 5;                   // corrimiento fijo para encriptar las claves

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

// ---------- Lectura segura por teclado ----------

// Lee un entero. Si el usuario escribe letras, avisa y vuelve a pedir.
// Si se acaba la entrada (EOF) devuelve 0 para no quedar en un loop infinito.
int leerEntero() {
    int x;
    while (!(cin >> x)) {
        if (cin.eof()) return 0;
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Entrada invalida, ingrese un numero: ";
    }
    return x;
}

// Lee una palabra (sin espacios) sin pasarse del tamano del buffer (50).
void leerTexto(char destino[50]) {
    destino[0] = '\0';
    cin.width(50);
    cin >> destino;
}

// ---------- Fecha ----------

bool fechaValida(const char* f) {
    if (strlen(f) != 10) return false;
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) {
            if (f[i] != '-') return false;
        } else if (f[i] < '0' || f[i] > '9') {
            return false;
        }
    }
    int dia = (f[0] - '0') * 10 + (f[1] - '0');
    int mes = (f[3] - '0') * 10 + (f[4] - '0');
    return dia >= 1 && dia <= 31 && mes >= 1 && mes <= 12;
}

// Pide la fecha una sola vez (repite solo si el formato es invalido).
bool pedirFecha(char fecha[50]) {
    while (true) {
        cout << "Ingrese la fecha del dia (DD-MM-AAAA): ";
        leerTexto(fecha);
        if (fechaValida(fecha)) return true;
        if (cin.eof()) return false;
        cout << "Fecha invalida. Use el formato DD-MM-AAAA (ej: 08-06-2025)." << endl;
    }
}

// ---------- Claves ----------

// Suma K a cada caracter. Se deshace restando K.
void encriptarCadena(const char* entrada, char* salida) {
    int i = 0;
    while (entrada[i] != '\0') {
        salida[i] = entrada[i] + K;
        i++;
    }
    salida[i] = '\0';
}

bool claveCorrecta(const Mozo& m, const char* passTipeada) {
    if (strlen(passTipeada) > 19) return false;   // no entra en char[20]
    char passEncriptada[20];
    encriptarCadena(passTipeada, passEncriptada);
    return strcmp(m.password, passEncriptada) == 0;
}

// ---------- Busquedas ----------

// mozos.dat: busqueda secuencial (es chico y hay que recorrerlo hasta encontrarlo).
bool buscarMozo(int id, Mozo& mOut) {
    FILE* f = fopen("mozos.dat", "rb");
    if (!f) return false;

    Mozo m;
    bool encontrado = false;
    while (!encontrado && fread(&m, sizeof(Mozo), 1, f) == 1) {
        if (m.idMozo == id) {
            mOut = m;
            encontrado = true;
        }
    }
    fclose(f);
    return encontrado;
}

// inventario.dat esta ordenado por codigo (con huecos): busqueda binaria.
// Devuelve la posicion del registro, o -1 si no existe.
long buscarProducto(FILE* f, int codigoBuscado, Producto& p) {
    fseek(f, 0, SEEK_END);
    long n = ftell(f) / sizeof(Producto);

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
    return pos;
}

// ---------- Ordenar la planilla del dia (directamente en el archivo) ----------
// Burbuja sobre el archivo: no usa arrays, asi que no tiene limite de registros.
bool ordenarPlanilla(const char* nombreArchivo) {
    FILE* f = fopen(nombreArchivo, "rb+");
    if (!f) return false;

    fseek(f, 0, SEEK_END);
    long n = ftell(f) / sizeof(Comanda);

    Comanda a, b;
    for (long i = 0; i < n - 1; i++) {
        for (long j = 0; j < n - 1 - i; j++) {
            fseek(f, j * sizeof(Comanda), SEEK_SET);
            fread(&a, sizeof(Comanda), 1, f);
            fread(&b, sizeof(Comanda), 1, f);
            if (a.idMozo > b.idMozo) {
                fseek(f, j * sizeof(Comanda), SEEK_SET);
                fwrite(&b, sizeof(Comanda), 1, f);
                fwrite(&a, sizeof(Comanda), 1, f);
            }
        }
    }
    fclose(f);
    return true;
}

// ---------- Cargar una venta ----------
// Si algo falla, avisa y vuelve al menu (no deja seguir con esa venta).
void cargarVenta(FILE* fInv, const char* nombreArchivo) {
    cout << "\n--- NUEVA VENTA ---" << endl;

    cout << "Numero de mozo: ";
    int idMozo = leerEntero();

    Mozo m;
    if (!buscarMozo(idMozo, m)) {
        cout << "Error: el mozo " << idMozo << " no existe." << endl;
        return;
    }

    cout << "Clave: ";
    char pass[50];
    leerTexto(pass);
    if (!claveCorrecta(m, pass)) {
        cout << "Error: clave incorrecta." << endl;
        return;
    }

    cout << "Codigo de producto: ";
    int codProd = leerEntero();

    Producto prod;
    long posProd = buscarProducto(fInv, codProd, prod);
    if (posProd == -1) {
        cout << "Error: El producto no existe." << endl;
        return;
    }
    if (prod.stockActual <= 0) {
        cout << "Error: El producto no tiene stock." << endl;
        return;
    }

    cout << "Cantidad: ";
    int cantidad = leerEntero();
    if (cantidad <= 0) {
        cout << "Error: la cantidad debe ser mayor a 0." << endl;
        return;
    }
    if (cantidad > prod.stockActual) {
        cout << "Error: Stock insuficiente. Disponible: " << prod.stockActual << endl;
        return;
    }

    Comanda c;
    c.idMozo = idMozo;
    c.codigoProducto = codProd;
    c.cantidad = cantidad;
    c.comision = (prod.precio * cantidad) * TASA_COMISION;

    // "ab": agrega al final y, si la planilla del dia no existe, la crea.
    FILE* fDia = fopen(nombreArchivo, "ab");
    if (!fDia) {
        cout << "Error: no se pudo abrir la planilla del dia." << endl;
        return;
    }
    fwrite(&c, sizeof(Comanda), 1, fDia);
    fclose(fDia);

    prod.stockActual -= cantidad;
    fseek(fInv, posProd * sizeof(Producto), SEEK_SET);
    fwrite(&prod, sizeof(Producto), 1, fInv);
    fflush(fInv);

    cout << "Venta registrada con exito. Comision: $" << c.comision << endl;
}

int main() {
    char fecha[50];
    if (!pedirFecha(fecha)) return 1;

    char nombreArchivo[80];
    sprintf(nombreArchivo, "comandas_%s.dat", fecha);

    FILE* fInv = fopen("inventario.dat", "rb+");
    if (!fInv) {
        cout << "Error: no se pudo abrir inventario.dat" << endl;
        return 1;
    }

    int opcion = -1;
    while (opcion != 0) {
        cout << "\n===== CARGA DE VENTAS - Dia " << fecha << " =====" << endl;
        cout << "1. Cargar una venta" << endl;
        cout << "0. Terminar y ordenar la planilla" << endl;
        cout << "Opcion: ";
        opcion = leerEntero();

        if (opcion == 1) {
            cargarVenta(fInv, nombreArchivo);
        } else if (opcion != 0) {
            cout << "Opcion invalida." << endl;
        }
    }
    fclose(fInv);

    if (ordenarPlanilla(nombreArchivo)) {
        cout << "Planilla " << nombreArchivo << " actualizada y ordenada por mozo." << endl;
    } else {
        cout << "No se cargaron ventas: no se creo planilla para " << fecha << "." << endl;
    }

    return 0;
}