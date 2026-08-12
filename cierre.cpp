struct Mozo {
    int idMozo;
    char nombre[8];     // Siempre char[], nunca string
    char password[9];   // Para el login encriptado
    float totalComision;
};

struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};