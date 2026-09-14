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
    FILE* fInvt = fopen("inventario.dat", "rb+");

     if (fHist == NULL || fInvt == NULL)
    {
        cout << "No se pudieron abrir los archivos base." << endl;
        return 0;
    }

    Mozo listaMozos[100];
    int cantidadMozos = 0;

    RegistroNormalizado reg[1000];
    int cantidadReg = 0;

    ComandaHistorica comandaHist;
 //  leer todo el historico, armar mozos + registros + actualizar stock 
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
   // armo el registro normalizado y lo guardo en memoria
 strcpy(reg[cantidadReg].fecha, comandaHist.fecha);
        reg[cantidadReg].comanda.idMozo = idMozo;
        reg[cantidadReg].comanda.codigoProducto = comandaHist.codigoProducto;
        reg[cantidadReg].comanda.cantidad = comandaHist.cantidad;
        reg[cantidadReg].comanda.comision = comandaHist.comision;
        cantidadReg++;
       

        // se crea la comanza normalizada 
    };

    fclose(fHist);
    fclose(fInvt);
// le pongo password inicial = su id como texto
    for (int i = 0; i < cantidadMozos; i++)
    {
        char passwordInicial[20];
        sprintf(passwordInicial, "%d", listaMozos[i].idMozo);   // convierto el numero a texto
        otroCodigo(passwordInicial);                              // lo encripto
        strcpy(listaMozos[i].password, passwordInicial);
    }
    
  FILE* fMozos = fopen("mozos.dat", "wb");
    for (int i = 0; i < cantidadMozos; i++)
    {
        fwrite(&listaMozos[i], sizeof(Mozo), 1, fMozos);
    }
    fclose(fMozos);
    //  ordenar TODOS los registros por fecha y, dentro de cada fecha, por mozo
    for (int i = 0; i < cantidadReg - 1; i++)
    {
        for (int j = 0; j < cantidadReg - 1 - i; j++)
        {
            bool debeIntercambiar = false;

            int cmpFecha = strcmp(reg[j].fecha, reg[j + 1].fecha);

            if (cmpFecha > 0)
            {
                debeIntercambiar = true;
            }
            else if (cmpFecha == 0 && reg[j].comanda.idMozo > reg[j + 1].comanda.idMozo)
            {
                debeIntercambiar = true;
            }

            if (debeIntercambiar)
            {
                RegistroNormalizado temp = reg[j];
                reg[j] = reg[j + 1];
                reg[j + 1] = temp;
            }
        }
    }
   // generar un archivo por cada dia distinto
    int i = 0;
    while (i < cantidadReg)
    {
        char fechaActual[11];
        strcpy(fechaActual, reg[i].fecha);

        char nombreArchivo[30];
        sprintf(nombreArchivo, "comandas_%s.dat", fechaActual);

        FILE* fDia = fopen(nombreArchivo, "wb");

        while (i < cantidadReg && strcmp(reg[i].fecha, fechaActual) == 0)
        {
            fwrite(&reg[i].comanda, sizeof(Comanda), 1, fDia);
            i++;
        }

        fclose(fDia);
    }

    cout << "Proceso de normalización completado." << endl;

    return 0;
}
    