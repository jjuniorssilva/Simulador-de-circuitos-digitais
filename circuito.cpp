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
Circuito::~Circuito(){}

Circuito::Circuito(const Circuito& C){

}

Circuito::Circuito(Circuito&& C){}


//falta_fazer();

std::ostream& operator<<(std::ostream& O, const Circuito& C){

}
/// ***********************
/// Funcoes de testagem
/// ***********************

// Retorna true se IdInput eh uma id de entrada do circuito valida (entre -1 e -NInput)
bool Circuito::validIdInput(int IdInput) const
{
  return (IdInput<=-1 && IdInput>=-getNumInputs());
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
bool Circuito::validPort(int IdPort) const
{
  if (!definedPort(IdPort)) return false;
  for (int j=0; j<getNumInputsPort(IdPort); j++)
  {
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


int Circuito::getNumInputs() const{
    return out_circ.size();
}
int Circuito::getNumOutputs() const{
     return id_out.size();
}
int Circuito::getNumPorts() const{
    return ports.size();
}
int Circuito::getIdOutput(int IdOutput) const{
    if(validIdOutput(IdOutput))
       {
           return id_out[IdOutput-1];
       }
       else return 0;
}

bool3S Circuito::getOutput(int IdOutput) const{
    if(validIdOutput(IdOutput))
        {
            return out_circ[IdOutput-1];
        }
        else return bool3S::UNDEF;
}
int Circuito::getNumInputsPort(int IdPort) const{
    if (definedPort(IdPort))
        {
            return ports[IdPort - 1] -> getNumInputs();
        }
        else return 0;
}

int Circuito::getId_inPort(int IdPort, int I) const{
    {
        if (definedPort(IdPort))  // && testar o indice da entrada I
        {
            return ports[IdPort - 1] -> getId_in(I);
        }
        else return 0;
    }
}
std::string Circuito::getNamePort(int IdPort) const{
    if (definedPort(IdPort))
        {
            return ports[IdPort-1]->getName();
        }
        else return "??";
}
/// ***********************
/// Funcoes de modificacao
/// ***********************
void Circuito::setIdOutput(int IdOut, int IdOrig){

}
void Circuito::setPort(int IdPort, std::string Tipo, int NIn){

}
void Circuito::setId_inPort(int IdPort, int I, int IdOrig) const{

}


/// ***********************
/// E/S de dados
/// ***********************

void Circuito::digitar(int NInputs, int NOutputs, int NPortas){

}
bool Circuito::ler(const std::string& arq){
    ifstream I(arq.c_str());
        // O resultado logico da leitura
        bool resultado=true;
        try{
            if (!I.is_open()) throw 1;
            string ps;
            int Nin,Nout,Nports;

            I >> ps >> Nin>> Nout >> Nports;
            if (!I.good() || ps!="CIRCUITO") throw 2;
            //clear();
            //resize(Nin,Nout,Nports);
            I >> ps;
            if(ps!="PORTAS") throw 3;

            for (int i=0; i<Nports; i++){
               I>>ps; // lê id 1) antes do nome da porta
               if(ps!=to_string(i)+")") throw 4;
               I >> ps; // lê o tipo d porta
               if(!validType(ps)) throw 5;

               ptr_Port new_port = allocPort(ps);
               if (!new_port->ler(I)) throw 5;
    //           for(int j=0; j<new_port->getNumInputs(); j++){
    //               if(!validIdInput(new_port->getId_in(j))) throw 4;
    //           }

            }
            I >> ps;
            if(ps!="SAIDAS") throw 3;
            for (int i=0; i<Nout; i++){
                I>>ps; // lê id 1) antes do nome da porta
            if(ps!=to_string(i)+")") throw 4;
            }
        }
        catch (int i)
         {
           //clear();
           resultado = false;
         }
         if (I.is_open()) I.close();
         return resultado;
}
std::ostream& Circuito::imprimir(std::ostream& arq) const{

}
bool Circuito::salvar(const std::string& arq) const{
    return true;
}


/// ***********************
/// SIMULACAO (funcao principal do circuito)
/// ***********************

bool Circuito::simular(const std::vector<bool3S>& in_circ){
    bool tudo_def,alguma_def;
        std::vector<bool3S> in_port;
        int id;

        for(unsigned i=0;i<ports.size();i++)
        {

            ports[i]->setOutput(bool3S::UNDEF);
        }

        do
        {

            tudo_def = true;
            alguma_def = false;

            for (unsigned i = 0; i < ports.size();i++)
            {

                if (ports[i] -> getOutput() == bool3S::UNDEF)
                {
                    for (unsigned j = 0; j < ports[i] -> getNumInputs(); j++)
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
        }while(!tudo_def && alguma_def);

        return true;
}
