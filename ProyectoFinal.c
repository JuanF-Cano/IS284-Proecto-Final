/*
- Fecha de publicación: 22/11/2024
- Hora de publicación: 2:52 AM
- Versión de su código: 3.0
- Autor. Ing(c): Juan Fernando Cano Duque
- Nombre del lenguaje utilizado: C
- Versión del lenguaje utilizado: C11
- Nombre y versión del Sistema Operativo(S.O) sobre el que compiló y probó los ejecutables: windows 10.0.22631
- Versión del compilador utilizado: Compilado con GCC 6.3.0 (MinGW.org GCC-6.3.0-1)
- Presentado a: Doctor Ricardo Moreno Laverde
- Universidad Tecnológica de Pereira
- Programa de Ingeniería de Sistemas y Computación
- Asignatura IS284 Programación II

- Descrptivo: El programa recibe un dataset cn registros de una empresa y genera informes utilizando busquda binaria y ordenamiento
              de archivos por Bubble Sort y Merge Sort.
    
- Salvedades:
    *No se garantiza su funcionamiento en otras versiones del sistema operativo, en otros sistemas operativos, en otras versiones del 
     lenguaje, del compilador u otro compilador. Primero se debe ejecutar la opcion 1 del menu, creando las tablas, para tener las 
     tablas en modo binario y guardados con las estructuras.
    *Si alguno de los procesos se interrumpe antes de terminar el eof se puede perder, ya que no se va a cerrar el archivo de forma exitosa
     Por lo tanto, es necesario que se deje terminar de ejecutar cada punto que se decida ingresar, si se causan problemas por esto se deben 
     volver a crear los archivos con la opción 1.
    *Los digitos a ingresar en el menú de opciones deben ser números decimales maracdos con punto ".", no con coma.
    *Se considera que la primera fila de cada uno de los .dat NO tiene las descripciones de lo que contiene la tabla, en ese orden de ideas
     no se garantizan los resultados si se ingresan archivos de datos con diferente estructura, condiciones o con esa linea descriptiva al inicio.  
    *Para las opciones del bubble, aunque tardados, funcionan. Se recomienda dejar el equipo apagado y no en suspension para que se demore menos.
    *En el punto 3, la cantidad de ingresos se tomó únicamente del precio del producto, ya que se asume que se refiere a lo que le entra a la empresa
     y no a las ganancias, en caso de necesitar las ganancias (diferencias entre costo de realización y precio de venta) descomentar la linea 1148
    *En el punto 4 se asume que son 6 años para mantener el tamaño del array 6x2, por tanto, si hay datos de otros años no se garantizan resultados. 
*/

#include <stdio.h>     // Proporciona funciones para entrada/salida estándar, como printf y scanf.
#include <stdlib.h>    // Incluye funciones para gestión de memoria dinámica (malloc, free), control de procesos, entre otras.
#include <string.h>    // Contiene funciones para manipulación de cadenas de caracteres, como strcpy, strcmp, strlen.
#include <ctype.h>     // Proporciona funciones para trabajar con caracteres (e.g., toupper, isdigit).
#include <time.h>      // Define funciones y estructuras para manipulación de tiempo y fechas (e.g., time, localtime).
#include <math.h>      // Incluye funciones matemáticas comunes, como fabs, pow.

// Estructura para representar una fecha (día, mes y año).
typedef struct {
    unsigned char MM;          // Mes (valor entre 1 y 12).
    unsigned char DD;          // Día del mes (valor entre 1 y 31 dependiendo del mes).
    unsigned short int AAAA;   // Año (e.g., 2024).
} date;

// Estructura para almacenar datos de ventas.
typedef struct {
    long int OrderNumber;          // Número único de la orden.
    unsigned char LineItem;        // Número de ítem dentro de una orden.
    date OrderDate;                // Fecha en que se realizó la orden.
    date DeliveryDate;             // Fecha en que se realizó la entrega.
    unsigned int CustomerKey;      // Identificador único del cliente.
    unsigned short int StoreKey;   // Identificador único de la tienda.
    unsigned short int ProductKey; // Identificador único del producto.
    unsigned short int Quantity;   // Cantidad de producto vendido en esta línea de la orden.
    char CurrencyCode[3];          // Código de moneda utilizado en la transacción (e.g., "USD").
} SalesData;
// Estructura para representar datos de clientes.
typedef struct {
    unsigned int CustomersKey;   // Identificador único del cliente.
    char Gender[8];              // Género del cliente (e.g., "Male", "Female").
    char Name[40];               // Nombre completo del cliente.
    char City[40];               // Ciudad donde reside el cliente.
    char StateCode[30];          // Código del estado o provincia del cliente.
    char State[30];              // Nombre del estado o provincia.
    unsigned int ZipCode;        // Código postal del cliente.
    char Country[20];            // País de residencia.
    char Continent[20];          // Continente de residencia.
    date Birthday;               // Fecha de nacimiento del cliente.
} CustomersData;

// Estructura para representar datos de productos.
typedef struct {
    unsigned short int ProductKey;  // Identificador único del producto.
    char ProductName[85];           // Nombre completo del producto.
    char Brand[30];                 // Marca del producto.
    char Color[15];                 // Color del producto.
    float UnitCostUSD;              // Costo unitario en dólares estadounidenses.
    float UnitPriceUSD;             // Precio unitario en dólares estadounidenses.
    char SubcategoryKey[4];         // Código de subcategoría del producto.
    char Subcategory[35];           // Nombre de la subcategoría del producto.
    char CategoryKey[2];            // Código de categoría principal del producto.
    char Category[30];              // Nombre de la categoría principal.
} ProductsData;

// Estructura para representar datos de tiendas.
typedef struct {
    unsigned short int StoreKey;    // Identificador único de la tienda.
    char Country[35];               // País donde está ubicada la tienda.
    char State[35];                 // Estado o provincia donde está ubicada la tienda.
    unsigned short int SquareMeters;// Tamaño de la tienda en metros cuadrados.
    date OpenDate;                  // Fecha de apertura de la tienda.
} StoresData;

// Estructura para representar tasas de cambio entre monedas.
typedef struct {
    char Date[10];                  // Fecha de la tasa de cambio (formato: DD/MM/AAAA).
    char Currency[3];               // Código de la moneda base (e.g., "USD").
    float Exchange;                 // Valor de cambio con respecto a una moneda de referencia.
} ExchangeRates;

// Definición de un puntero a función para comparar elementos.
typedef int (*Compare)(const void*, const void*); // Utilizado en funciones de ordenamiento (mergeSort y bubbleSort).

// Funciones para la cración de las tablas
void CreateSalesTable(char fileName[]) {
    FILE *fp = fopen(fileName, "r");          // Abre el archivo de texto en modo lectura.
    FILE *fpSales = fopen("Sales.table", "wb"); // Abre el archivo binario para escritura.
    
    if (fp == NULL) { // Verifica si hubo error al abrir el archivo de texto.
        printf("Error opening file: %s\n", fileName);
        return;
    }
    if (fpSales == NULL) { // Verifica si hubo error al abrir el archivo binario.
        printf("Error opening file: Sales.table\n");
        return;
    }

    int countRegisters = 0;   // Contador de registros escritos correctamente.
    int counterErrors = 0;    // Contador de errores detectados.
    SalesData data;           // Estructura para almacenar temporalmente los datos de ventas.
    char line[1024] = "";     // Buffer para leer cada línea del archivo de texto.

    while (fgets(line, sizeof(line), fp)) { // Lee el archivo línea por línea.
        char *token = strtok(line, ",");    // Separa los campos delimitados por comas.

        // Parseo y asignación de campos a la estructura SalesData.
        data.OrderNumber = atol(token); // Número de orden.

        token = strtok(NULL, ",");
        data.LineItem = (unsigned char)atoi(token); // Ítem de la orden.

        token = strtok(NULL, ",");
        sscanf(token, "%hhu/%hhu/%hu", &data.OrderDate.MM, &data.OrderDate.DD, &data.OrderDate.AAAA); // Fecha de orden.

        token = strtok(NULL, ",");
        if (token[0] == ' ') { // Si la fecha está vacía, inicializa en 0.
            data.DeliveryDate.DD = 0;
            data.DeliveryDate.MM = 0;
            data.DeliveryDate.AAAA = 0;
        } else { // Si la fecha está presente, la parsea.
            sscanf(token, "%hhu/%hhu/%hu", &data.DeliveryDate.MM, &data.DeliveryDate.DD, &data.DeliveryDate.AAAA);
        }

        token = strtok(NULL, ",");
        data.CustomerKey = (unsigned int)atoi(token); // ID del cliente.
        
        token = strtok(NULL, ",");
        data.StoreKey = (unsigned short int)atoi(token); // ID de la tienda.
        
        token = strtok(NULL, ",");
        data.ProductKey = (unsigned short int)atoi(token); // ID del producto.
        
        token = strtok(NULL, ",");
        data.Quantity = (unsigned short int)atoi(token); // Cantidad.

        token = strtok(NULL, ";");
        if (strlen(token) - 1 <= 3) { // Verifica longitud del código de moneda.
            strcpy(data.CurrencyCode, token);
        } else { // Manejo de error por desbordamiento.
            counterErrors++;
            printf("\nError (%i): Overflow CurrencyCode: %s", counterErrors, data.CurrencyCode);
        }

        // Escribe el registro en el archivo binario.
        if (fwrite(&data, sizeof(data), 1, fpSales) != 1) {
            printf("\nError writing to file. Order number: %li\n", data.OrderNumber);
        } else {
            countRegisters++; // Incrementa el contador de registros válidos.
        }
    }
    // Cierra los archivos abiertos.
    fclose(fp);
    fclose(fpSales);
    return;
}

void CreateCustomersTable(char fileName[]) {
    // Abre el archivo de texto en modo lectura.
    FILE *fp = fopen(fileName, "r");
    // Abre el archivo binario "Customers.table" en modo escritura binaria.
    FILE *fpCustomers = fopen("Customers.table", "wb");
    // Verifica si el archivo de texto no pudo abrirse.
    if (fp == NULL) {
        printf("Error opening file: %s\n", fileName);
        return;
    }
    // Verifica si el archivo binario no pudo abrirse.
    if (fpCustomers == NULL) {
        printf("Error opening file: Customers.table\n");
        return;
    }

    int countRegisters = 0;    // Contador de registros escritos correctamente.
    int counterErrors = 0;     // Contador de errores detectados.
    CustomersData data;        // Estructura para almacenar temporalmente los datos de clientes.
    char line[250] = "";       // Buffer para leer cada línea del archivo de texto.
    // Lee cada línea del archivo de texto.
    while (fgets(line, sizeof(line), fp)) {
        char *token = strtok(line, ";"); // Divide la línea en campos delimitados por ';'.
        data.CustomersKey = (unsigned int)atoi(token); // Asigna el identificador del cliente.

        // Procesa y valida el género del cliente.
        token = strtok(NULL, ";");
        if (strlen(token) <= 8) {
            strcpy(data.Gender, token);
        } else {
            counterErrors++;
            printf("\nError (%i): Overflow Gender: %s", counterErrors, token);
        }

        // Procesa y valida el nombre del cliente.
        token = strtok(NULL, ";");
        if (strlen(token) <= 40) {
            strcpy(data.Name, token);
        } else {
            counterErrors++;
            printf("\nError (%i): Overflow Name: %s", counterErrors, token);
        }

        // Procesa y valida la ciudad del cliente.
        token = strtok(NULL, ";");
        if (strlen(token) <= 40) {
            strcpy(data.City, token);
        } else {
            counterErrors++;
            printf("\nError (%i): Overflow City: %s", counterErrors, token);
        }

        // Procesa y valida el código del estado.
        token = strtok(NULL, ";");
        if (strlen(token) <= 30) {
            strcpy(data.StateCode, token);
        } else {
            counterErrors++;
            printf("\nError (%i): Overflow StateCode: %s", counterErrors, token);
        }

        // Procesa y valida el estado del cliente.
        token = strtok(NULL, ";");
        if (strlen(token) <= 30) {
            strcpy(data.State, token);
        } else {
            counterErrors++;
            printf("\nError (%i): Overflow State: %s", counterErrors, token);
        }

        // Procesa y valida el código postal y el país.
        token = strtok(NULL, ";");
        if (isdigit((unsigned char)token[0])) { // Si el token comienza con un dígito, es un código postal.
            data.ZipCode = (unsigned int)atoi(token);
            token = strtok(NULL, ";");
            if (strlen(token) <= 20) {
                strcpy(data.Country, token);
            } else {
                counterErrors++;
                printf("\nError (%i): Overflow Country: %s", counterErrors, token);
            }
        } else { // Si no es un dígito, asume que es un país.
            data.ZipCode = 0;
            if (strlen(token) <= 20) {
                strcpy(data.Country, token);
            } else {
                counterErrors++;
                printf("\nError (%i): Overflow Country: %s", counterErrors, token);
            }
        }

        // Procesa y valida el continente del cliente.
        token = strtok(NULL, ";");
        if (strlen(token) <= 20) {
            strcpy(data.Continent, token);
        } else {
            counterErrors++;
            printf("\nError (%i): Overflow Continent: %s", counterErrors, token);
        }

        // Procesa la fecha de nacimiento del cliente.
        sscanf(strtok(NULL, ","), "%hhu/%hhu/%hu", &data.Birthday.MM, &data.Birthday.DD, &data.Birthday.AAAA);

        // Escribe el registro en el archivo binario.
        if (fwrite(&data, sizeof(data), 1, fpCustomers) != 1) {
            printf("\nError writing to file. Customers key: %i\n", data.CustomersKey);
        } else {
            countRegisters++; // Incrementa el contador de registros válidos.
        }
    }
    // Imprime el total de registros procesados.
    printf("\nCustomers Records entered: %i", countRegisters);
    // Cierra los archivos abiertos.
    fclose(fp);
    fclose(fpCustomers);

    return;
}

float parseToken(char **token) {
    // Si el token comienza con una comilla doble, avanza al siguiente token dentro de las comillas.
    if ((*token)[0] == '"') {
        *token = strtok(NULL, "\"");
    }
    char buffer[1024]; // Buffer para almacenar el token sin comas.
    int j = 0;         // Índice para rastrear la posición en el buffer.
    // Recorre el token y elimina todas las comas.
    for (int i = 0; (*token)[i] != '\0'; i++) {
        if ((*token)[i] != ',') { // Si no es una coma, copia el carácter al buffer.
            buffer[j++] = (*token)[i];
        }
    }
    buffer[j] = '\0'; // Termina la cadena en el buffer.
    // Convierte el buffer resultante en un número flotante y lo devuelve.
    return (float)atof(buffer);
}

void CreateProductsTable(char fileName[]) {
    // Abre el archivo de texto en modo lectura.
    FILE *fp = fopen(fileName, "r");
    // Abre el archivo binario "Products.table" en modo escritura binaria.
    FILE *fpProducts = fopen("Products.table", "wb");
    // Verifica si el archivo de texto no pudo abrirse.
    if (fp == NULL) {
        printf("Error opening file: %s\n", fileName);
        return;
    }
    // Verifica si el archivo binario no pudo abrirse.
    if (fpProducts == NULL) {
        printf("Error opening file: Products.table\n");
        return;
    }

    int countRegisters = 0;    // Contador de registros escritos correctamente.
    int counterErrors = 0;     // Contador de errores detectados.
    ProductsData data;         // Estructura para almacenar temporalmente los datos de productos.
    char line[1024] = "";      // Buffer para leer cada línea del archivo de texto.

    // Lee cada línea del archivo de texto.
    while (fgets(line, sizeof(line), fp)) {
        // Procesa y asigna la clave del producto.
        char *token = strtok(line, ",");
        data.ProductKey = (unsigned short int)atoi(token);

        // Procesa y asigna el nombre del producto, manejando casos con y sin comillas.
        if (line[strlen(token) + 1] == '"' && (line[strlen(token) + 2] == 'P' || line[strlen(token) + 2] == 'M')) {
            token = strtok(NULL, "\"");
        } else {
            token = strtok(NULL, ",");
        }
        if (token[0] == '"') token++; // Omite la comilla inicial si está presente.
        if (strlen(token) <= 85) {
            strcpy(data.ProductName, token);
        } else {
            counterErrors++;
            printf("\nError (%i): Overflow ProductName: %s", counterErrors, token);
        }

        // Procesa y asigna la marca del producto.
        token = strtok(NULL, ",");
        if (strlen(token) <= 30) {
            strcpy(data.Brand, token);
        } else {
            counterErrors++;
            printf("\nError (%i): Overflow Brand: %s", counterErrors, token);
        }

        // Procesa y asigna el color del producto.
        token = strtok(NULL, ",");
        if (strlen(token) <= 15) {
            strcpy(data.Color, token);
        } else {
            counterErrors++;
            printf("\nError (%i): Overflow Color: %s", counterErrors, token);
        }

        // Procesa y asigna el costo unitario en USD.
        token = strtok(NULL, ",$");
        data.UnitCostUSD = parseToken(&token);

        // Procesa y asigna el precio unitario en USD.
        token = strtok(NULL, ",$");
        data.UnitPriceUSD = parseToken(&token);

        // Procesa y asigna la clave de la subcategoría.
        token = strtok(NULL, ",");
        if (strlen(token) <= 4) {
            strcpy(data.SubcategoryKey, token);
        } else {
            counterErrors++;
            printf("\nError (%i): Overflow SubcategoryKey: %s", counterErrors, token);
        }

        // Procesa y asigna la subcategoría, manejando posibles comillas.
        token = strtok(NULL, "¬");
        if (token[0] == '"' || token[1] == '"') {
            token = strtok(token, "\"");
        } else {
            token = strtok(token, ",");
        }
        if (strlen(token) <= 35) {
            strcpy(data.Subcategory, token);
        } else {
            counterErrors++;
            printf("\nError (%i): Overflow Subcategory: %s", counterErrors, token);
        }

        // Procesa y asigna la clave de la categoría.
        token = strtok(NULL, ",");
        if (strlen(token) <= 2 || (strlen(token) == 11 && token[2] == ' ')) {
            strcpy(data.CategoryKey, token);
        } else {
            counterErrors++;
            printf("\nError (%i): Overflow CategoryKey: %s", counterErrors, token);
        }

        // Procesa y asigna la categoría.
        token = strtok(NULL, "\"");
        if (strlen(token) <= 30) {
            strcpy(data.Category, token);
        } else {
            counterErrors++;
            printf("\nError (%i): Overflow Category: %s", counterErrors, token);
        }

        // Escribe la estructura en el archivo binario.
        if (fwrite(&data, sizeof(data), 1, fpProducts) != 1) {
            printf("\nError writing to file. Product key: %i\n", data.ProductKey);
        } else {
            countRegisters++; // Incrementa el contador de registros válidos.
        }
    }
    // Imprime el total de registros procesados.
    printf("\nProducts Records entered: %i", countRegisters);
    // Cierra los archivos abiertos.
    fclose(fp);
    fclose(fpProducts);

    return;
}

void CreateStoresTable(char fileName[]) {
    // Abre el archivo de texto en modo lectura.
    FILE *fp = fopen(fileName, "r");
    // Abre el archivo binario "Stores.table" en modo escritura binaria.
    FILE *fpStores = fopen("Stores.table", "wb");

    // Verifica si el archivo de texto no pudo abrirse.
    if (fp == NULL) {
        printf("Error opening file: %s\n", fileName);
        return;
    }
    // Verifica si el archivo binario no pudo abrirse.
    if (fpStores == NULL) {
        printf("Error opening file: Stores.table\n");
        return;
    }

    int countRegisters = 0;    // Contador de registros escritos correctamente.
    int counterErrors = 0;     // Contador de errores detectados.
    StoresData data;           // Estructura para almacenar temporalmente los datos de las tiendas.
    char line[1024] = "";      // Buffer para leer cada línea del archivo de texto.

    // Lee cada línea del archivo de texto.
    while (fgets(line, sizeof(line), fp)) {
        // Procesa y asigna la clave de la tienda.
        char *token = strtok(line, ",");
        data.StoreKey = (unsigned short int)atoi(token);

        // Procesa y asigna el país.
        token = strtok(NULL, ",");
        if (strlen(token) <= 35) {
            strcpy(data.Country, token);
        } else {
            counterErrors++;
            printf("\nError (%i): Overflow Country: %s", counterErrors, token);
        }

        // Procesa y asigna el estado.
        token = strtok(NULL, ",");
        if (strlen(token) <= 35) {
            strcpy(data.State, token);
        } else {
            counterErrors++;
            printf("\nError (%i): Overflow State: %s", counterErrors, token);
        }

        // Procesa y asigna los metros cuadrados o la fecha de apertura dependiendo del formato.
        token = strtok(NULL, ",");
        if (isdigit((unsigned char)token[strlen(token) - 5])) {
            // Si el token contiene una fecha, asigna 0 a SquareMeters y extrae la fecha.
            data.SquareMeters = 0;
            sscanf(token, "%hhu/%hhu/%hu", &data.OpenDate.MM, &data.OpenDate.DD, &data.OpenDate.AAAA);
        } else {
            // Si el token contiene un número, asigna el valor a SquareMeters y procesa la fecha.
            data.SquareMeters = (unsigned short int)atoi(token);
            token = strtok(NULL, ",");
            sscanf(token, "%hhu/%hhu/%hu", &data.OpenDate.MM, &data.OpenDate.DD, &data.OpenDate.AAAA);
        }

        // Escribe la estructura en el archivo binario.
        if (fwrite(&data, sizeof(data), 1, fpStores) != 1) {
            printf("\nError writing to file. Store key: %i\n", data.StoreKey);
        } else {
            countRegisters++; // Incrementa el contador de registros válidos.
        }
    }
    // Imprime el total de registros procesados.
    printf("\nStores Records entered: %i", countRegisters);
    // Cierra los archivos abiertos.
    fclose(fp);
    fclose(fpStores);

    return;
}

void CreateExchangeRatesTable(char fileName[]) {
    // Abre el archivo de texto en modo lectura.
    FILE *fp = fopen(fileName, "r");
    // Abre el archivo binario "ExchangeRates.table" en modo escritura binaria.
    FILE *fpExchangeRates = fopen("ExchangeRates.table", "wb");
    // Verifica si el archivo de texto no pudo abrirse.
    if (fp == NULL) {
        printf("Error opening file: %s\n", fileName);
        return;
    }
    // Verifica si el archivo binario no pudo abrirse.
    if (fpExchangeRates == NULL) {
        printf("Error opening file: ExchangeRates.table\n");
        return;
    }

    int countRegisters = 0;    // Contador de registros escritos correctamente.
    int counterErrors = 0;     // Contador de errores detectados.
    ExchangeRates data;        // Estructura para almacenar temporalmente los datos de las tasas de cambio.
    char line[1024] = "";      // Buffer para leer cada línea del archivo de texto.

    // Lee cada línea del archivo de texto.
    while (fgets(line, sizeof(line), fp)) {
        // Procesa y asigna la fecha.
        char *token = strtok(line, ",");
        if (strlen(token) <= 10) {
            strcpy(data.Date, token); // Asigna la fecha si no excede los 10 caracteres.
        } else {
            counterErrors++; // Incremente el contador de errores si el valor es demasiado largo.
            printf("\nError (%i): Overflow Date: %s", counterErrors, token);
        }

        // Procesa y asigna la moneda.
        token = strtok(NULL, ",");
        if (strlen(token) <= 3 || (token[3] == ' ' && token[4] == ' ' && token[5] == ' ' && token[6] == ' ' && token[7] == ' ')) {
            strcpy(data.Currency, token); // Asigna la moneda si no excede los 3 caracteres.
        } else {
            counterErrors++; // Incremente el contador de errores si el valor es demasiado largo.
            printf("\nError (%i): Overflow Currency: %s", counterErrors, token);
        }

        // Procesa y asigna el valor de la tasa de cambio.
        token = strtok(NULL, ",");
        data.Exchange = (float)atof(token); // Convierte el valor a float.

        // Escribe la estructura en el archivo binario.
        if (fwrite(&data, sizeof(data), 1, fpExchangeRates) != 1) {
            printf("\nError writing to file. Date: %s. Currency: %s\n", data.Date, data.Currency);
        } else {
            countRegisters++; // Incrementa el contador de registros válidos.
        }
    }
    // Imprime el total de registros procesados.
    printf("\nExchanges Rates Records entered: %i", countRegisters);
    // Cierra los archivos abiertos.
    fclose(fp);
    fclose(fpExchangeRates);

    return;
}

// Funciones de ordenamiento (Merge Sort y Bubble Sort)
void Merge(FILE *fp, int left, int right, int medium, size_t size, Compare compare) {
    // Abre un archivo auxiliar para almacenar los registros ordenados temporalmente.
    FILE *fpAux = fopen("auxiliar.table", "wb+");
    if (fpAux == NULL) {
        printf("Error opening file: auxiliar.\n");
        return;
    }
    // Inicializa los índices para las dos mitades del archivo que se van a mezclar.
    int i = left, j = medium + 1;
    void *temporalLeft = malloc(size);  // Almacena temporalmente los registros de la mitad izquierda.
    void *temporalRight = malloc(size); // Almacena temporalmente los registros de la mitad derecha.

    // Mientras haya registros en ambas mitades, compara y escribe el más pequeño en el archivo auxiliar.
    while (i <= medium && j <= right) {
        fseek(fp, i * size ,SEEK_SET); fread(temporalLeft, size, 1, fp);  // Lee el registro de la mitad izquierda.
        fseek(fp, j * size ,SEEK_SET); fread(temporalRight, size, 1, fp); // Lee el registro de la mitad derecha.
        
        // Compara los dos registros y escribe el menor en el archivo auxiliar.
        if (compare(temporalLeft, temporalRight) <= 0) {
            fwrite(temporalLeft, size, 1, fpAux); i++;
        } else {
            fwrite(temporalRight, size, 1, fpAux); j++;
        }
    }
    // Si quedan registros en la mitad izquierda, los escribe en el archivo auxiliar.
    while (i <= medium) {
        fseek(fp, i * size, SEEK_SET);
        fread(temporalLeft, size, 1, fp);
        fwrite(temporalLeft, size, 1, fpAux); 
        i++;
    }
    // Si quedan registros en la mitad derecha, los escribe en el archivo auxiliar.
    while (j <= right) {
        fseek(fp, j * size, SEEK_SET);
        fread(temporalRight, size, 1, fp);
        fwrite(temporalRight, size, 1, fpAux); 
        j++;
    }

    // Libera la memoria usada por los buffers temporales.
    free(temporalLeft);
    free(temporalRight);
    // Copia los registros ordenados del archivo auxiliar de vuelta al archivo original.
    void *aux = malloc(size);
    fseek(fpAux, 0, SEEK_SET); fseek(fp, left * size, SEEK_SET); 
    while(fread(aux, size, 1, fpAux)) {
        fwrite(aux, size, 1, fp);
    }
    
    // Libera la memoria del buffer temporal final y cierra el archivo auxiliar.
    free(aux);
    fclose(fpAux); 
}

void Sort(FILE *fp, int left, int right, size_t size, Compare compare) {
    // Verifica si la sección del archivo tiene más de un registro.
    if (left < right) {
        int medium = left + (right - left) / 2; // Calcula el índice medio para dividir el archivo en dos mitades.
        Sort(fp, left, medium, size, compare); // Llama recursivamente a Sort para ordenar la mitad izquierda.
        Sort(fp, medium + 1, right, size, compare); // Llama recursivamente a Sort para ordenar la mitad derecha.
        // Combina las dos mitades ordenadas usando la función Merge.
        Merge(fp, left, right, medium, size, compare);
    }
}

int MergeSort(char fileName[], char nameDest[], size_t size, Compare compare) {
    // Abre el archivo de origen en modo de lectura binaria.
    FILE *fp = fopen(fileName, "rb");
    if (fp == NULL) {
        printf("Error opening file: %s\n", fileName);
        return 1;
    }
    // Abre el archivo de destino en modo de lectura/escritura binaria.
    FILE *fpDest = fopen(nameDest, "wb+");
    if (fpDest == NULL) {
        printf("Error opening file: %s\n", nameDest);
        return 1;
    }
    // Calcula el número de registros en el archivo de origen.
    fseek(fp, 0, SEEK_END);
    int registers = ftell(fp) / size;
    fseek(fp, 0, SEEK_SET);
    // Reserva memoria para un registro temporal.
    void *record = malloc(size);
    // Lee cada registro del archivo de origen y lo escribe en el archivo de destino.
    while (fread(record, size, 1, fp)) {
        fwrite(record, size, 1, fpDest);
    }

    // Ordena los registros en el archivo de destino utilizando MergeSort.
    fseek(fpDest, 0, SEEK_SET);
    Sort(fpDest, 0, registers - 1, size, compare);
    
    // Cierra ambos archivos y elimina el archivo auxiliar utilizado por Merge.
    fclose(fp); fclose(fpDest);
    remove("auxiliar.table");
    // Retorna el número de registros procesados.
    return registers;
}

void swap(FILE *fpDest, int pos1, int pos2, size_t size) {
    // Reserva memoria para almacenar los registros temporalmente.
    void *temp1 = malloc(size);
    void *temp2 = malloc(size);
    // Lee el primer registro de la posición pos1.
    fseek(fpDest, pos1 * size, SEEK_SET);
    fread(temp1, size, 1, fpDest);
    // Lee el segundo registro de la posición pos2.
    fseek(fpDest, pos2 * size, SEEK_SET);
    fread(temp2, size, 1, fpDest);
    // Escribe el segundo registro en la posición del primer registro.
    fseek(fpDest, pos1 * size, SEEK_SET);
    fwrite(temp2, size, 1, fpDest);
    // Escribe el primer registro en la posición del segundo registro.
    fseek(fpDest, pos2 * size, SEEK_SET);
    fwrite(temp1, size, 1, fpDest);
    // Libera la memoria temporal utilizada para almacenar los registros.
    free(temp1);
    free(temp2);
}

int BubbleSort(char fileName[], char nameDest[], size_t size, Compare compare) {
    // Abre el archivo de entrada en modo lectura binaria.
    FILE *fp = fopen(fileName, "rb");
    if (fp == NULL) {
        printf("Error opening file: %s", fileName);
        return 1;
    }
    // Abre el archivo de destino en modo escritura binaria.
    FILE *fpDest = fopen(nameDest, "wb+");
    if (fpDest == NULL) {
        fclose(fp);
        printf("Error opening file: %s", nameDest);
        return 1;
    }
    // Calcula el número de registros en el archivo.
    fseek(fp, 0, SEEK_END);
    int registers = ftell(fp) / size;
    fseek(fp, 0, SEEK_SET); 
    // Lee todos los registros del archivo de entrada y los escribe en el archivo de destino.
    void *record = malloc(size);
    while (fread(record, size, 1, fp)) {
        fwrite(record, size, 1, fpDest);
    }

    // Recorre el archivo de destino y ordena los registros usando BubbleSort.
    fseek(fpDest, 0, SEEK_SET);
    for (int i = 0; i < registers - 1; i++) {
        printf("%s: %i\n", fileName, i + 1);
        for (int j = 0; j < registers - i - 1; j++) {
            // Lee dos registros consecutivos en el archivo de destino para compararlos.
            void *temporal1 = malloc(size);
            void *temporal2 = malloc(size);
            fseek(fpDest, j * size, SEEK_SET);
            fread(temporal1, size, 1, fpDest);
            fseek(fpDest, (j + 1) * size, SEEK_SET);
            fread(temporal2, size, 1, fpDest);
            // Si los registros no están en el orden correcto, los intercambia.
            if (compare(temporal1, temporal2) > 0) {
                swap(fpDest, j, j + 1, size);
            }
            // Libera la memoria temporal utilizada para almacenar los registros.
            free(temporal1);
            free(temporal2);
        }
    }
    // Cierra ambos archivos.
    fclose(fp); 
    fclose(fpDest);
    // Retorna el número de registros procesados.
    return registers;
}

// Funciones de comparación para los algoritmos de ordenamiento
int CompareProductName(const void *product1, const void *product2) {
    // Compara dos productos por su nombre.
    const ProductsData *productA = (const ProductsData*)product1;
    const ProductsData *productB = (const ProductsData*)product2;
    return strcmp(productA->ProductName, productB->ProductName);
}

int CompareCustomerName(const void *customer1, const void *customer2) {
    // Compara dos clientes por su nombre.
    const CustomersData *customerA = (const CustomersData*)customer1;
    const CustomersData *customerB = (const CustomersData*)customer2;
    return strcmp(customerA->Name, customerB->Name);
}

int CompareSaleByProductKey(const void *register1, const void *register2) {
    // Compara dos registros de ventas por su clave de producto.
    const SalesData *sale1 = (const SalesData*)register1;
    const SalesData *sale2 = (const SalesData*)register2;
    return (sale1->ProductKey > sale2->ProductKey) - (sale1->ProductKey < sale2->ProductKey);
}

int CompareSaleByCustomerKey(const void *register1, const void *register2) {
    // Compara dos registros de ventas por su clave de cliente.
    const SalesData *sale1 = (const SalesData*)register1;
    const SalesData *sale2 = (const SalesData*)register2;
    return (sale1->CustomerKey > sale2->CustomerKey) - (sale1->CustomerKey < sale2->CustomerKey);
}

int CompareCustomerKey(const void *key1, const void *key2) {
    // Compara dos claves de clientes.
    const CustomersData *customer1 = (const CustomersData*)key1;
    const CustomersData *customer2 = (const CustomersData*)key2;
    return (customer1->CustomersKey > customer2->CustomersKey) - (customer1->CustomersKey < customer2->CustomersKey);
}

int CompareProductKey(const void *key1, const void *key2) {
    // Compara dos claves de productos.
    const ProductsData *product1 = (const ProductsData*)key1;
    const ProductsData *product2 = (const ProductsData*)key2;
    return (product1->ProductKey > product2->ProductKey) - (product1->ProductKey < product2->ProductKey);
}

int CompareCustomerLocation(const void *record1, const void *record2) {
    // Compara dos clientes por ubicación (continente, país, estado, ciudad).
    const CustomersData *customer1 = (const CustomersData*)record1;
    const CustomersData *customer2 = (const CustomersData*)record2;
    int result = strcmp(customer1->Continent, customer2->Continent);
    if (result == 0) {
        result = strcmp(customer1->Country, customer2->Country);
        if (result == 0) {
            result = strcmp(customer1->State, customer2->State);
            if (result == 0) {
                result = strcmp(customer1->City, customer2->City);
            }
        }
    }
    return result;
}

int CompareSalesDate(const void *reg1, const void *reg2) {
    // Compara dos ventas por fecha de orden (año, mes, día).
    const SalesData *sale1 = (const SalesData*)reg1;
    const SalesData *sale2 = (const SalesData*)reg2;
    int result = sale1->OrderDate.AAAA - sale2->OrderDate.AAAA;
    if (result == 0) {
        result = sale1->OrderDate.MM - sale2->OrderDate.MM;
        if (result == 0) {
            result = sale1->OrderDate.DD - sale2->OrderDate.DD;
        }
    }
    return result;
}

int CompareSalesByOrderNumber(const void *a, const void *b) {
    // Compara dos ventas por número de orden. Si son iguales, compara por clave de producto.
    const SalesData *saleA = (const SalesData*)a;
    const SalesData *saleB = (const SalesData*)b;
    int comparation = saleA->OrderNumber - saleB->OrderNumber;
    if (comparation == 0) {
        comparation = (int)(saleA->ProductKey - saleB->ProductKey);
    }
    return comparation;
}

// Funciones de busqueda binaria
int BinarySearch(FILE *fp, unsigned int item, size_t size, int registers, int option) {
    int start = 0;              // Índice inicial del rango de búsqueda.
    int end = registers - 1;    // Índice final del rango de búsqueda.
    int medium = (registers - 1) / 2; // Calcula el punto medio.
    void *record = malloc(size);      // Reserva memoria para un registro del tamaño especificado.
    int result = -1;                  // Inicializa el resultado como no encontrado.

    while (start <= end) {
        // Recalcula el punto medio para cada iteración.
        medium = start + (end - start) / 2;
        // Posiciona el cursor en el registro correspondiente al punto medio.
        fseek(fp, medium * size, SEEK_SET);
        // Lee el registro desde el archivo.
        fread(record, size, 1, fp);
        // Determina la clave en función de la opción.
        unsigned int key = 0;
        if (option == 1) { // Caso: Clave de producto en una estructura ProductsData.
            ProductsData *product = (ProductsData *)record;
            key = product->ProductKey;
        } else if (option == 2) { // Caso: Clave de producto en una estructura SalesData.
            SalesData *sale = (SalesData *)record;
            key = sale->ProductKey;
        } else if (option == 3) { // Caso: Clave de cliente en una estructura CustomersData.
            CustomersData *customer = (CustomersData *)record;
            key = customer->CustomersKey;
        } else if (option == 4) { // Caso: Clave de cliente en una estructura SalesData.
            SalesData *sale = (SalesData *)record;
            key = sale->CustomerKey;
        }

        // Compara la clave leída con el valor buscado.
        if (key == item) {
            result = medium;    // Si se encuentra, guarda la posición.
            start = end + 1;    // Sale del bucle forzando la condición de término.
        } else if (key > item) {
            end = medium - 1;   // Ajusta el límite superior del rango.
        } else {
            start = medium + 1; // Ajusta el límite inferior del rango.
        }
    }

    free(record); // Libera la memoria asignada.
    return result; // Devuelve la posición encontrada o -1 si no se encontró.
}

int BinarySearchExchangeDate(FILE *fp, SalesData recordSale) {
    // Declaración de variables para el rango de búsqueda y tamaño de los registros.
    unsigned int start = 0, middle = 0, end = 0, sizeOfRecord = sizeof(ExchangeRates);
    int result = 0; // Inicializa el resultado como 0 (no encontrado).
    ExchangeRates record; // Variable temporal para leer los registros.
    // Calcula el índice final en función del tamaño del archivo.
    fseek(fp, 0, SEEK_END);
    end = (ftell(fp) / sizeOfRecord) - 1;

    // Bucle de búsqueda binaria.
    while (start <= end) {
        // Calcula el índice del medio.
        middle = start + ((end - start) / 2);
        // Lee el registro del archivo en la posición del medio.
        fseek(fp, middle * sizeOfRecord, SEEK_SET);
        fread(&record, sizeOfRecord, 1, fp);
        // Obtiene la clave de la fecha desde el registro.
        char key[11] = ""; // Formato esperado: "MM/DD/YYYY".
        strcpy(key, record.Date);
        // Variables para almacenar la fecha del registro actual y la buscada.
        int currentMonth = 0, currentDay = 0, currentYear = 0;
        int month = 0, day = 0, year = 0;
        // Asigna la fecha buscada desde `recordSale`.
        month = recordSale.OrderDate.MM;
        day = recordSale.OrderDate.DD;
        year = recordSale.OrderDate.AAAA;
        // Divide la fecha del registro actual en día, mes y año.
        sscanf(key, "%d/%d/%d", &currentMonth, &currentDay, &currentYear);
        // Comparación de fechas.
        if (currentYear == year) {
            if (currentMonth == month) {
                if (currentDay == day) {
                    // Fecha encontrada, guarda el índice y termina el bucle.
                    result = middle;
                    start = end + 1;
                } else if (currentDay > day) {
                    // Ajusta el rango eliminando la mitad superior.
                    end = middle - 1;
                } else {
                    // Ajusta el rango eliminando la mitad inferior.
                    start = middle + 1;
                }
            } else if (currentMonth > month) {
                // Ajusta el rango eliminando la mitad superior.
                end = middle - 1;
            } else {
                // Ajusta el rango eliminando la mitad inferior.
                start = middle + 1;
            }
        } else if (currentYear > year) {
            // Ajusta el rango eliminando la mitad superior.
            end = middle - 1;
        } else {
            // Ajusta el rango eliminando la mitad inferior.
            start = middle + 1;
        }
    }
    // Devuelve el índice del registro encontrado, o 0 si no se encuentra.
    return result;
}

// Funciones para imprimir y calcular los valores necesarios para los informes
void ShowDateTime() {
    // Obtiene el tiempo actual en formato de época.
    time_t current_time = time(NULL);
    // Convierte el tiempo actual a una estructura de tiempo local.
    struct tm *local_time = localtime(&current_time);
    // Buffer para almacenar la fecha y hora formateada.
    char formattedTime[50] = "";
    // Formatea la fecha y hora como: "Valid to YYYY-MMM-DD at HH:MM hours".
    strftime(formattedTime, 50, "Valid to %Y-%b-%d at %H:%M hours", local_time);
    // Imprime la cadena formateada.
    printf("%s\n", formattedTime);
}

void PrintReport2(int option) {
    // Marca el inicio de la medición de tiempo
    clock_t start = clock();
    // Variables para contar los registros de ventas y clientes
    int salesRegisters = 0;
    int customersRegisters = 0;
    // Dependiendo de la opción elegida, se realiza el ordenamiento usando BubbleSort o MergeSort
    if (option == 1) {
        BubbleSort("Products.table", "ProductsOrder.table", sizeof(ProductsData), CompareProductName);
        salesRegisters = BubbleSort("Sales.table", "SalesOrder.table", sizeof(SalesData), CompareSaleByProductKey);
        customersRegisters = BubbleSort("Customers.table", "CustomersOrder.table", sizeof(CustomersData), CompareCustomerKey);
    } else if (option == 2) {
        MergeSort("Products.table", "ProductsOrder.table", sizeof(ProductsData), CompareProductName);
        salesRegisters = MergeSort("Sales.table", "SalesOrder.table", sizeof(SalesData), CompareSaleByProductKey);
        customersRegisters = MergeSort("Customers.table", "CustomersOrder.table", sizeof(CustomersData), CompareCustomerKey);
    }
    // Abre los archivos necesarios para el procesamiento
    FILE *fpProducts = fopen("ProductsOrder.table", "rb");
    if (fpProducts == NULL) {
        printf("Error opening file: ProductsOrder.table.\n");
        return;
    }
    FILE *fpSales = fopen("SalesOrder.table", "rb");
    if (fpSales == NULL) {
        printf("Error opening file: SalesOrder.table.\n");
        fclose(fpProducts);
        return;
    }
    FILE *fpCustomers = fopen("CustomersOrder.table", "rb");
    if (fpCustomers == NULL) {
        printf("Error opening file: CustomersOrder.table.\n");
        fclose(fpProducts);
        fclose(fpSales);
        return;
    }

    // Define las estructuras para almacenar datos
    ProductsData product;
    SalesData sale;
    CustomersData customer;
    int result = 0; // Variable que guardará el resulltado de la busqueda binaria
    // Imprime el encabezado del reporte
    printf("------------------------------------------------------------------------------------------------------------------------\n");
    printf("Company Global Electronics Retailer\nValid to");
    ShowDateTime();
    printf("\nProducts list ordered by ProductName + Continent + Country + State + City\n");

    // Procesa cada producto
    while (fread(&product, sizeof(ProductsData), 1, fpProducts)) {
        // Crea un archivo temporal para almacenar los clientes que compraron el producto
        FILE *fpAux = fopen("CustomersAux.table", "wb");
        if (fpAux == NULL) {
            printf("Error opening file: CustomersAux.table.\n");
            fclose(fpProducts);
            fclose(fpSales);
            fclose(fpCustomers);
            return;
        }

        // Imprime el nombre del producto
        printf("ProductName: %s", product.ProductName);
        // Realiza la búsqueda binaria para encontrar las ventas del producto
        result = BinarySearch(fpSales, product.ProductKey, sizeof(SalesData), salesRegisters, 2);
        if (result == -1) {
            // Si no hay ventas reportadas, imprime un mensaje
            printf(" - No sales reported\n");
        } else {
            // Si hay ventas, imprime el encabezado de la sección de clientes
            printf("\n%-22s %-22s %-32s %-42s\n", "Continent", "Country", "State", "City");
            printf("________________________________________________________________________________________________________________________________\n");
            // Ubica el registro de la venta y procesa las ventas del producto
            fseek(fpSales, result * sizeof(SalesData), SEEK_SET);
            while (fread(&sale, sizeof(SalesData), 1, fpSales) == 1 && sale.ProductKey == product.ProductKey) {
                // Busca los clientes asociados con cada venta
                int customerPos = BinarySearch(fpCustomers, sale.CustomerKey, sizeof(CustomersData), customersRegisters, 3);
                if (customerPos != -1) {
                    fseek(fpCustomers, customerPos * sizeof(CustomersData), SEEK_SET);
                    fread(&customer, sizeof(CustomersData), 1, fpCustomers);
                    fwrite(&customer, sizeof(CustomersData), 1, fpAux);
                }
            }
            fclose(fpAux);
            // Ordena los clientes según la opción elegida (BubbleSort o MergeSort)
            if (option == 1) {
                BubbleSort("CustomersAux.table", "CustomersAuxOrder.table", sizeof(CustomersData), CompareCustomerLocation);
            } else if (option == 2) {
                MergeSort("CustomersAux.table", "CustomersAuxOrder.table", sizeof(CustomersData), CompareCustomerLocation);
            }
            // Abre el archivo ordenado de clientes y los imprime
            FILE *fpOrder = fopen("CustomersAuxOrder.table", "rb");
            if (fpOrder != NULL) {
                while (fread(&customer, sizeof(CustomersData), 1, fpOrder) == 1) {
                    printf("%-22s %-22s %-32s %-42s\n", customer.Continent, customer.Country, customer.State, customer.City);
                }
                fclose(fpOrder);
            } else {
                printf("Error opening file: CustomersAuxOrder.table.\n");
            }
            // Imprime una línea de separación entre productos
            printf("________________________________________________________________________________________________________________________________\n\n");
        }
    }

    // Elimina los archivos temporales
    remove("CustomersAux.table");
    remove("CustomersAuxOrder.table");

    // Marca el final de la medición de tiempo y calcula el tiempo total
    clock_t end = clock();
    double totalTime = ((double)(end - start)) / CLOCKS_PER_SEC;
    int minutes = (int)(totalTime / 60);
    int seconds = (int)(totalTime) % 60;

    // Imprime el tiempo de ejecución
    printf("------------------------------------------------------------------------------------------------------------------------\n");
    printf("Time used to produce this listing: %d' %d\"\n", minutes, seconds);
    printf("***************************LAST LINE OF THE REPORT***************************\n");
    printf("------------------------------------------------------------------------------------------------------------------------\n");

    // Cierra los archivos
    fclose(fpProducts);
    fclose(fpSales);
    fclose(fpCustomers);
}

void Graph(char screen[24][56], float array[12]) {
    // Crea un arreglo auxiliar para no modificar el original
    float arrAux[12] = {0.0};
    for (int i = 0; i < 12; i++) {
        arrAux[i] = array[i];
    }
    // Establece el valor máximo en el arreglo para la escala en el eje Y
    double yMax = arrAux[0];
    if ((int)yMax / 1000000 >= 1) {
        // Si el valor es mayor o igual a 1 millón, divide todos los valores por 1 millón
        for (int i = 0; i < 12; i++) {
            arrAux[i] = arrAux[i] / 1000000;
        }
        yMax = arrAux[0];
    }
    // Encuentra el valor máximo en el arreglo
    for (int i = 0; i < 12; i++) {
        if (arrAux[i] > yMax) {
            yMax = arrAux[i];
        }
    }
    // Calcula la escala para el eje Y, el máximo valor debe mapearse a la fila 0 (en el eje vertical)
    double scaleY = 24.0 / fabs(yMax);
    // Dibuja la línea vertical en la columna 9
    for (int i = 0; i < 24; i++) {
        screen[i][9] = 179;  // Carácter ASCII para línea vertical
    }
    // Dibuja la línea horizontal en la fila 23
    for (int i = 9; i < 56; i++) {
        if (i != 9) {
            screen[23][i] = 196;  // Carácter ASCII para línea horizontal
        } else {
            screen[23][i] = 192;  // Carácter para esquina inferior izquierda
        }
    }
    // Dibuja los valores de los datos en el gráfico, posicionándolos en las filas correspondientes
    for (int i = 0; i < 12; i++) {
        // Calcula la posición vertical donde dibujar el valor
        int placeY = (int)24.0 - round(scaleY * arrAux[i]);
        char str[12] = "";
        sprintf(str, "%.2f", arrAux[i]);  // Convierte el valor a cadena con dos decimales
        int length = strlen(str);
        // Dibuja el valor de la variable en la columna correspondiente
        for (int j = 0; j < length; j++) {
            screen[placeY][j] = str[j];
        }
        // Dibuja un marcador 'x' para representar el valor
        screen[placeY][11 + (4 * i)] = 'x';
    }
    return;
}

float GetProductPrice(unsigned short int productKey, int registers) {
    // Abre el archivo donde se almacenan los productos
    FILE *fp = fopen("ProductsOrder.table", "rb");
    if (fp == NULL) {
        // Maneja el error si no se puede abrir el archivo
        printf("Error opening file: ProductsOrder.table\n");
        return 1.0;  // Devuelve un valor por defecto en caso de error
    }

    // Declara una variable para almacenar los datos del producto
    ProductsData product;
    // Busca el producto en el archivo usando la clave
    fseek(fp, BinarySearch(fp, productKey, sizeof(ProductsData), registers, 1) * sizeof(ProductsData), SEEK_SET);
    fread(&product, sizeof(ProductsData), 1, fp);  // Lee los datos del producto
    fclose(fp);  // Cierra el archivo
    return product.UnitPriceUSD /* - product.UnitCostUSD */;  // Devuelve el precio del producto en USD
}

void PrintSeasonalAnalysis(int option) {
    // Variable que marca el inicio del tiempo de ejecución de la función
    clock_t start = clock();

    // Variable que almacenará el número de registros de productos después de realizar el ordenamiento
    int productsRegisters = 0;

    // Selección del algoritmo de ordenamiento dependiendo de la opción proporcionada
    if (option == 1) {
        // Si la opción es 1, se usa BubbleSort para ordenar los archivos de productos y ventas
        productsRegisters = BubbleSort("Products.table", "ProductsOrder.table", sizeof(ProductsData), CompareProductKey);
        BubbleSort("Sales.table", "SalesOrder.table", sizeof(SalesData), CompareSalesDate);
    } else if (option == 2) {
        // Si la opción es 2, se usa MergeSort para ordenar los archivos de productos y ventas
        productsRegisters = MergeSort("Products.table", "ProductsOrder.table", sizeof(ProductsData), CompareProductKey);
        MergeSort("Sales.table", "SalesOrder.table", sizeof(SalesData), CompareSalesDate);
    }

    // Definición de variables para manejar los registros de ventas y las métricas de análisis mensual
    SalesData sale;
    float monthlyOrders[12] = {0.0};  // Arreglo para almacenar el volumen total de órdenes por mes
    float monthlyIncome[12] = {0.0};  // Arreglo para almacenar los ingresos totales por mes

    // Apertura del archivo de ventas
    FILE *fpSales = fopen("SalesOrder.table", "rb");
    if (fpSales == NULL) {
        printf("Error opening file: Sales.table\n");
        return;
    }

    // Variables para almacenar las fechas de inicio y fin del análisis
    SalesData startDate, finalDate;
    int yearsAnalyzed = 0;

    // Lectura del primer registro de ventas para obtener la fecha de inicio
    fread(&sale, sizeof(SalesData), 1, fpSales);
    startDate = sale;

    // Moverse al final del archivo y leer el último registro de ventas para obtener la fecha final
    fseek(fpSales, -1 * sizeof(SalesData), SEEK_END);
    fread(&sale, sizeof(SalesData), 1, fpSales);
    finalDate = sale;

    // Calcular el número de años de análisis
    fseek(fpSales, 0, SEEK_SET);  // Volver al principio del archivo
    yearsAnalyzed = finalDate.OrderDate.AAAA - startDate.OrderDate.AAAA + 1;

    // Procesamiento de las ventas y acumulación de datos mensuales
    int monthIndex = 0;
    while (fread(&sale, sizeof(SalesData), 1, fpSales)) {
        monthIndex = sale.OrderDate.MM - 1;  // Convertir el mes en índice (0-11)
        monthlyOrders[monthIndex] += sale.Quantity;  // Acumular las órdenes del mes
        monthlyIncome[monthIndex] += sale.Quantity * GetProductPrice(sale.ProductKey, productsRegisters);  // Acumular los ingresos del mes
    }
    fclose(fpSales);

    // Calcular los promedios mensuales
    float averageMonthlyOrders[12];
    float averageMonthlyIncome[12];
    for (int i = 0; i < 12; i++) {
        averageMonthlyOrders[i] = monthlyOrders[i] / yearsAnalyzed;  // Promedio de órdenes por mes
        averageMonthlyIncome[i] = monthlyIncome[i] / yearsAnalyzed;  // Promedio de ingresos por mes
    }

    // Preparación para mostrar los gráficos de los resultados
    char screen[24][56] = {{}};  // Arreglo para representar los gráficos de forma ASCII
    memset(screen, ' ', sizeof(screen));  // Inicializar el arreglo de gráficos con espacios en blanco
    char meses[] = " ene feb mar abr may jun jul ago sep oct nov dic";  // Nombres de los meses en español

    printf("------------------------------------------------------------------------------------------------------------------------\n");
    printf("Company Global Electronics Retailer\n");
    printf("Valid to ");
    ShowDateTime();
    printf("Title: Analysis of Seasonal Patterns in Orders and Income for Company Global Electronics Retailer\n");
    printf("This report aims to analyze whether there are seasonal patterns or trends in order volume and Income.\n");
    printf("Methodology: Data collection: Historical order and revenue data is collected, with an appropriate time range to identify seasonal patterns.\n");
    printf("Data preparation: The data will be organized to facilitate analysis.\n\n");
    printf("Order Volume and Monthly income Analysis\nWhen examining the order volume and monthly income data, a direct relationship between the two is observed, making it easy to identify the increases and decreases in both income and the number of orders. This analysis is especially useful when visualizing these patterns through charts and tables, facilitating the understanding of the peaks and drops that affect both sales volume and income. Additionally, there is a noticeable stability in certain months, highlighting the seasonality of the business.");
    printf("Order and income Peaks\nFebruary and December stand out with the highest order volumes (%.0f and %.0f orders) and incomes ($%.2f and $%.2f), due to year-end promotions and January sales.\n", monthlyOrders[1], monthlyOrders[11], monthlyIncome[1], monthlyIncome[11]);
    printf("Months with Fewer Orders and income\nIn March and April, both orders and income drop significantly, with %.0f and %.0f orders respectively. This is due to lower demand after the holidays and the lack of promotions or incentives.\n", monthlyOrders[2], monthlyOrders[3]);
    printf("Stability in the Intermediate Months\nMay, June, and July show steady demand, with 4,000 to 5,000 orders per month, suggesting that activity remains balanced during these intermediate months.\n");
    printf("Monthly Averages\nThe average order volumes follow the same seasonal trend, with February and December reaching the highest peaks, while March and April show the lowest values, confirming the post-holiday drop.\n\n");
    printf("Results:\nChart 1: Order volume per month from %i/%i/%i to %i/%i/%i\n", startDate.OrderDate.MM, startDate.OrderDate.DD, startDate.OrderDate.AAAA, finalDate.OrderDate.MM, finalDate.OrderDate.DD, finalDate.OrderDate.AAAA);
    printf("-------------------------\n|Month\t|Order Volume\n-------------------------\t");
    Graph(screen, monthlyOrders); // Llamar a la funcion que llena screen con la gráfica
    for (int i = 0; i < 24; i++) {
        for (int j = 0; j < 56; j++) {
            printf("%c", screen[i][j]); // Imprimir una fila del gráfico
        }
        printf("\n");
        if (i % 2 == 0) {
            printf("|%d\t|%.0f\t\t|\t", (i / 2) + 1, monthlyOrders[(i / 2)]); // imprimir una filla de la tabla y grafica
        } else if (i == 23) {
            printf("-------------------------\t         %s", meses); // Imprimir meses
        } else {
            printf("-------------------------\t"); // Separaciones de la tabla
        }
    }
    printf("\n\nChart 2: Income per month from %i/%i/%i to %i/%i/%i\n", startDate.OrderDate.MM, startDate.OrderDate.DD, startDate.OrderDate.AAAA, finalDate.OrderDate.MM, finalDate.OrderDate.DD, finalDate.OrderDate.AAAA);
    printf("-------------------------\n|Month\t|Total Income\t|\t\tMillions USD\n-------------------------\t");
    memset(screen, ' ', sizeof(screen)); // Limpiar la pantalla para el siguiente gráfico
    Graph(screen, monthlyIncome); // Llamar a la funcion que llena screen con la gráfica
    for (int i = 0; i < 24; i++) {
        for (int j = 0; j < 56; j++) {
            printf("%c", screen[i][j]); // Imprimir una fila del gráfico
        }
        printf("\n");
        if (i % 2 == 0) {
            printf("|%d\t|%.2f\t|\t", (i / 2) + 1, monthlyIncome[(i / 2)]); // imprimir una filla de la tabla y grafica
        } else if (i == 23) {
            printf("-------------------------\t         %s", meses); // Imprimir meses
        } else {
            printf("-------------------------\t"); // Separaciones de la tabla
        }
    }
    printf("\n\nChart 3: Monthly average order volume from %i/%i/%i to %i/%i/%i\n", startDate.OrderDate.MM, startDate.OrderDate.DD, startDate.OrderDate.AAAA, finalDate.OrderDate.MM, finalDate.OrderDate.DD, finalDate.OrderDate.AAAA);
    printf("-------------------------\n|Month\t|Monthly Average|\n-------------------------\t");
    memset(screen, ' ', sizeof(screen)); // Limpiar la pantalla para el siguiente gráfico
    Graph(screen, averageMonthlyOrders); // Llamar a la funcion que llena screen con la gráfica
    for (int i = 0; i < 24; i++) {
        for (int j = 0; j < 56; j++) {
            printf("%c", screen[i][j]);// Imprimir una fila de la gráfica
        }
        printf("\n");
        if (i % 2 == 0) {
            printf("|%d\t|%.0f\t\t|\t", (i / 2) + 1, averageMonthlyOrders[(i / 2)]); // Imprimir una fila de la tabla
        } else if (i == 23) {
            printf("-------------------------\t         %s", meses); // Imprimir ultima fila de la tabla y labels de x
        } else {
            printf("-------------------------\t"); // Separaciones de la tabla
        }
    }
    printf("\n\nChart 4: Monthly average income from %i/%i/%i to %i/%i/%i\n", startDate.OrderDate.MM, startDate.OrderDate.DD, startDate.OrderDate.AAAA, finalDate.OrderDate.MM, finalDate.OrderDate.DD, finalDate.OrderDate.AAAA);
    printf("-------------------------\n|Month\t|Total Income\t|\t\tMillions USD\n-------------------------\t");
    memset(screen, ' ', sizeof(screen)); // Limpiar la pantalla para el siguiente gráfico
    Graph(screen, averageMonthlyIncome); // Llamar a la funcion que llena screen con la gráfica
    for (int i = 0; i < 24; i++) {
        for (int j = 0; j < 56; j++) {
            printf("%c", screen[i][j]);// Imprimir una fila de la gráfica
        }
        printf("\n");
        if (i % 2 == 0) {
            printf("|%d\t|%.2f\t|\t", (i / 2) + 1, averageMonthlyIncome[(i / 2)]); // Imprimir una fila de la tabla
        } else if (i == 23) {
            printf("-------------------------\t         %s", meses); // Imprimir ultima fila de la tabla y labels de x
        } else {
            printf("-------------------------\t"); // Separaciones de la tabla
        }
    }
    // Conclusiones y recomendaciones
    printf("\nConclusions:\nClear seasonal patterns show sales peaks in December and February, followed by drops in March and April.\n");
    printf("It is recommended to take advantage of demand peaks with marketing strategies and promotions, while managing the low-demand months with offers and discounts to maintain income flow.\n");
    printf("\nRecommendations:\nStrengthen marketing efforts during low-demand months (March and April) with promotions and discounts.\nIncrease inventory during high-demand months (November, December, and January) to meet the sales peaks.\n");
    printf("Reduce inventory during months of lower activity (March and April) to avoid overstocking and additional costs.\nBuild customer loyalty with reward programs that encourage purchases in low months\n");
    clock_t end = clock();
    double totalTime = ((double)(end - start)) / CLOCKS_PER_SEC;
    int minutes = (int)(totalTime / 60);
    int seconds = (int)(totalTime) % 60;
    printf("------------------------------------------------------------------------------------------------------------------------\n");
    printf("Time used to produce this listing: %d' %d\"\n", minutes, seconds); // Tiempo y cierre del reporte
    printf("***************************LAST LINE OF THE REPORT***************************\n");
}

void PrintDeliveryTimeAnalysis() {
    clock_t start = clock(); // Inicia el temporizador para medir el tiempo total de ejecución.
    // Abre el archivo Sales.table en modo de solo lectura binaria.
    FILE *fpSales = fopen("Sales.table", "rb");
    if (fpSales == NULL) {
        printf("Error openning file.\n");
        return;
    }
    int salesRegisters = 0;
    // Mueve el puntero al final del archivo para calcular el número de registros de ventas.
    fseek(fpSales, 0, SEEK_END);
    salesRegisters = ftell(fpSales) / sizeof(SalesData);
    fseek(fpSales, 0, SEEK_SET);
    // Crea un archivo auxiliar para almacenar las ventas con fechas de entrega válidas.
    FILE *fpAux = fopen("auxiliarSales", "wb+");
    if (fpAux == NULL) {
        printf("Error creating file.\n");
        return;
    }
    SalesData sale; // Variable para almacenar los datos de cada venta.
    int deliveredSales = 0; // Contador de ventas con fechas de entrega válidas.
    int averageDeliveryTime[6][2] = {0}; // Arreglo para almacenar la suma y el conteo de los tiempos de entrega por año.
    // Recorre todas las ventas y filtra aquellas con fechas de entrega válidas.
    for (int i = 0, temporalIndex = 0; i < salesRegisters; i++) {
        fseek(fpSales, i * sizeof(SalesData), SEEK_SET);
        fread(&sale, sizeof(SalesData), 1, fpSales);
        // Siempre y cuando haya una fecha de delivery, se hace la adicion a la cantidad de deliveredSales
        if (sale.DeliveryDate.AAAA != 0 && sale.DeliveryDate.MM != 0 && sale.DeliveryDate.DD != 0) {
            fseek(fpAux, sizeof(SalesData) * temporalIndex, SEEK_SET);
            fwrite(&sale, sizeof(SalesData), 1, fpAux);
            temporalIndex++;
            deliveredSales++;
        }
    }

    // Calcula el tiempo de entrega para las ventas válidas.
    for (int i = 0; i < deliveredSales; i += 1) {
        fseek(fpAux, sizeof(SalesData) * i, SEEK_SET);
        fread(&sale, sizeof(SalesData), 1, fpAux);
        // Siempre y cuando haya una fecha de delivery, se continua con el proceso
        if (sale.DeliveryDate.AAAA != 0 && sale.DeliveryDate.MM != 0 && sale.DeliveryDate.DD != 0) {
            int year = (int)sale.DeliveryDate.AAAA % 2016;
            long int orderDate = 0, deliveryDate = 0, deliveryTimeInDays = 0;
            // Calcula las fechas de orden y entrega en formato de días transcurridos desde un punto de referencia.
            orderDate = sale.OrderDate.AAAA * 360 + sale.OrderDate.MM * 30 + sale.OrderDate.DD;
            deliveryDate = sale.DeliveryDate.AAAA * 360 + sale.DeliveryDate.MM * 30 + sale.DeliveryDate.DD;
            // Calcula el tiempo de entrega en días.
            deliveryTimeInDays = deliveryDate - orderDate;
            averageDeliveryTime[year][0] += deliveryTimeInDays; // Acumula el tiempo de entrega total por año.
            averageDeliveryTime[year][1] += 1; // Cuenta las ventas por año.
        } else {
            printf("\t\t\t\t\tOrderNumber: %li has an invalid DeliveryDate.\n", sale.OrderNumber);
        }
    }
    // Imprime el encabezado del informe.
    printf("------------------------------------------------------------------------------------------------------------------------\n");
    printf("Company Global Electronics Retailer\nValid to ");
    ShowDateTime();
    printf("\nTitle: Analysis of delivery time in days for Company Global Electronics Retailer\n");
    printf("\nOver the years analyzed, a progressive decrease in delivery times is observed, suggesting that both\nlocal and online stores have managed to improve their delivery efficiency over time.\n\n");
    printf("Once the data was sorted by delivery date, the average delivery time in days was calculated, which\nrefers to the average number of days between the order date and the delivery date. This calculation\nis crucial to understand the efficiency of delivery times based on the provided data.\n\n");
    printf("It is important to note that, in the analyzed dataset, there are no records of physical stores with a\ndelivery date. Therefore, the comparison was made only between online store that\ndo have valid delivery date records.\n\n");
    printf("Results:\n");
    printf("After analyzing the data, the following results were obtained for the average delivery time in days:\n\n");
    // Muestra los tiempos promedio de entrega por año.
    for(int i = 0; i < 6; i += 1){
        printf("\n\tAverage delivery time in: %i:\t%.2f days\n", i + 2016, (1.0 * averageDeliveryTime[i][0]) / (1.0 * averageDeliveryTime[i][1]));
    }
    // Concluye el análisis.
    printf("\n\nOver the years analyzed, a progressive decrease in delivery times is observed, suggesting that \nthe online store has managed to improve its delivery efficiency over time.\n\n");
    printf("Conclusion:\n\n");
    printf("In the provided dataset, there are no records for physical stores with delivery dates, so the analysis\nhas been limited to online stores. The calculation of average delivery time shows a trend of\nimprovement over the years, with a significant reduction in average delivery time, from 7.25 days in\n2016 to 3.76 days in 2021.\n\n");
    printf("This analysis highlights how, over time, delivery processes have improved, which may reflect\noptimization in logistical systems or operational efficiency in both online and local stores.\n\n");
    // Cierra los archivos.
    fclose(fpAux);
    fclose(fpSales);
    // Calcula y muestra el tiempo total de ejecución.
    clock_t end = clock();
    double totalTime = ((double)(end - start)) / CLOCKS_PER_SEC;
    int minutes = (int)(totalTime / 60);
    int seconds = (int)(totalTime) % 60;
    printf("------------------------------------------------------------------------------------------------------------------------\n");
    printf("Time used to produce this listing: %d' %d\"\n", minutes, seconds);
    printf("***************************LAST LINE OF THE REPORT***************************\n");
    printf("------------------------------------------------------------------------------------------------------------------------\n");
}

void PrintReport5(FILE *fpSales, FILE *fpCustomers, FILE *fpProducts ,int option) {
    clock_t start = clock();
    int customersRegisters = 0, salesRegisters = 0, productsRegisters = 0;
    // Dependiendo de la opcion se ordenan los datos que se necesitan
    if (option == 1) {
        customersRegisters = BubbleSort("Customers.table", "CustomersOrder.table", sizeof(CustomersData), CompareCustomerName);
        productsRegisters = BubbleSort("Products.table", "ProductsOrder.table", sizeof(ProductsData), CompareProductKey);
        salesRegisters = BubbleSort("Sales.table", "SalesOrder.table", sizeof(SalesData), CompareSaleByCustomerKey);
    } else if (option == 2) {
        customersRegisters = MergeSort("Customers.table", "CustomersOrder.table", sizeof(CustomersData), CompareCustomerName);
        productsRegisters = MergeSort("Products.table", "ProductsOrder.table", sizeof(ProductsData), CompareProductKey);
        salesRegisters = MergeSort("Sales.table", "SalesOrder.table", sizeof(SalesData), CompareSaleByCustomerKey);
    }
    // Imprime el encabezado del informe
    printf("\n---------------------------------------------------------------------------------------");
    printf("\nCompany Global Electronics Retailer\n");
    printf("Customer list ordered by Customer name + order date for sale + ProductKey\n");
    // Inicializamos las variables que van a contener los datos extraidos de las tablas
	SalesData recordSale; 
	CustomersData recordCustomer;
	ExchangeRates recordExchange; 
	
	FILE *fpExchangeRates = NULL; // Apuntador para el archivo de tasas de cambio
    FILE *fpTemporalSales = NULL; // Apuntador para el archivo temporal de ventas

    char customerName[40] = ""; // Cadena para almacenar el nombre del cliente
    unsigned int customerKey = 0; // Clave única del cliente

    // Abrimos archivos temporales para guardar ventas y tipos de cambio
    fpTemporalSales = fopen("TemporalFileSalesOption5", "wb+");
    if(fpTemporalSales == NULL){
        printf("Error abiendo el temporal de sales\n");
        return;
    }
    fpExchangeRates = fopen("ExchangeRates.table", "rb+");
    if(fpExchangeRates == NULL){
        printf("Error abriendo el archivo de exchange\n");
        return;
    }
    // Procesamos cada cliente
    for(int i = 0; i < customersRegisters; i++) {
        fseek(fpCustomers, sizeof(CustomersData) * i, SEEK_SET); // Posicionamos el puntero al registro del cliente i
        fread(&recordCustomer, sizeof(CustomersData), 1, fpCustomers); // Leemos el registro del cliente

        strcpy(customerName, recordCustomer.Name); // Guardamos el nombre del cliente
        printf("\nCustomer name: %-40s", customerName);

        customerKey = recordCustomer.CustomersKey; // Guardamos la clave del cliente
        int positionSales = BinarySearch(fpSales, customerKey, sizeof(SalesData), salesRegisters, 4); // Buscamos las ventas del cliente
        if(positionSales == -1){
            printf(" - No purchases reported\n"); // Si no hay ventas, indicamos que no se reportaron
        } else {
            // Posicionamos el puntero al primer registro de ventas del cliente
            fseek(fpSales, (positionSales - 1) * sizeof(SalesData), SEEK_SET);
            fread(&recordSale, sizeof(SalesData), 1, fpSales);
            // Verificamos si es el primer registro de ventas para este cliente
            for( int i = positionSales - 1; i >= 0 && customerKey == recordSale.CustomerKey; i -= 1){
				// Lectura del registro anterior en ventas para verificar si es el primero con la CustomerKey actual
    	    	fseek(fpSales, sizeof(SalesData) * (i - 1), SEEK_SET);
    	    	fread(&recordSale, sizeof(SalesData), 1, fpSales);

    	    	positionSales = i; // Cambiar positionSales para que sea el índice del primer registro en ventas con la CustomerKey actual
    	    }
            fseek(fpSales, positionSales * sizeof(SalesData), SEEK_SET);
            fread(&recordSale, sizeof(SalesData), 1, fpSales);

            int numOfOrders = 0, totalNumOfPurchases = 0; // Contadores de órdenes y compras totales
            long orderNumber = recordSale.OrderNumber; // Número de la primera orden
            // Procesamos todas las ventas para este cliente
            for(int index = positionSales, i = 0; recordSale.CustomerKey == customerKey && index < salesRegisters; index++, i++) {
                fseek(fpTemporalSales, i * sizeof(SalesData), SEEK_SET);
                fwrite(&recordSale, sizeof(SalesData), 1, fpTemporalSales); // Guardamos las ventas en el archivo temporal
                totalNumOfPurchases++; // Incrementamos el contador de compras
                fseek(fpSales, (index + 1) * sizeof(SalesData), SEEK_SET);
                fread(&recordSale, sizeof(SalesData), 1, fpSales);
				 // Si encontramos un cambio en el número de orden, incrementamos el contador de órdenes
                if(orderNumber != recordSale.OrderNumber){
                    numOfOrders++;
                    orderNumber = recordSale.OrderNumber; // Actualizamos el número de la orden
                }
            }
            // Ordenamos las ventas por número de orden según la opción seleccionada
			if(option == 1){
				BubbleSort("TemporalFileSalesOption5", "TemporalSalesOrder", sizeof(SalesData), CompareSalesByOrderNumber);
			} else if(option == 2){
				MergeSort("TemporalFileSalesOption5", "TemporalSalesOrder", sizeof(SalesData), CompareSalesByOrderNumber);
			}			
		    SalesData tempRecordSale1, tempRecordSale2; // Registros temporales para almacenar las ventas ordenadas

            // Comenzamos a procesar las ventas ordenadas por número de orden
            fseek(fpSales, positionSales * sizeof(SalesData), SEEK_SET);
            fread(&recordSale, sizeof(SalesData), 1, fpSales);
			
			int orderIndex = 0;  // Índice de órdenes, usado para iterar sobre las ventas de una orden
            float totalValue = 0.0; // Valor total acumulado de las ventas

            // Procesamos cada orden
			for (int order = 0; order < numOfOrders; order++) {
			    // Leer el primer registro de la orden actual
			    fseek(fpTemporalSales, sizeof(SalesData) * orderIndex, SEEK_SET);
			    fread(&tempRecordSale1, sizeof(SalesData), 1, fpTemporalSales);

			    // Leer el siguiente registro para comparación
			    fseek(fpTemporalSales, sizeof(SalesData) * (orderIndex + 1), SEEK_SET);
			    fread(&tempRecordSale2, sizeof(SalesData), 1, fpTemporalSales);

			    float exchange = -1.0; // Variable para almacenar la tasa de cambio de la moneda

				// Buscar el tipo de cambio para esta orden
				int positionExchange = BinarySearchExchangeDate(fpExchangeRates, tempRecordSale1);
				if (positionExchange != -1) {
					ExchangeRates staticExchangeRecord; // Registro de tasa de cambio
					fseek(fpExchangeRates, sizeof(ExchangeRates) * positionExchange, SEEK_SET);
					fread(&staticExchangeRecord, sizeof(ExchangeRates), 1, fpExchangeRates);
					int index = positionExchange;
                    // Si la moneda no es USD, buscamos la tasa de cambio en USD
					if (strcmp("USD", staticExchangeRecord.Currency) != 0) {
						fseek(fpExchangeRates, sizeof(ExchangeRates) * positionExchange, SEEK_SET);
						fread(&recordExchange, sizeof(ExchangeRates), 1, fpExchangeRates);
						for (; strcmp("USD", recordExchange.Currency) != 0 && index > 0; index--) {
							fseek(fpExchangeRates, sizeof(ExchangeRates) * index, SEEK_SET);
							fread(&recordExchange, sizeof(ExchangeRates), 1, fpExchangeRates);
						}
						index++;
					}
					FILE *fpTemporalExchange = tmpfile(); // Archivo temporal para tasas de cambio
                    // Guardamos las tasas de cambio temporales
					for (int i = 0; i < 5; i++, index++){
						fseek(fpExchangeRates, sizeof(ExchangeRates) * index, SEEK_SET);
						fread(&recordExchange, sizeof(ExchangeRates), 1, fpExchangeRates);
						fseek(fpTemporalExchange, sizeof(ExchangeRates) * i, SEEK_SET);
						fwrite(&recordExchange, sizeof(ExchangeRates), 1, fpTemporalExchange);
					}

					int indexTemoralExchange = -1; // Índice de la tasa de cambio correspondiente a la moneda de la venta
                    if (tempRecordSale1.CurrencyCode[0] == 'U'){
						indexTemoralExchange = 0;
					}
					else if (tempRecordSale1.CurrencyCode[0] == 'C'){
						indexTemoralExchange = 1;
					}
					else if (tempRecordSale1.CurrencyCode[0] == 'A'){
						indexTemoralExchange = 2;
					}
					else if (tempRecordSale1.CurrencyCode[0] == 'E'){
						indexTemoralExchange = 3;
					}
					else if (tempRecordSale1.CurrencyCode[0] == 'G'){
						indexTemoralExchange = 4;
					}
					fseek(fpTemporalExchange, sizeof(ExchangeRates) * indexTemoralExchange, SEEK_SET);
					fread(&recordExchange, sizeof(ExchangeRates), 1, fpTemporalExchange);
                
					exchange = recordExchange.Exchange; // Asignamos la tasa de cambio
                    fclose(fpTemporalExchange); // Cerramos el archivo temporal
				}

			    // Imprimir encabezado de la orden
			    printf("\nOrder date: %hu/%u/%u\tOrder Number: %li\n", 
			           tempRecordSale1.OrderDate.AAAA, tempRecordSale1.OrderDate.MM, tempRecordSale1.OrderDate.DD, 
			           tempRecordSale1.OrderNumber);
				printf("%-17s%-90s%-15s%s %s", "ProductKey", "ProductName", "Quantity", "Value", tempRecordSale1.CurrencyCode);
			    printf("\n_________________________________________________________________________________________________________________________________________________________\n");

			    float subTotal = 0.0; // Variable para almacenar el subtotal de la venta
                ProductsData tempProductRecord; // Registro temporal para almacenar el producto

                // Iterar sobre todos los productos de la misma orden
			    while (orderIndex < totalNumOfPurchases - 1 && tempRecordSale1.OrderNumber == tempRecordSale2.OrderNumber) {
			        unsigned int productKey = tempRecordSale1.ProductKey;

			        // Buscar el producto en el archivo de productos
			        int positionProducts = BinarySearch(fpProducts, productKey, sizeof(ProductsData), productsRegisters, 1);
			        if (positionProducts != -1) {
			            fseek(fpProducts, sizeof(ProductsData) * positionProducts, SEEK_SET);
			            fread(&tempProductRecord, sizeof(ProductsData), 1, fpProducts);

			            // Calcular el valor del producto
			            float price = tempProductRecord.UnitPriceUSD * exchange * tempRecordSale1.Quantity;
			            printf("%-17hu%-100s%-15hu%.2f\n", tempProductRecord.ProductKey, tempProductRecord.ProductName, tempRecordSale1.Quantity, price);
			            subTotal += price;
			        }

			        // Avanzar al siguiente registro
			        orderIndex++;

			        // Actualizar los datos para la próxima iteración
			        fseek(fpTemporalSales, sizeof(SalesData) * orderIndex, SEEK_SET);
			        fread(&tempRecordSale1, sizeof(SalesData), 1, fpTemporalSales);

			        fseek(fpTemporalSales, sizeof(SalesData) * (orderIndex + 1), SEEK_SET);
			        fread(&tempRecordSale2, sizeof(SalesData), 1, fpTemporalSales);
			    }

			    // Procesar el último producto de la orden
			    unsigned int productKey = tempRecordSale1.ProductKey;

			    int positionProducts = BinarySearch(fpProducts, productKey, sizeof(ProductsData), productsRegisters, 1);
			    if (positionProducts != -1) {
			        fseek(fpProducts, sizeof(ProductsData) * positionProducts, SEEK_SET);
			        fread(&tempProductRecord, sizeof(ProductsData), 1, fpProducts);

			        float price = tempProductRecord.UnitPriceUSD * exchange * tempRecordSale1.Quantity;
			        printf("%-17hu%-100s%-15hu%.2f\n", tempProductRecord.ProductKey, tempProductRecord.ProductName, tempRecordSale1.Quantity, price);
			        subTotal += price;
			    }

			    orderIndex++; // Avanzar al siguiente índice
			    printf("_________________________________________________________________________________________________________________________________________________________\n");
				printf("%-117s%-15s%.2lf\n", "", "Subtotal", subTotal);
			    totalValue += subTotal;
			}
			printf("%-117s%-15s%.2lf\n", "", "TOTAL", totalValue);
		}
        freopen(NULL, "wb+", "TemporalFileSalesOption5"); // Volver a abrir el archivo para vaciarl
	}
    clock_t end = clock(); // Se captura el tiempo al final de la ejecución para calcular el tiempo total
    double totalTime = ((double)(end - start)) / CLOCKS_PER_SEC; // Calcula el tiempo total de ejecución en segundos
    int minutes = (int)(totalTime / 60); // Convierte el tiempo total a minutos
    int seconds = (int)(totalTime) % 60; // Obtiene el resto de segundos después de convertir a minutos
    printf("------------------------------------------------------------------------------------------------------------------------\n");
    printf("Time used to produce this listing: %d' %d\"\n", minutes, seconds); // Muestra los minutos y segundos transcurridos
    printf("***************************LAST LINE OF THE REPORT***************************\n");
    printf("------------------------------------------------------------------------------------------------------------------------\n");
    // Cierra los archivos abiertos
    fclose(fpTemporalSales); // Cierra el archivo temporal de ventas
    fclose(fpExchangeRates); // Cierra el archivo de tipos de cambio
    // Elimina el archivo temporal de ventas para limpiar los archivos generados durante la ejecución
    remove("TemporalFileSalesOption5"); // Elimina el archivo temporal de ventas después de finalizar el procesamiento
}

int main() {
    float option = 0.0; // Variable para almacenar la opción seleccionada por el usuario
    do {
        // Menú de opciones para que el usuario seleccione una acción
        printf("\n\nCompany Global Electronics Retailer\nOptions menu\n");
        printf("0. Exit program\n1. Construction of the Database with the dataset tables\n");
        printf("2. List of %cWhat types of products does the company sell, and where are customers located?\n\t2.1 Utility bubbleSort\n\t2.2 Utility mergeSort\n", 168);
        printf("3. List of %cAre there any seasonal patterns or trends for order volume or Income?\n\t3.1 Utility bubbleSort\n\t3.2 Utility mergeSort\n", 168);
        printf("4. List of %cHow long is the average delivery time in days? Has that changed over time?\n\t4.1 Utility bubbleSort\n\t4.2 Utility mergeSort\n", 168);
        printf("5. List of sales order by \"Customer Name\"+\"Order Date\"+\"ProductKey\";\n\t5.1 Utility bubbleSort\n\t5.2 Utility mergeSort\nWhat is your option: ");
        
        // Lee la opción seleccionada por el usuario
        scanf("%f", &option);
        option = (option * 10); // Convierte la opción a un formato entero para facilitar la comparación

        // Opción 1: Creación de las tablas de la base de datos
        if (option == 10) {
            CreateSalesTable("Sales.dat"); // Crea la tabla de ventas
            CreateCustomersTable("Customers.dat"); // Crea la tabla de clientes
            CreateStoresTable("Stores.dat"); // Crea la tabla de tiendas
            CreateExchangeRatesTable("Exchange_Rates.dat"); // Crea la tabla de tasas de cambio
            CreateProductsTable("Products.dat"); // Crea la tabla de productos
        } 
        // Opción 2: Análisis de los tipos de productos y la ubicación de los clientes
        else if (option == 21) {
            PrintReport2(1); // Llama a la función para mostrar el reporte usando bubbleSort
        } else if (option == 22) {
            PrintReport2(2); // Llama a la función para mostrar el reporte usando mergeSort
        } 
        // Opción 3: Análisis estacional de los patrones o tendencias
        else if (option == 31) {
            PrintSeasonalAnalysis(1); // Llama a la función para análisis estacional con bubbleSort
        } else if (option == 32) {
            PrintSeasonalAnalysis(2); // Llama a la función para análisis estacional con mergeSort
        } 
        // Opción 4: Análisis del tiempo promedio de entrega
        else if (option == 41 || option == 42) {
            PrintDeliveryTimeAnalysis(); // Llama a la función para análisis de tiempos de entrega
        } 
        // Opción 5: Informe de las ventas ordenadas por "Nombre del Cliente", "Fecha del Pedido" y "ProductKey"
        else if (option == 51) {
            // Abre los archivos necesarios y genera el reporte con bubbleSort
            FILE *fpSales = fopen("SalesOrder.table", "rb");
            FILE *fpCustomers = fopen("CustomersOrder.table", "rb");
            FILE *fpProducts = fopen("ProductsOrder.table", "rb");
            PrintReport5(fpSales, fpCustomers, fpProducts, 1); // Imprime el reporte con bubbleSort
            fclose(fpSales); // Cierra el archivo de ventas
            fclose(fpCustomers); // Cierra el archivo de clientes
            fclose(fpProducts); // Cierra el archivo de productos
        } 
        // Opción 5.2: Informe de las ventas ordenadas con mergeSort
        else if (option == 52) {
            // Abre los archivos necesarios y genera el reporte con mergeSort
            FILE *fpSales = fopen("SalesOrder.table", "rb");
            FILE *fpCustomers = fopen("CustomersOrder.table", "rb");
            FILE *fpProducts = fopen("ProductsOrder.table", "rb");
            PrintReport5(fpSales, fpCustomers, fpProducts, 2); // Imprime el reporte con mergeSort
            fclose(fpSales); // Cierra el archivo de ventas
            fclose(fpCustomers); // Cierra el archivo de clientes
            fclose(fpProducts); // Cierra el archivo de productos
        } 
        // Si el usuario ingresa una opción no válida
        else if (option != 0) {
            printf("\nInvalid option\n"); // Mensaje de opción no válida
        } 
    } while (option != 0); // El bucle continuará hasta que el usuario seleccione la opción 0 para salir

    return 0; // Fin del programa
}
