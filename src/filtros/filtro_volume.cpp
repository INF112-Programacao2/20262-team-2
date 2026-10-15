#include "filtro_volume.hpp"
#include <cstdint>

//construtor
FiltroVolume :: FiltroVolume (double porcentagem) 
: multiplicador(porcentagem/100.0) {}



void FiltroVolume :: processar(DadosAudio &amostras) {

}