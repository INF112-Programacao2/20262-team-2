# Interface gráfica: situação e contrato de integração

Este documento descreve o que a interface Qt (pasta `gui/`) já faz e como ela vai se ligar às classes do grupo. As assinaturas abaixo foram copiadas dos cabeçalhos da `main`. Os pontos marcados como **a combinar** ainda dependem de decisão do grupo.

## 1. Situação atual

A interface compila e roda sozinha. Ela valida arquivos e caminhos, monta a cadeia de filtros e controla o estado dos botões, mas **ainda não chama as classes do grupo**: cada ponto de ligação está marcado com `TODO` em `gui/interfacegrafica.cpp`. Como compilar: [`gui/README.md`](../gui/README.md).

## 2. Pontos de integração

| Ação na tela | Função | O que será chamado |
|--------------|--------|--------------------|
| Carregar | `InterfaceGrafica::carregar()` | `GerenciadorArquivoWav::carregar` lê o `.wav` e devolve um `DadosAudio` |
| Processar | `InterfaceGrafica::aoProcessar()` | Monta uma `CadeiaFiltros` com a lista da tela e chama `processar` sobre o `DadosAudio` |
| Exportar | `InterfaceGrafica::exportar()` | `GerenciadorArquivoWav::exportar` grava o `DadosAudio` processado |

Antes de chamar as classes, a interface já trata: caminho vazio, arquivo inexistente, caminho que é pasta, extensão diferente de `.wav`, pasta de destino inexistente e confirmação de sobrescrita. O restante (cabeçalho inválido, arquivo corrompido, erro de gravação) deve vir das classes do grupo.

## 3. Contrato (conforme os cabeçalhos da `main`)

### 3.1 `GerenciadorArquivoWav` (`include/gerenciador_arquivo_WAV.hpp`)

```cpp
static DadosAudio* carregar(const std::string &caminho);
static void exportar(const std::string &caminho, const DadosAudio &dados);
```

- Os dois métodos são estáticos, e **por enquanto só existe o `.hpp`** (falta a implementação).
- `carregar` devolve um ponteiro cru. A interface assume que **quem recebe é o dono** e precisa liberá-lo (`delete`). Ela vai guardá-lo em um `std::unique_ptr<DadosAudio>`.
- **A combinar:** como os erros são sinalizados. O cabeçalho não diz. A proposta é lançar exceção cuja `what()` seja uma mensagem em português legível para o usuário (por exemplo, "Arquivo não é um WAV válido: marcador RIFF ausente"), que a interface mostra em um pop-up.
- A interface converte o caminho de `QString` para `std::string`. No Windows, caminhos com acentos exigem cuidado com a codificação, e esse ponto será tratado na integração.

### 3.2 `DadosAudio` (`include/dados_audio.hpp`)

```cpp
DadosAudio(const CabecalhoWav &cabecalho, int16_t *audio, uint32_t num_amostras);
int16_t* get_audio();
uint32_t get_num_amostras() const;
CabecalhoWav& get_cabecalho();
```

- As amostras são `int16_t`, intercaladas quando o arquivo é estéreo.
- O construtor assume a posse do vetor (o destrutor faz `delete[]`).
- Como o processamento altera as amostras **no lugar**, para comparar antes e depois a interface precisará guardar uma cópia das amostras originais.

### 3.3 `CadeiaFiltros` (`include/filtros/cadeia_filtros.hpp`)

```cpp
explicit CadeiaFiltros(uint32_t capacidade_inicial = 16);
bool adicionar_filtro(FiltroAudio *filtro);
void processar(DadosAudio &audio) override;
```

- A cadeia aceita no máximo **64 filtros**, e `adicionar_filtro` devolve `false` ao passar disso. A interface vai impedir o 65º filtro, com aviso.
- A cadeia **não é dona dos filtros**: o destrutor libera só o vetor interno. Quem cria os filtros precisa mantê-los vivos durante o `processar` e liberá-los depois. A interface vai guardá-los em `std::vector<std::unique_ptr<FiltroAudio>>`.
- A **ordem** é decidida na tela (adicionar, remover, subir e descer acontecem na lista). A cada **Processar**, a interface cria uma `CadeiaFiltros` nova e adiciona os filtros na ordem da lista.

### 3.4 Filtros

| Filtro na tela | Classe no núcleo | Situação | Observação |
|----------------|------------------|----------|------------|
| Ganho de volume | `FiltroVolume(double porcentagem)` | implementado | 100 = sem mudança. A tela usa dB: `porcentagem = 100 * 10^(dB/20)` |
| Limpeza de fala | `FiltroLimpaRuido()` | implementado | O construtor não recebe parâmetros, então o slider "Atenuação fora da fala" ainda não teria efeito |
| Distorção | | não existe | |
| Eco / Delay | | não existe | |
| Reforço de frequência (perda auditiva) | | não existe | |

Cada item da cadeia na tela guarda `[índice do filtro no catálogo, valor1, valor2]` (todos inteiros, vindos de sliders). As faixas dos sliders ficam em `gui/catalogo_filtros.hpp` e podem ser ajustadas conforme a implementação de vocês.

### 3.5 `GerenciadorBancoDados`

A tela de perfis e presets (História 9) ainda não existe. Quando for feita, este documento ganha a proposta de métodos.

## 4. Tratamento de erros

A interface chama as classes do grupo dentro de `try/catch` e mostra `e.what()` em um `QMessageBox::critical`. Por isso é importante que as exceções tenham mensagens claras e que a memória seja liberada mesmo quando houver exceção (História 3).

## 5. Pontos a alinhar nos arquivos atuais

Observações sobre o código que já está na `main`, para o grupo avaliar:

- `FiltroAudio::processar` é `virtual` comum. Para a classe ser realmente abstrata, como no cartão CRC, o método precisa ser puro (`= 0`).
- `DadosAudio` assume a posse do vetor, mas não impede a cópia do objeto. Copiar causaria `delete[]` duplo. Vale apagar o construtor de cópia e a atribuição.
- `FiltroLimpaRuido` percorre as amostras como um único canal. Em áudio estéreo (como o `data/teste.wav`), as amostras da esquerda e da direita ficam intercaladas, então o estado do filtro é compartilhado entre os dois canais. Os cortes também são fixos e não estão em Hz.
- A forma de sinalizar erros (exceções) ainda não está definida (ver 3.1).
