# Interface gráfica - Processador Digital de Áudio

Interface do projeto de **INF112 - Programação II**, implementada em **C++17** e **Qt 6 Widgets**.

> **Situação de desenvolvimento:** a GUI possui código para carregar arquivos WAV, montar e aplicar uma cadeia de filtros e exportar áudio processado. **A execução do fluxo completo ainda não foi validada.** Na revisão de 18/11/2026, a compilação parou em um erro do núcleo (`src/gerenciador_arquivo_WAV.cpp`: chamada a `e_valido()` que não corresponde a `e_valida(const char*)`). Essa pendência pertence aos responsáveis pelo núcleo; a GUI não deve alterar `src/` nem `include/` para contorná-la.

## Recursos implementados na GUI

- Selecionar arquivo `.wav` de entrada, validar caminho e solicitar carregamento pela API `GerenciadorArquivoWav::carregar`.
- Guardar o áudio em memória com `std::unique_ptr<DadosAudio>`, sem copiar os dados de áudio.
- Exibir informações do WAV carregado: canais, taxa de amostragem, bits por amostra, quantidade de amostras e duração estimada.
- Escolher filtros e ajustar parâmetros na tela, incluindo edição de filtros que já estão na cadeia (botão ou duplo clique).
- Adicionar, remover, reordenar e limpar a cadeia; preservar a ordem visual de processamento.
- Montar `CadeiaFiltros` por meio de `ponte_filtros.cpp` e solicitar processamento com `processar`.
- Exportar pela API `GerenciadorArquivoWav::exportar`, verificando o caminho e pedindo confirmação antes de sobrescrever.
- Invalidar o resultado anterior quando a entrada ou a configuração da cadeia mudar, evitando exportar dados desatualizados.
- Exibir mensagens de estado e erros com recursos do Qt (`QMessageBox` e barra de status).

### Filtros mostrados na interface

| Filtro | Integração na GUI | Observação |
| --- | --- | --- |
| Ganho de volume | Código presente | Parâmetro em dB convertido para multiplicador |
| Distorção | Código presente | Intensidade e limite; fator de saída definido na ponte |
| Reforço de frequência | Código presente | Frequência central e ganho |
| Limpeza de fala | Código presente | Cortes grave e agudo |
| Eco / Delay | Indisponível | Permanece sinalizado como recurso futuro; depende de API no núcleo |

**Importante:** integração no código não equivale a funcionamento comprovado. Os filtros precisam ser testados com arquivos de áudio depois que a compilação do núcleo for corrigida.

## Organização de arquivos

| Arquivo | Responsabilidade |
| --- | --- |
| `main.cpp` | Iniciar `QApplication` e abrir a janela |
| `interfacegrafica.hpp` / `.cpp` | Janela Qt, botões, validações, estados e eventos |
| `catalogo_filtros.hpp` | Nomes, unidades, limites e valores padrão dos controles |
| `ponte_filtros.hpp` / `.cpp` | Adaptar os valores da tela aos construtores de filtros do núcleo |
| `CMakeLists.txt` | Configurar a compilação da GUI e sua ligação com o núcleo |
| `TESTES_GUI.md` | Roteiro manual e registro de testes pendentes |

A interface utiliza os contratos públicos de `DadosAudio`, `GerenciadorArquivoWav` e `CadeiaFiltros`, mas não implementa as regras de leitura e escrita WAV nem os algoritmos de processamento digital.

## Ferramentas

- Qt 6 com módulo **Widgets** (compilador compatível com a mesma instalação do Qt).
- CMake 3.16 ou superior.
- Ninja.
- Compilador com suporte a C++17.

### Windows (Git Bash, a partir da raiz do repositório)

Ajuste as versões conforme a sua instalação; os caminhos abaixo correspondem ao ambiente utilizado no desenvolvimento:

```bash
export PATH="/c/Qt/Tools/CMake_64/bin:/c/Qt/Tools/Ninja:/c/Qt/Tools/mingw1310_64/bin:/c/Qt/6.12.0/mingw_64/bin:$PATH"

cmake -S gui -B build-gui -G Ninja -DCMAKE_PREFIX_PATH=C:/Qt/6.12.0/mingw_64
cmake --build build-gui
./build-gui/gui.exe
```

O executável somente estará disponível se a compilação for concluída com sucesso. Um erro no núcleo pode impedir a geração de `gui.exe`.

### Verificar somente a compilação da interface

Se o núcleo ainda apresentar erros, o alvo `gui_verificacao` permite compilar
os arquivos da GUI separadamente, com o compilador e as bibliotecas Qt6
configurados pelo CMake:

```bash
cmake --build build-gui --target gui_verificacao -- -k 0
```

Esse alvo verifica `main.cpp`, `interfacegrafica.cpp`, `ponte_filtros.cpp` e a
geração Qt MOC. **Não compila `src/`, não vincula o programa e não executa
testes de processamento ou exportação**. Um resultado sem erros confirma
somente a compilação isolada das unidades da GUI, não o funcionamento final.

Para testar o fluxo completo após a correção do núcleo, utilize o alvo normal:

```bash
cmake --build build-gui --target gui
./build-gui/gui.exe
```

### Linux (Ubuntu / Debian)

```bash
sudo apt install qt6-base-dev cmake ninja-build g++
cmake -S gui -B build-gui -G Ninja
cmake --build build-gui
./build-gui/gui
```

Os comandos de Linux são orientações de configuração e **não representam um teste bem-sucedido desta versão**.

## Como validar

O roteiro de casos de teste está em [`TESTES_GUI.md`](TESTES_GUI.md). Registre os resultados reais, principalmente em carregamento, aplicação de cada filtro e exportação/reabertura de WAV.

### Escopo da branch

A branch `interface-qt` e responsavel apenas pelos arquivos da interface. Problemas encontrados no núcleo devem ser comunicados a quem mantem `src/` e `include/`, sem alterações nesses diretorios.
