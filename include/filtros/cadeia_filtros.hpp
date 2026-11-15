#ifndef CADEIA_FILTROS_HPP
#define CADEIA_FILTROS_HPP
#include"filtro_audio.hpp"
#include <cstdint>
#include <memory>


//cadeia de filtros: executa os filtros em sequência, a saída de um é a entrada do próximo
//a cadeia É dona dos filtros (unique_ptr): ao remover um filtro ou destruir a cadeia, eles são liberados
class CadeiaFiltros : public FiltroAudio { 
    private:
        
        static const uint32_t capacidade_max_filtros = 64; //limite de segurança
        
         //unique_ptr para um ARRAY, onde cada posição é um unique_ptr<FiltroAudio> (dono de um filtro)
        std::unique_ptr<std::unique_ptr<FiltroAudio>[]> filtros;
        uint32_t capacidade; 
        uint32_t quantidade;

        void redimensionar(); //aumenta a capacidade da cadeia

    public:
        //construtor
        explicit CadeiaFiltros(uint32_t capacidade_inicial = 16); //inicializa a cadeia já com capacidade definida
        //destrutor
        ~CadeiaFiltros() override; 

        //função pra adicionar um filtro na cadeia
        bool adicionar_filtro(std::unique_ptr<FiltroAudio> filtro); //retorna bool para verificar se adicionou

        bool remover_filtro(uint32_t indice);//nao deleta realmente o filtro, só tira da cadeia, retorna false se indice invalido

        bool mover_filtro(uint32_t posicao_atual, uint32_t posicao_destino); //move o filtro usando o indice da posicao autal na cadeia e da posicao de destino

        void limpar(); //esvazia a sequência (não deleta os filtros)

        uint32_t get_quantidade() const;

        //nullptr se o índice for inválido; útil para salvar presets e mostrar a cadeia na GUI
        FiltroAudio *get_filtro(uint32_t indice) const;

        void processar(DadosAudio &audio) override;//precisa colocar o processara aqui porque a cadeia vai acionar o processar de cada classe de filtro uma atras da outra
                                          //isso vai permitir correr menos risco na hora de modificar metadados e aumentar a eficiencia

        std::string get_tipo() const override;
        std::string get_parametros() const override; //ex.: "filtros=3"
};

#endif