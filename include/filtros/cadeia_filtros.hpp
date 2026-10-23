#ifndef CADEIA_FILTROS_HPP
#define CADEIA_FILTROS_HPP
#include"filtro_audio.hpp"

class CadeiaFiltros : public FiltroAudio { 
    private:
        
        static const uint32_t capacidade_max_filtros = 64; //limite de segurança
        
        FiltroAudio **filtros; //Ponteiro para array de ponteiros (FiltroAudio*[])
        uint32_t capacidade; 
        uint32_t quantidade;

        void redimensionar(); //aumenta a capacidade da cadeia

    public:
        //construtor
        explicit CadeiaFiltros(uint32_t capacidade_inicial = 16); //inicializa a cadeia já com capacidade definida
        //destrutor
        ~CadeiaFiltros() override; 

        //função pra adicionar um filtro na cadeia
        bool adicionar_filtro(FiltroAudio *filtro); //retorna bool para verificar se adicionou

        void processar(DadosAudio &audio) override;//precisa colocar o processara aqui porque a cadeia vai acionar o processar de cada classe de filtro uma atras da outra
                                          //isso vai permitir correr menos risco na hora de modificar metadados e aumentar a eficiencia

};

#endif