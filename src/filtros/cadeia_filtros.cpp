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

    filtros = std::make_unique<std::unique_ptr<FiltroAudio>[]>(capacidade); //funciona como se fosse o new
}

//destrutor
CadeiaFiltros::~CadeiaFiltros(){
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

    //auto para unique ptr
    auto novo_array = std::make_unique<std::unique_ptr<FiltroAudio>[]>(nova_capacidade);
    for(uint32_t i=0;i<capacidade;i++){
        novo_array[i] = std::move(filtros[i]);
    }

    filtros = std::move(novo_array);
    capacidade = nova_capacidade;

}

bool CadeiaFiltros::adicionar_filtro(std::unique_ptr<FiltroAudio> filtro){
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
    filtros[quantidade] = std::move(filtro);
    quantidade++;
    return true;
}

bool CadeiaFiltros::remover_filtro(uint32_t indice) {
    if (indice >= quantidade) //impede acesso a um indice invalido
        return false;

    //remove o filtro da cadeia (array da cadeia) e coloca em uma variável que será automaticamante destruida ao fim do escopo
    std::unique_ptr<FiltroAudio> removido = std::move(filtros[indice]);

    //desloca os filtros seguintes pra tras
    for (uint32_t i = indice; i + 1 < quantidade; i++) 
        filtros[i] = std::move(filtros[i + 1]);
    
    quantidade--;
    return true;
}

bool CadeiaFiltros::mover_filtro(uint32_t indice_atual, uint32_t indice_destino) {
    if (indice_atual >= quantidade || indice_destino >= quantidade)
        return false;
    if (indice_atual == indice_destino)
        return true;

    std::unique_ptr<FiltroAudio> movido = std::move(filtros[indice_atual]);
    if (indice_atual < indice_destino) {
        for (uint32_t i = indice_atual; i < indice_destino; i++)
            filtros[i] = std::move(filtros[i + 1]);
    } else {
        for (uint32_t i = indice_atual; i > indice_destino; i--)
            filtros[i] = std::move(filtros[i - 1]);
    }
    filtros[indice_destino] = std::move(movido);
    return true;
}


void CadeiaFiltros::limpar() {
    //um array novo (todo nulo) substitui o antigo, que é destruído junto com todos os filtros
    filtros = std::make_unique<std::unique_ptr<FiltroAudio>[]>(capacidade);
    quantidade = 0;
}

FiltroAudio *CadeiaFiltros::get_filtro(uint32_t indice) const {
    if (indice >= quantidade)
        return nullptr;
    return filtros[indice].get();
}

uint32_t CadeiaFiltros::get_quantidade() const {
    return quantidade;
}

std::string CadeiaFiltros::get_parametros() const {
    return "filtros=" + std::to_string(quantidade);
}

//analisa se o vetor da cadeia não está vazio e chama o método processar da classe do objeto no indice atual, ou seja, aplica filtros de forma encadeada
void CadeiaFiltros::processar(DadosAudio &audio){
    for(uint32_t i =0; i < quantidade; i++){
        if(filtros[i]!= nullptr){
            filtros[i]->processar(audio);
        }
    }

}


