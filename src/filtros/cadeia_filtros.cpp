#include"cadeia_filtros.hpp"
#include<cstdint>
#include<iostream>

//construtor
CadeiaFiltros :: CadeiaFiltros(uint32_t capacidade_inicial)
: capacidade(capacidade_inicial), quantidade(0){

    // Evita inicialização com 0
    if (capacidade == 0) {
        capacidade = 1;
    }
    
    //trava de segurança, impede que o usuário encadeie mais de 64 filtros
    if (capacidade > capacidade_max_filtros){
        capacidade = capacidade_max_filtros;
    }

    filtros = new FiltroAudio*[capacidade];
}

//destrutor
CadeiaFiltros::~CadeiaFiltros(){
    delete[] filtros;
}

//redimensiona tamanho inicial da cadeia
void CadeiaFiltros::redimensionar(){
    uint32_t nova_capacidade= capacidade*2;

    //trava de segurança
    if(nova_capacidade > capacidade_max_filtros){
        nova_capacidade = capacidade_max_filtros;
    }
    //se ja estiver no máximo só retorna
    if(nova_capacidade == capacidade){
        return;
    }

    FiltroAudio** novo_array = new FiltroAudio* [nova_capacidade];
    for(uint32_t i=0;i<capacidade;i++){
        novo_array[i] = filtros[i];
    }

    delete[] filtros;
    filtros = novo_array;
    capacidade = nova_capacidade;

}

bool CadeiaFiltros::adicionar_filtro(FiltroAudio* filtro){
    //verifica se o filtro é um objeto válido existente
    if(filtro == nullptr){
        return false;
    }
    
    //trava de segurança
    if(quantidade >= capacidade_max_filtros){
        std::cerr << "Alerta: Limite máximo de filtros (" << capacidade_max_filtros << ") atingida!";
        return false;
    }

    //se precisar de mais espaço 
    if(quantidade >= capacidade){
        redimensionar();
    }

    //aloca o objeto filtro no vetor da cadeia e incrementa a quantidade
    filtros[quantidade] = filtro;
    quantidade++;
    return true;
}

//analisa se o vetor da cadeia não está vazio e chama o método processar da classe do objeto no indice atual, ou seja, aplica filtros de forma encadeada
void CadeiaFiltros::processar(DadosAudio &audio){
    for(uint32_t i =0; i < quantidade; i++){
        if(filtros[i]!= nullptr){
            filtros[i]->processar(audio);
        }
    }

}


