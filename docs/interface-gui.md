# Interface gráfica: situação e contrato de integração

Este documento descreve o que a interface Qt (pasta `gui/`) já faz e o que ela **espera** das classes do grupo. Os métodos marcados como **proposta** ainda precisam ser combinados com quem implementa cada classe. Se algum nome ou assinatura mudar, a interface se adapta, e o importante é decidirmos juntos.

## 1. Situação atual

A interface é independente do restante do projeto: compila e roda sozinha. Ela valida arquivos e caminhos, monta a cadeia de filtros e controla o estado dos botões. **Não** lê, processa nem grava áudio, e cada ponto de ligação está marcado com `TODO` em `gui/interfacegrafica.cpp`. Como compilar: [`gui/README.md`](../gui/README.md).

## 2. Pontos de integração

| Ação na tela | Função | O que será chamado |
|--------------|--------|--------------------|
| Carregar | `InterfaceGrafica::carregar()` | `GerenciadorArquivoWav` lê o `.wav` e devolve um `DadosAudio` |
| Processar | `InterfaceGrafica::aoProcessar()` | Monta uma `CadeiaFiltros` com a lista da tela e a executa sobre o `DadosAudio` |
| Exportar | `InterfaceGrafica::exportar()` | `GerenciadorArquivoWav` grava o `DadosAudio` processado |

A interface já trata, antes de chamar as classes: caminho vazio, arquivo inexistente, caminho que é pasta, extensão diferente de `.wav`, pasta de destino inexistente e confirmação de sobrescrita. O restante (cabeçalho inválido, arquivo corrompido, erro de gravação) deve vir como **exceção** das classes do grupo.

## 3. Contrato proposto

### 3.1 `GerenciadorArquivoWav`

```cpp
// Lê o arquivo, valida o cabeçalho (RIFF/WAVE, 44 bytes) e devolve o áudio em memória.
std::unique_ptr<DadosAudio> carregar(const std::string &caminho);

// Grava o cabeçalho atualizado e as amostras do áudio no caminho indicado.
void exportar(const DadosAudio &dados, const std::string &caminho);
```

- Em caso de problema, lançar uma exceção (por exemplo, derivada de `std::runtime_error`) cuja `what()` seja uma **mensagem em português, legível para o usuário**. A interface mostra esse texto em um pop-up. Exemplos: "Arquivo não é um WAV válido (marcador RIFF ausente)", "Sem permissão para gravar em ...".
- `DadosAudio` não pode ser copiado (cópia proibida por causa do ponteiro cru), então o retorno é por `std::unique_ptr`, e a interface guarda esse ponteiro.
- A interface converte o caminho de `QString` para `std::string`. Em Windows, caminhos com acentos exigem cuidado com a codificação, e esse ponto será tratado na integração.

### 3.2 `CadeiaFiltros`

```cpp
void adicionar(std::unique_ptr<FiltroAudio> filtro);   // acrescenta ao fim da fila
void executar(DadosAudio &dados);                      // aplica na ordem, cada saída é a entrada do próximo
```

- A **ordem** da cadeia é decidida na tela (adicionar, remover, subir e descer acontecem na lista). No momento de **Processar**, a interface cria uma `CadeiaFiltros` nova, adiciona os filtros na ordem da lista e chama `executar`. Assim, a interface não depende de remover ou reordenar dentro da classe, embora o cartão CRC preveja esses métodos.
- O `DadosAudio` é alterado **no lugar**. Para reprocessar com outra cadeia, a interface carrega o arquivo de novo.

### 3.3 Filtros (`FiltroAudio`)

Cada filtro concreto herda de `FiltroAudio` e implementa `processar(DadosAudio &)`. Os parâmetros entram pelo **construtor**. Os valores abaixo são os que a tela oferece hoje (todos inteiros, vindos de sliders):

| Filtro na tela | Parâmetro 1 | Parâmetro 2 |
|----------------|-------------|-------------|
| Ganho de volume | Ganho: -20 a 20 dB (padrão 0) | |
| Distorção | Intensidade: 0 a 100 % (padrão 30) | |
| Eco / Delay | Atraso: 50 a 1000 ms (padrão 300) | Repetição: 0 a 90 % (padrão 40) |
| Reforço de frequência (perda auditiva) | Frequência central: 250 a 8000 Hz (padrão 2000) | Ganho: 0 a 24 dB (padrão 6) |
| Limpeza de fala | Atenuação fora da fala: 0 a 100 % (padrão 70) | |

Cada item da cadeia na tela guarda internamente `[índice do filtro no catálogo, valor1, valor2]`. Na integração, a interface terá uma função que transforma esse item em um filtro real, por exemplo:

```cpp
std::unique_ptr<FiltroAudio> criarFiltro(int indiceCatalogo, int valor1, int valor2);
```

Se os intervalos acima não servirem para a implementação de vocês (por exemplo, uma faixa de frequência maior), basta avisar, porque eles ficam em `gui/catalogo_filtros.hpp`.

### 3.4 `GerenciadorBancoDados`

A tela de perfis e presets (História 9) ainda não existe. Quando for feita, este documento ganha a proposta de métodos.

## 4. Tratamento de erros

A interface chama as classes do grupo dentro de `try/catch` e mostra `e.what()` em um `QMessageBox::critical`. Por isso é importante que as exceções tenham mensagens claras, e que `DadosAudio` libere a memória mesmo quando houver exceção (História 3).

## 5. Pontos a alinhar nos cabeçalhos atuais

Observações sobre os `.hpp` que já estão na `main`, para vocês avaliarem:

- `FiltroAudio::processar` está como `virtual` comum. Para a classe ser realmente abstrata, como no cartão CRC, o método precisa ser puro (`= 0`).
- `DadosAudio` comenta que guarda `floats`, mas o ponteiro é `double *_amostras`, e `get_amostra` retorna `float`. Vale alinhar o tipo.
- `CabecalhoWav` ainda não tem construtor nem métodos de acesso, e o construtor de `DadosAudio` depende dele.
