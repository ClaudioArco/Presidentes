#include <iostream>
#include <vector>
#include <list>
#include <stack>
#include <set>
#include <queue>
#include <ctime>
#include <fstream>
#include <string> 

using namespace std;


int calculaconveniencia(int n, int **c, int a[]);
int ** allocate(int r, int c);
double uniforme();
void generaconveniencias(int n, int **c);
void generapermutaciones(int m, int a[], int & optimo, int n, int **c);
int greedy(int n, int **conveniencia);
int ** matrizProfe(int option, int &number_presidentes);

class Solucion {
    private:

        //El orden que lleva la mesa es decir la fila de presidentes que estan sentados
        //contiguamente
        vector<int> ordenPresidentesSentados;
        //Por cada celda indica si el presidente sentado
        vector<bool> presidentesSentados;
        int** conveniencias;
        int ultimo_sentado;
        int convenienciaAcumulada;
        int numPresidentes;
        int cota_local;
        int max_conveniencia;


    public:
        Solucion(const int presidentes){
            numPresidentes = presidentes;
            convenienciaAcumulada = 0;
            ultimo_sentado = -1;
            max_conveniencia = 0;
            cota_local = 0;
            for (int i = 0; i < numPresidentes; i++)
                presidentesSentados.push_back(false);
        }


        bool Factible(int candidato ) {
            
            if (presidentesSentados[candidato] == false)
                return true;
            else 
                return false;

        }

        void actualizarVariables(int candidato){

            ordenPresidentesSentados.push_back(candidato);
            presidentesSentados[candidato] = true;
            ultimo_sentado = candidato;

            convenienciaAcumulada = getConveniencia();

            int estimacion = (numPresidentes - ordenPresidentesSentados.size()) * 2 ;
            cota_local = convenienciaAcumulada + (estimacion * max_conveniencia);

        }

        int CotaLocal() const{
            return cota_local;
        };    
        
        bool EsSolucion() const{
            return (ordenPresidentesSentados.size() == numPresidentes);
        }

        void asignar_matriz(int **matriz){
            conveniencias = matriz;
            overrideCotaLocal();
        }

        void overrideCotaLocal(){
            for(int i = 0; i < numPresidentes; i++){
                for(int j = 0; j < numPresidentes; j++){
                    if(conveniencias[i][j] > max_conveniencia){
                        max_conveniencia = conveniencias[i][j];
                    }
                }
            }
            
            //Obtenemos el maximo de las conveniencias ya que en algunas matrices
            //puede llegar a 1000 y le sumamos una cantidad un poco mayor para que
            //siempre la cota local sea optimista
            max_conveniencia+=18;
            cota_local = (numPresidentes * 2) * max_conveniencia;
        }

        int getConvenienciaAcumulada(){
            return convenienciaAcumulada;
        }

        int getConveniencia(){
            int asientosCopia[ordenPresidentesSentados.size()];
            for(int i = 0; i < ordenPresidentesSentados.size(); i++){
                asientosCopia[i] = ordenPresidentesSentados[i];
            }

            return calculaconveniencia(ordenPresidentesSentados.size(), conveniencias, asientosCopia);
        }

 
};

struct ComparaCostes {
    bool operator()(const Solucion e, const Solucion a){
        return e.CotaLocal() > a.CotaLocal();
    }
};

Solucion Branch_and_Bound(int numPresidentes, int ** matriz, int optimo )
{

    priority_queue<Solucion, vector<Solucion>, ComparaCostes> Q;
    
    Solucion nodo_e(numPresidentes);
    Solucion mejor_Solucion(numPresidentes);

    float cotaGlobal = greedy(numPresidentes, matriz);
    float conveniencia_actual = 0.0;

    int podas = 0;
    int maximosVivos = 0;
    int nodosExpandidos = 0;
    clock_t tantes;
    clock_t tdespues;

    tantes = clock();

    nodo_e.asignar_matriz(matriz);

    Q.push(nodo_e);

    while ( !Q.empty() ){ 
        
        nodo_e = Q.top();
        Q.pop();

        if(nodo_e.EsSolucion()){
            if(nodo_e.getConveniencia() > conveniencia_actual){
                mejor_Solucion = nodo_e;
                conveniencia_actual = nodo_e.getConveniencia();
            }
        } else {
            for ( int i = 0; i < numPresidentes; i++) {
                Solucion nodoHijo = nodo_e;
                if ( nodoHijo.Factible(i) and nodoHijo.CotaLocal() >= cotaGlobal) {   
                    nodoHijo.actualizarVariables(i);
                    Q.push(nodoHijo);
                    nodosExpandidos++;
                } else {
                    podas++;
                }
            }
        }

        if(Q.size() > maximosVivos){
            maximosVivos = Q.size();
        } 
    }

    tdespues = clock();
    cout << "Tiempo para: "<< numPresidentes << " presidentes " << (double)(tdespues - tantes) / CLOCKS_PER_SEC << endl;
    cout << "Podas realizadas: " << podas << endl;
    cout << "Nodos expandidos: " << nodosExpandidos << endl;
    cout << "Máximos vivos: " << maximosVivos << endl;

    return mejor_Solucion;
}



int main(int argc, char ** argv) {

    int optimo = -1;
    int n = atoi(argv[1]); //Numero de presidentes
    int **conveniencia = allocate(n, n);

    generaconveniencias(n, conveniencia);

    if (n <= 10) {
        
        int a[n];
        for (int i=0; i<n; i++)
            a[i]=i;
        generapermutaciones(n, a, optimo, n, conveniencia);
        cout << "Resultado óptimo generado por permutaciones: " << optimo << endl;
    }
    else {
        cout << "No se muestra el resultado óptimo debido al elevado número de presidentes" << endl;
    }
    
    //Comentar lo de arriba y descomentar esto de abajo para cargar las matrices del profesor
    //el archivo que contiene la matriz debe estar en el directorio
    //int **conveniencia = matrizProfe(1, n);
    
    Solucion BB = Branch_and_Bound(n, conveniencia, optimo);
    cout << "Resultado B&B: " << BB.getConveniencia() << endl;

    return 0;
}

int ** matrizProfe(int option, int &number_presidentes){
    string nombreArchivo = "";
    int tam = 18;
    switch(option){
        case 1:
            nombreArchivo = "matriz18-1.txt";

            break;
        case 2:
            nombreArchivo = "matriz18-2.txt";
            break;
        case 3:
            nombreArchivo = "matriz18-3.txt";
            break;
        case 4:
            nombreArchivo = "matriz20-2m.txt";
            tam = 20;
            break;
        case 5:
            nombreArchivo = "matriz20-2mm.txt";
            tam = 20;
            break;
    
    }

    ifstream archivo(nombreArchivo.c_str());
    string linea;
    vector<vector<int> > matrizCompleta;
    vector<int> numerosLinea;
    string number;

    while (getline(archivo, linea)) {
        numerosLinea.clear();
        for(int i = 1; i < linea.size(); i++){
            //cout << linea[i] << endl;
            if(linea[i] == ' ' and number != ""){
                int aux = stoi(number);
                number = "";
                numerosLinea.push_back(aux);
                //cout << aux << " "; 

            } else {
                number += linea[i];
            }
        }
        matrizCompleta.push_back(numerosLinea);
        //cout << endl;
    }

    int ** conveniencias = allocate(tam, tam);
    for(int i = 0; i < tam; i++){
        for(int j = 0; j < tam; j++){
            conveniencias[i][j] = matrizCompleta[i][j];
        }
    }

    number_presidentes = tam;

    return conveniencias;


}

int greedy(int n, int **conveniencia){

    bool sentados[n];
    int asientos[n];

    for(int i = 0; i < n; i++){
        asientos[i] = -1;
        sentados[i] = false;
    }

    sentados[0] = true; 
    asientos[0] = 0;

    for (int i = 0; i < n-1; i++) {
        int p_sentado = asientos[i];
        int maximo = -1;
        int candidato = -1;
        for (int j = 0; j < n; j++) {
            if (!sentados[j]) {
                int actual = conveniencia[p_sentado][j];
                if (actual > maximo) {
                    maximo = actual;
                    candidato = j;
                }
            }
        }
        sentados[candidato] = true;
        asientos[i+1] = candidato;
    }

    return calculaconveniencia(n, conveniencia, asientos);
}

int calculaconveniencia(int n, int **c, int a[]) {
    int suma = 0;
    for (int i = 1; i < n-1; i++)
        suma += c[a[i]][a[i-1]]+c[a[i]][a[i+1]]; // desde 1 hasta n-2

    suma+=c[a[0]][a[n-1]]+c[a[0]][a[1]]; //se añade los que están al lado de 0
    suma+=c[a[n-1]][a[n-2]]+c[a[n-1]][a[0]]; //se añade los que están al lado de n-1
    return suma;
}

void generapermutaciones(int m, int a[], int & optimo, int n, int **c) {
    if (m==1) {
        int aux = calculaconveniencia(n, c, a);
        if (aux > optimo) 
            optimo = aux;
    }
    else
        for (int i= 0; i < m; i++) {
            generapermutaciones(m-1, a, optimo, n, c);
            if (m%2==0) swap(a[i],a[m-1]);
            else
                swap(a[0],a[m-1]);
            }
}

void generaconveniencias(int n, int **c) {
 srand(time(0));
    for (int i = 0; i < n-1; i++) {
        for (int j = i+1; j < n; j++) {
                double u = uniforme();
                c[i][j] = (int)(u*101);
                c[j][i] = c[i][j];
            }
            for (int i = 0; i < n; i++) 
                c[i][i] = 0;           
    }
}

double uniforme() {
    int t = rand();
    double f = ((double)RAND_MAX+1.0);
    return (double)t/f;
}

int ** allocate(int r, int c) {
    int ** block;
    block = new int *[r];
    
    for (int i = 0; i < r; i++) {
        block[i] = new int[c];
    }
    // allocate memory into block
    return block;
}