#include <iostream>
#include <iomanip>
#include <limits>

void inicializar(float **&, float *&, float *&, int **&, int &, int &);
void verPlan(float **, int, int);
void agregarSemana(float **&, int &, int);
void modificarProduccion(float **, int, int);
void calcularCOGS(float **, float *, float *, int **, int, int);
void reporteConsumo(float **, int **, int, int);
void expandirProductos(float **&, float *&, int **&, int &, int);
void cargarDatosIniciales(float **, int, int);
void liberar(float **&, float *&, float *&, int **&, int, int);
bool validarEntrada();

int main()
{
    float **planProduccion = nullptr;
    float *costosUnitarios = nullptr;
    float *materiaPrima = nullptr;
    int **requerimientos = nullptr;
    
    int productos = 5;
    int semanas = 4;
    
    std::cout << "Sistema de Planificacion y Costos \n\n";
    
    try {
        inicializar(planProduccion, costosUnitarios, materiaPrima, 
                   requerimientos, productos, semanas);
    } catch (std::bad_alloc& e) {
        std::cerr << "Error critico: No hay memoria suficiente para inicializar el sistema.\n";
        return 1;
    }
    
    int op;
    do {
        std::cout << "\n--- Menu Principal ---\n";
        std::cout << "1. Ver Plan de Produccion\n";
        std::cout << "2. Agregar Nueva Semana (Redimensionar Matriz)\n";
        std::cout << "3. Modificar Produccion\n";
        std::cout << "4. Calcular COGS y Final Inventory\n";
        std::cout << "5. Reporte de Consumo de MP\n";
        std::cout << "6. Cargar Datos de Produccion\n";
        std::cout << "7. Expandir Productos (Agregar Nuevo Producto)\n";
        std::cout << "8. Salir\n";
        std::cout << "Opcion seleccionada: ";
        std::cin >> op;
        
        if (!validarEntrada()) {
            std::cout << "Entrada invalida. Intente de nuevo.\n";
            continue;
        }
        
        switch(op) {
            case 1:
                verPlan(planProduccion, productos, semanas);
                break;
            case 2:
                try {
                    agregarSemana(planProduccion, semanas, productos);
                } catch (std::bad_alloc& e) {
                    std::cerr << "Error: No hay memoria suficiente para agregar semana.\n";
                }
                break;
            case 3:
                modificarProduccion(planProduccion, productos, semanas);
                break;
            case 4:
                calcularCOGS(planProduccion, costosUnitarios, materiaPrima, 
                           requerimientos, productos, semanas);
                break;
            case 5:
                reporteConsumo(planProduccion, requerimientos, productos, semanas);
                break;
            case 6:
                cargarDatosIniciales(planProduccion, productos, semanas);
                break;
            case 7:
                try {
                    expandirProductos(planProduccion, costosUnitarios, 
                                    requerimientos, productos, semanas);
                } catch (std::bad_alloc& e) {
                    std::cerr << "Error: No hay memoria suficiente para expandir productos.\n";
                }
                break;
            case 8:
                liberar(planProduccion, costosUnitarios, materiaPrima, 
                       requerimientos, productos, semanas);
                std::cout << "Liberando memoria de Matriz y Vectores Dinamicos...\n";
                std::cout << "Sistema cerrado.\n";
                break;
            default:
                std::cout << "Opcion invalida.\n";
        }
    } while(op != 8);
    
    return 0;
}

bool validarEntrada()
{
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return false;
    }
    return true;
}

void inicializar(float **&plan, float *&costos, float *&mp, 
                int **&reqs, int &productos, int &semanas)
{
    std::cout << "Inicializando sistema...\n";
    
    plan = new float*[productos];
    for (int i = 0; i < productos; i++) {
        plan[i] = new float[semanas];
        for (int j = 0; j < semanas; j++) {
            plan[i][j] = 100.0 * (i + 1) * (j + 1);
        }
    }
    
    costos = new float[productos];
    costos[0] = 15.50;
    costos[1] = 22.00;
    costos[2] = 18.75;
    costos[3] = 30.00;
    costos[4] = 25.50;
    
    mp = new float[4];
    mp[0] = 1000.0;
    mp[1] = 800.0;
    mp[2] = 1200.0;
    mp[3] = 600.0;
    
    reqs = new int*[productos];
    for (int i = 0; i < productos; i++) {
        reqs[i] = new int[4];
    }
    
    int reqIniciales[5][4] = {
        {10, 5, 8, 3},
        {12, 6, 10, 4},
        {8, 4, 6, 2},
        {15, 7, 12, 5},
        {9, 5, 7, 3}
    };
    
    for (int i = 0; i < productos; i++) {
        for (int j = 0; j < 4; j++) {
            reqs[i][j] = reqIniciales[i][j];
        }
    }
    
    std::cout << "Inicializacion exitosa. Matriz de Planificacion: " 
              << productos << " Productos x " << semanas << " Semanas.\n";
}

void verPlan(float **plan, int productos, int semanas)
{
    std::cout << "\n--- Plan de Produccion ---\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Prod\t";
    for (int j = 0; j < semanas; j++) {
        std::cout << "Sem" << j+1 << "\t";
    }
    std::cout << "\n";
    
    for (int i = 0; i < productos; i++) {
        std::cout << "P" << i+1 << "\t";
        for (int j = 0; j < semanas; j++) {
            std::cout << plan[i][j] << "\t";
        }
        std::cout << "\n";
    }
}

void agregarSemana(float **&plan, int &semanas, int productos)
{
    std::cout << "\nAgregando Semana " << semanas + 1 << "...\n";
    
    float **nuevoPlan = new float*[productos];
    
    for (int i = 0; i < productos; i++) {
        nuevoPlan[i] = new float[semanas + 1];
        
        for (int j = 0; j < semanas; j++) {
            nuevoPlan[i][j] = plan[i][j];
        }
        
        nuevoPlan[i][semanas] = 0.0;
    }
    
    for (int i = 0; i < productos; i++) {
        delete[] plan[i];
    }
    delete[] plan;
    
    plan = nuevoPlan;
    semanas++;
    
    std::cout << "Matriz redimensionada a " << productos << "x" << semanas 
              << ". Memoria gestionada con exito!\n";
}

void modificarProduccion(float **plan, int productos, int semanas)
{
    int prod, sem;
    float cant;
    
    std::cout << "\nIngrese Producto (0-" << productos-1 << "): ";
    std::cin >> prod;
    
    if (!validarEntrada() || prod < 0 || prod >= productos) {
        std::cout << "Producto invalido.\n";
        return;
    }
    
    std::cout << "Ingrese Semana (0-" << semanas-1 << "): ";
    std::cin >> sem;
    
    if (!validarEntrada() || sem < 0 || sem >= semanas) {
        std::cout << "Semana invalida.\n";
        return;
    }
    
    std::cout << "Cantidad a producir: ";
    std::cin >> cant;
    
    if (!validarEntrada() || cant < 0) {
        std::cout << "Cantidad invalida.\n";
        return;
    }
    
    plan[prod][sem] = cant;
    std::cout << "Produccion actualizada.\n";
}

void calcularCOGS(float **plan, float *costos, float *mp, 
                 int **reqs, int productos, int semanas)
{
    int sem;
    std::cout << "\nIngrese Semana para el calculo de costos (0-" << semanas-1 << "): ";
    std::cin >> sem;
    
    if (!validarEntrada() || sem < 0 || sem >= semanas) {
        std::cout << "Semana invalida.\n";
        return;
    }
    
    std::cout << "\n--- Reporte de Costos (Semana " << sem+1 << ") ---\n";
    std::cout << std::fixed << std::setprecision(2);
    
    float produccionTotal = 0.0;
    float costoProduccion = 0.0;
    
    for (int i = 0; i < productos; i++) {
        produccionTotal += plan[i][sem];
        costoProduccion += plan[i][sem] * costos[i];
    }
    
    std::cout << "Produccion Total (Semana " << sem+1 << "): " 
              << produccionTotal << " unidades.\n";
    
    float consumoMP[4] = {0, 0, 0, 0};
    for (int i = 0; i < productos; i++) {
        for (int j = 0; j < 4; j++) {
            consumoMP[j] += plan[i][sem] * reqs[i][j];
        }
    }
    
    std::cout << "\nConsumo de Materia Prima: \n";
    std::cout << "  MP-01: " << consumoMP[0] << " kg\n";
    std::cout << "  MP-02: " << consumoMP[1] << " kg\n";
    std::cout << "  MP-03: " << consumoMP[2] << " kg\n";
    std::cout << "  MP-04: " << consumoMP[3] << " unidades\n";
    
    float costoUnitarioMP = 10.0;
    float inventarioInicial = 0.0;
    float inventarioFinal = 0.0;
    
    for (int i = 0; i < 4; i++) {
        inventarioInicial += mp[i] * costoUnitarioMP;
        float restante = mp[i] - consumoMP[i];
        if (restante < 0) {
            std::cout << "\n¡ADVERTENCIA! Falta materia prima MP-0" << i+1 << "\n";
            restante = 0;
        }
        inventarioFinal += restante * costoUnitarioMP;
    }
    
    // fórmula COGS: Inv.Inicial + Compras - Inv.Final
    float COGS = inventarioInicial + costoProduccion - inventarioFinal;
    
    std::cout << "\n--- Calculo COGS ---\n";
    std::cout << "Inventario Inicial de M.P.: $" << inventarioInicial << "\n";
    std::cout << "Costo de Produccion (Compras): $" << costoProduccion << "\n";
    std::cout << "Inventario Final de M.P.: $" << inventarioFinal << "\n";
    std::cout << "COGS (Costo de Bienes Vendidos): $" << COGS << "\n";
}

void reporteConsumo(float **plan, int **reqs, int productos, int semanas)
{
    int sem;
    std::cout << "\nIngrese Semana para reporte de consumo (0-" << semanas-1 << "): ";
    std::cin >> sem;
    
    if (!validarEntrada() || sem < 0 || sem >= semanas) {
        std::cout << "Semana invalida.\n";
        return;
    }
    
    std::cout << "\n--- Reporte de Consumo de MP (Semana " << sem+1 << ") ---\n";
    std::cout << std::fixed << std::setprecision(2);
    
    float consumoMP[4] = {0, 0, 0, 0};
    
    std::cout << "\nDetalle por Producto:\n";
    for (int i = 0; i < productos; i++) {
        std::cout << "Producto " << i+1 << " (Produccion: " << plan[i][sem] << " uni):\n";
        for (int j = 0; j < 4; j++) {
            float consumo = plan[i][sem] * reqs[i][j];
            consumoMP[j] += consumo;
            std::cout << "  MP-0" << j+1 << ": " << consumo 
                     << (j < 3 ? " kg" : " uni") << "\n";
        }
    }
    
    std::cout << "\nConsumo Total:\n";
    std::cout << "MP-01: " << consumoMP[0] << " kg\n";
    std::cout << "MP-02: " << consumoMP[1] << " kg\n";
    std::cout << "MP-03: " << consumoMP[2] << " kg\n";
    std::cout << "MP-04: " << consumoMP[3] << " unidades\n";
}

void expandirProductos(float **&plan, float *&costos, int **&reqs, 
                      int &productos, int semanas)
{
    std::cout << "\n--- Expandiendo Sistema: Agregar Nuevo Producto ---\n";
    
    float **nuevoPlan = new float*[productos + 1];
    
    for (int i = 0; i < productos; i++) {
        nuevoPlan[i] = new float[semanas];
        for (int j = 0; j < semanas; j++) {
            nuevoPlan[i][j] = plan[i][j];
        }
    }
    
    nuevoPlan[productos] = new float[semanas];
    for (int j = 0; j < semanas; j++) {
        nuevoPlan[productos][j] = 0.0;
    }
    
    for (int i = 0; i < productos; i++) {
        delete[] plan[i];
    }
    delete[] plan;
    
    float *nuevosCostos = new float[productos + 1];
    for (int i = 0; i < productos; i++) {
        nuevosCostos[i] = costos[i];
    }
    
    std::cout << "Ingrese costo unitario del Producto " << productos + 1 << ": $";
    std::cin >> nuevosCostos[productos];
    
    if (!validarEntrada()) {
        std::cout << "Entrada invalida. Operacion cancelada.\n";
        delete[] nuevosCostos;
        for (int i = 0; i <= productos; i++) {
            delete[] nuevoPlan[i];
        }
        delete[] nuevoPlan;
        return;
    }
    
    delete[] costos;
    
    int **nuevosReqs = new int*[productos + 1];
    for (int i = 0; i < productos; i++) {
        nuevosReqs[i] = new int[4];
        for (int j = 0; j < 4; j++) {
            nuevosReqs[i][j] = reqs[i][j];
        }
    }
    
    nuevosReqs[productos] = new int[4];
    std::cout << "\nIngrese requerimientos de MP para el nuevo producto:\n";
    for (int j = 0; j < 4; j++) {
        std::cout << "  MP-0" << j+1 << ": ";
        std::cin >> nuevosReqs[productos][j];
        if (!validarEntrada()) {
            std::cout << "Entrada invalida. Usando valor 0.\n";
            nuevosReqs[productos][j] = 0;
        }
    }
    
    for (int i = 0; i < productos; i++) {
        delete[] reqs[i];
    }
    delete[] reqs;
    
    plan = nuevoPlan;
    costos = nuevosCostos;
    reqs = nuevosReqs;
    productos++;
    
    std::cout << "\n¡Producto " << productos << " agregado exitosamente!\n";
    std::cout << "Sistema expandido a " << productos << " productos.\n";
}

void cargarDatosIniciales(float **plan, int productos, int semanas)
{
    std::cout << "\n--- Cargar Datos de Produccion ---\n";
    std::cout << "Ingrese datos para cada producto y semana:\n";
    
    for (int i = 0; i < productos; i++) {
        std::cout << "\nProducto " << i+1 << ":\n";
        for (int j = 0; j < semanas; j++) {
            std::cout << "  Semana " << j+1 << ": ";
            std::cin >> plan[i][j];
            if (!validarEntrada() || plan[i][j] < 0) {
                std::cout << "  Entrada invalida. Usando valor 0.\n";
                plan[i][j] = 0.0;
            }
        }
    }
    
    std::cout << "\nDatos cargados exitosamente.\n";
}

void liberar(float **&plan, float *&costos, float *&mp, 
            int **&reqs, int productos, int semanas)
{
    std::cout << "\nLiberando memoria...\n";
    
    if (plan != nullptr) {
        for (int i = 0; i < productos; i++) {
            delete[] plan[i];
        }
        delete[] plan;
        plan = nullptr;
    }
    
    if (costos != nullptr) {
        delete[] costos;
        costos = nullptr;
    }
    
    if (mp != nullptr) {
        delete[] mp;
        mp = nullptr;
    }
    
    if (reqs != nullptr) {
        for (int i = 0; i < productos; i++) {
            delete[] reqs[i];
        }
        delete[] reqs;
        reqs = nullptr;
    }
    
    std::cout << "Memoria liberada correctamente.\n";
}