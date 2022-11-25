#include <fstream>
#include <utility> // para std::swap
#include "circuito.h"
#include <vector>
#include <iostream>
#include <iomanip>
using namespace std;
///
/// As strings que definem os tipos de porta
///

// Funcao auxiliar que testa se uma string com nome de porta eh valida
// Caso necessario, converte os caracteres da string para maiusculas
bool validType(std::string& Tipo)
{
    if (Tipo.size()!=2) return false;
    Tipo.at(0) = toupper(Tipo.at(0));
    Tipo.at(1) = toupper(Tipo.at(1));
    if (Tipo=="NT" ||
            Tipo=="AN" || Tipo=="NA" ||
            Tipo=="OR" || Tipo=="NO" ||
            Tipo=="XO" || Tipo=="NX") return true;
    return false;
}

// Funcao auxiliar que retorna um ponteiro que aponta para uma porta alocada dinamicamente
// O tipo da porta alocada depende do parametro string de entrada (AN, OR, etc.)
// Caso o tipo nao seja nenhum dos validos, retorna nullptr
// Pode ser utilizada nas funcoes: Circuito::setPort, Circuito::digitar e Circuito::ler
ptr_Port allocPort(std::string& Tipo)
{
    if (!validType(Tipo)) return nullptr;
    if (Tipo=="NT") return new Port_NOT;
    if (Tipo=="AN") return new Port_AND;
    if (Tipo=="NA") return new Port_NAND;
    if (Tipo=="OR") return new Port_OR;
    if (Tipo=="NO") return new Port_NOR;
    if (Tipo=="XO") return new Port_XOR;
    if (Tipo=="NX") return new Port_NXOR;

    // Nunca deve chegar aqui...
    return nullptr;
}

///
/// CLASSE CIRCUITO
///

/// ***********************
/// Inicializacao e finalizacao
/// ***********************
Circuito::Circuito():id_out(),out_circ(),ports()
{
}
Circuito::~Circuito()
{
}
Circuito::Circuito(const Circuito& C)
{
    Nin = C.Nin;
    id_out = C.id_out;
    out_circ = C.out_circ;

    for(unsigned i=0; i<C.getNumPorts(); i++) {
        ports.push_back(C.ports[i]->clone());
    }
}

void Circuit::operator=(const Circuit& C)
{
    for(unsigned i=0; i<ports.size(); i++)if (ports[i]!=nullptr) delete ports[i];
    for (unsigned i = 0; i < C.getNumPorts(); i++) ports.push_back(C.ports[i] ->clone());
    for ( unsigned i = 0; i < C.getNumOutputs(); i++)id_out.push_back(C.id_out[i]);
    for (unsigned i = 0; i < C.getNumInputs(); i++) out_circ.push_back(C.out_circ[i]);

}

void Circuito::clear()
{
       id_out.clear();
       out_circ.clear();
       Nin = 0;
       for(unsigned i=0; i<ports.size(); i++){
           if (ports[i]!=nullptr) delete ports[i];
       }
       ports.clear();
}
void Circuito::resize(unsigned NI, unsigned NO, unsigned NP){
    if(NI<=0 && NO<=0 && NP<=0) return;
    clear();
    Nin = NI;
    id_out.resize(NO);
    out_circ.resize(NO, bool3S::UNDEF);
    ports.resize(NP, nullptr);
}

/// ***********************
/// Funcoes de testagem
/// ***********************

// Retorna true se IdInput eh uma id de entrada do circuito valida (entre -1 e -NInput)
bool Circuito::validIdInput(int IdInput) const
{
    return (IdInput<=-1 && IdInput>=-int(getNumInputs()));
}

// Retorna true se IdOutput eh uma id de saida do circuito valida (entre 1 e NOutput)
bool Circuito::validIdOutput(int IdOutput) const
{
    return (IdOutput>=1 && IdOutput<=getNumOutputs());
}

// Retorna true se IdPort eh uma id de porta do circuito valida (entre 1 e NPort)
bool Circuito::validIdPort(int IdPort) const
{
    return (IdPort>=1 && IdPort<=getNumPorts());
}

// Retorna true se IdOrig eh uma id valida para a origem do sinal de uma entrada de porta ou
// para a origem de uma saida do circuito (podem vir de uma entrada do circuito ou de uma porta)
// validIdOrig == validIdInput OR validIdPort
bool Circuito::validIdOrig(int IdOrig) const
{
    return validIdInput(IdOrig) || validIdPort(IdOrig);
}

// Retorna true se IdPort eh uma id de porta valida (validIdPort) e
// a porta estah definida (estah alocada, ou seja, != nullptr)
bool Circuito::definedPort(int IdPort) const
{
    if (!validIdPort(IdPort)) return false;
    if (ports.at(IdPort-1)==nullptr) return false;
    return true;
}

// Retorna true se IdPort eh uma porta existente (definedPort) e
// todas as entradas da porta com Id de origem valida (usa getId_inPort e validIdOrig)
bool Circuito::validPort(int IdPort) const{
    if (!definedPort(IdPort)) return false;
    for (int j=0; j<getNumInputsPort(IdPort); j++){
        if (!validIdOrig(getId_inPort(IdPort,j))) return false;
    }
}

// Retorna true se o circuito eh valido (estah com todos os dados corretos):
// - numero de entradas, saidas e portas valido (> 0)
// - todas as portas validas (usa validPort)
// - todas as saidas com Id de origem validas (usa getIdOutput e validIdOrig)
// Essa funcao deve ser usada antes de salvar ou simular um circuito
bool Circuito::valid() const
{
    if (getNumInputs()<=0) return false;
    if (getNumOutputs()<=0) return false;
    if (getNumPorts()<=0) return false;
    for (int i=0; i<getNumPorts(); i++)
    {
        if (!validPort(i+1)) return false;
    }
    for (int i=0; i<getNumOutputs(); i++)
    {
        if (!validIdOrig(getIdOutput(i+1))) return false;
    }
    return true;

}

/// ***********************
/// Funcoes de consulta
/// ***********************


unsigned Circuito::getNumInputs() const
{
    return Nin;
}
unsigned Circuito::getNumOutputs() const
{
    return out_circ.size();
}
unsigned Circuito::getNumPorts() const
{
    return ports.size();
}
int Circuito::getIdOutput(int IdOutput) const
{
    if(validIdOutput(IdOutput))
    {
        return id_out[IdOutput-1];
    }
    else
    {
        return 0;
    }
}

bool3S Circuito::getOutput(int IdOutput) const
{
    if(validIdOutput(IdOutput))
    {
        return out_circ[IdOutput-1];
    }
    else
    {
        return bool3S::UNDEF;
    }
}
unsigned Circuito::getNumInputsPort(int IdPort) const
{
    if (definedPort(IdPort))
    {
        return ports[IdPort - 1] -> getNumInputs();
    }
    else
    {
        return 0;
    }
}

int Circuito::getId_inPort(int IdPort, unsigned I) const
{
    if (definedPort(IdPort) && validIdPort(I))  // && testar o indice da entrada I
    {
        return ports[IdPort - 1] -> getId_in(I);
    }
    else
    {
        return 0;
    }
}
std::string Circuito::getNamePort(int IdPort) const
{
    if (definedPort(IdPort))
    {
        return ports[IdPort-1]->getName();
    }
    else
    {
        return "??";
    }
}
/// ***********************
/// Funcoes de modificacao
/// ***********************
void Circuito::setIdOutput(int IdOut, int IdOrig)
{
    if(validIdOutput(IdOut) && validIdOrig(IdOrig))
    {
        id_out[IdOut-1] = IdOrig;
    }
}
void Circuito::setPort(int IdPort, std::string Tipo, unsigned NIn)
{
    delete ports[IdPort-1];
    ports[IdPort-1] = allocPort(Tipo);
    ports[IdPort-1]->setNumInputs(NIn);
}
void Circuito::setId_inPort(int IdPort, unsigned I, int IdOrig) const
{
    if(definedPort(IdPort) && validIdOrig(IdOrig))
    {
        ports[IdPort-1]->setId_in(I, IdOrig);
    }
}

/// ***********************
/// E/S de dados
/// ***********************

void Circuito::digitar()
{
    // essa parte sai na interface
    int Nin, Nout, Nports;
    do{
        cout<<"Digite os dados a seguir:";
        cout << "Entradas do circuito: ";
        cin >> Nin;
        cout << "Saidas do circuito: ";
        cin >> Nout;
        cout << "Portas no circuito: ";
        cin >> Nports;
    }
    while(Nin<=0 || Nout<=0 || Nports<=0);
    // -------------------------
    clear();
    resize(Nin, Nout, Nports);
    string portaTipo;
    unsigned totalPortas=0;
    do{
        // aqui retorna um erro para a interface e não repete
        do{
            cout << "Informe o tipo da porta " << totalPortas+1 << ": ";
            cin >> portaTipo;
        }while(!validType(portaTipo));

        ports[totalPortas] = allocPort(portaTipo);
        //ports[totalPortas] = (&pont) -> clone();
        ports[totalPortas]->digitar();
        totalPortas++;
    }while((int)totalPortas < Nports);
    int posIdOut, contador=0;
    do{
        cout << "Digite o id da saida "  << contador+1 << ": ";
        cin >> posIdOut;

        if(!validIdOrig(posIdOut)){
            while(!validIdOrig(posIdOut)){
               cout << "Id invalido, digite novamente: ";
               cin >> posIdOut;
            }
        }
        id_out[contador] = posIdOut;
        contador++;

      }while(contador < Nout);
}
bool Circuito::ler(const std::string& arq)
{
    ifstream I(arq.c_str());
    bool resultado=true;
    try{
         if (!I.is_open()) throw 1;
         string ps;
         int Nin,Nout,Nports;
         I >> ps >> Nin>> Nout >> Nports;
         if (!I.good() || ps!="CIRCUITO"|| Nin<=0 || Nout<=0 || Nports<=0) throw 2;
         clear();
         resize(Nin,Nout,Nports);
         I >> ps;
         if(ps!="PORTAS") throw 3;

         for (int i=0; i<Nports; i++){
            I>>ps; // lê id 1) antes do nome da porta
            if(ps!=to_string(i)+")") throw 4;
            I >> ps; // lê o tipo d porta
            if(!validType(ps)) throw 5;
            ports[i] = allocPort(ps);
            if (!ports[i]->ler(I)) throw 6;
         }
         //saidas
         I >> ps;
         if(ps!="SAIDAS") throw 7;
         for (int i=0; i<Nout; i++){
            I>>ps; // lê id 1) antes do nome da porta
            if(ps!=to_string(i)+")") throw 8;
            I>>ps; //lê o id de saida
            if(!validIdOrig(stoi(ps))) throw 9;
            id_out[i]=stoi(ps);
         }
        }
        catch (int i){
           cout<<"Arquivo de leitura Invalido";
           clear();
           resultado = false;
         }
         if (I.is_open()) I.close();
         return resultado;
}
std::ostream& Circuito::imprimir(std::ostream& arq) const
{
    //Exemplo de arquivos validos
    //CIRCUITO 2 1 3
    //PORTAS
    //1) OR 2: -1 -2
    //2) NT 1: -2
    //3) AN 2: 1 2
    //SAIDAS
    //1) 3
    arq<<"CIRCUITO"<<" "<<out_circ.size()<<" "<<id_out.size()<<" "<<ports.size()<<'\n'<<"PORTAS";
    for (unsigned i = 0; i < ports.size(); i++)
    {
        arq<<i<<')';
        ports[i]->imprimir(arq);
    }
    arq<<'\n'<<"SAIDAS";
    for(unsigned i=0;i<id_out.size();i++)
    {
        arq<<i<<')'<<" "<<id_out[i];
    }

    return arq;
}
bool Circuito::salvar(const std::string& arq) const
{
    if(!valid()) return false;
    ofstream arq1(arq);
    if (!arq1.is_open()) return false;
    imprimir(arq1);
    arq1.close();
    return true;
}


/// ***********************
/// SIMULACAO (funcao principal do circuito)
/// ***********************

bool Circuito::simular(const std::vector<bool3S>& in_circ)
{
    bool tudo_def,alguma_def;
    std::vector<bool3S> in_port;
    int id;

    for(unsigned i=0; i<getNumPorts()-1; i++)
    {

        ports[i]->setOutput(bool3S::UNDEF);
    }
    do
    {
        tudo_def = true;
        alguma_def = false;
        for (unsigned i = 0; i < getNumPorts()-1; i++)
        {
            if (ports[i] -> getOutput() == bool3S::UNDEF)
            {
                for (unsigned j = 0; j < ports[i] -> getNumInputs()-1; j++)
                {

                    id = ports[i]->getId_in(j);
                    if(id > 0)
                    {
                        in_port[j] = ports[id-1]->getOutput();
                    }
                    else
                    {
                        in_port[j] = in_circ[-id-1];
                    }
                }
                ports[i]->simular(in_port);
                if(ports[i]->getOutput()== bool3S::UNDEF)
                {

                    tudo_def = false;
                }
                else
                {
                    alguma_def = true;
                }
            }
        }
    }
    while(!tudo_def && alguma_def);
    // DETERMINAÇÃO DAS SAÍDAS

    for(unsigned j=0; j<getNumOutputs()-1; j++)
    {
        id = id_out[j];
        if(id > 0)
        {
            out_circ[j] = ports[id-1]->getOutput();
        }
        else
        {
            out_circ[j] = in_circ[-id-1];
        }
    }
    return true;
}
