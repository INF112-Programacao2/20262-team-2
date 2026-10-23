#include "filtro_audio.hpp"

//destrutor
FiltroAudio::~FiltroAudio() {
}

//método processar base que será sobrescrito por cada filtro 
void FiltroAudio::processar(DadosAudio &audio){}
