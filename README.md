# TP Integrador AyED - El buffet de Alberto

## Integrantes
- Chamorro Rocha, Sabrina - @scoffeerc/@scoffee21 
- Piñero, Santiago José - @spinero-utn
- Cedermas, Valentín Tomás - @valenceder
- Pacheco Lopez, Tobias - @ppachecolopez-coder
- Dermisache, Lautaro Gabriel - @DermisacheLautaro

## Quién hizo cada parte
- normalizacion.cpp: Tobias, Lautaro 
- ventas.cpp: Valentín
- cierre.cpp: Santiago
- resumen.cpp, control de la estructura y verificaciones: Sabrina

## Cómo compilar
    g++ normalizacion.cpp -o normalizacion
    g++ ventas.cpp -o ventas
    g++ cierre.cpp -o cierre
    g++ resumen.cpp -o resumen

## Orden de ejecución
Colocar `comandas_historicas.dat` e `inventario.dat` en la carpeta y correr:
1. `./normalizacion` genera mozos.dat y una planilla por día, y actualiza el stock.
2. `./ventas` pide la fecha una vez y carga ventas con login (mozo y clave).
3. `./cierre` junta los días en comandas_semana_sX-mm.dat.
4. `./resumen` muestra el resumen del cierre por pantalla.

## Cómo resolvimos las partes clave
- **Claves:** a cada carácter se le suma K = 5 y se guarda así en mozos.dat. Para el login se aplica la misma cuenta a lo que tipea el mozo y se compara con strcmp. Se deshace restando K (ej: "sol" pasa a "xtq"). La clave inicial es el ID del mozo como texto.
- **Búsqueda de productos:** binaria, porque el inventario está ordenado por código (con huecos). Los mozos se buscan de forma secuencial.
- **Stock:** se actualiza en el lugar con fseek + fwrite, sin rehacer el archivo. En normalizacion, si la venta supera el stock, queda en 0 con un aviso.
- **Ordenamiento:** por fecha y mozo en normalizacion, y por mozo en ventas (sobre el archivo) y cierre.
- **Resumen:** recorrido con corte de control por mozo. El último mozo se cierra al terminar el archivo.
- **Casos raros:** mozo inexistente, clave incorrecta, producto sin stock o inexistente, planilla del día inexistente, y último mozo del cierre.
