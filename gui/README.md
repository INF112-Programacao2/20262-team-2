# Na Escuta? — Interface gráfica (Qt 6 Widgets)

Interface do projeto de **INF112 - Programação II**, implementada em **C++17** e **Qt 6 Widgets**.

> **Situação em 19/11/2026:** o executável foi compilado com sucesso no Windows (Qt 6.12 / MinGW), e a janela **Na Escuta?** foi aberta. O fluxo básico de carregamento WAV, processamento e exportação foi exercitado durante o desenvolvimento; a reprodução original/processada, o botão Parar, as formas de onda e a rolagem também foram confirmados manualmente. **Ainda faltam registros formais e testes abrangentes de todos os filtros e condições de erro.**

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
- Reproduzir áudio original e processado, com botão para interromper a reprodução (Qt Multimedia, quando disponível).
- Exibir as formas de onda original e processada, usando amostras de `DadosAudio` e desenho em Qt Widgets.
- Apresentar a identidade visual **Na Escuta?** em preto, branco e cinza, com logo PNG transparente incorporada por recursos Qt (`.qrc`), cabeçalho fixo e área de conteúdo rolável.

### Filtros mostrados na interface

| Filtro | Integração na GUI | Observação |
| --- | --- | --- |
| Ganho de volume | Código presente | Parâmetro em dB convertido para multiplicador |
| Distorção | Código presente | Intensidade e limite; fator de saída definido na ponte |
| Reforço de frequência | Código presente | Frequência central e ganho |
| Limpeza de fala | Código presente | Cortes grave e agudo |
| Eco / Delay | Indisponível | Permanece sinalizado como recurso futuro; depende de API no núcleo |

**Importante:** a compilação completa passou, mas isso não comprova a qualidade de cada filtro. Volume foi usado no fluxo básico; Distorção, Reforço de frequência, Limpeza de fala e combinações de filtros ainda precisam de testes individuais e conjuntos documentados.

## Organização de arquivos

| Arquivo | Responsabilidade |
| --- | --- |
| `main.cpp` | Iniciar `QApplication` e abrir a janela |
| `interfacegrafica.hpp` / `.cpp` | Janela Qt, botões, validações, estados e eventos |
| `catalogo_filtros.hpp` | Nomes, unidades, limites e valores padrão dos controles |
| `ponte_filtros.hpp` / `.cpp` | Adaptar os valores da tela aos construtores de filtros do núcleo |
| `CMakeLists.txt` | Configurar a compilação da GUI, Qt Multimedia opcional e ligação com o núcleo |
| `reprodutor_audio.hpp` / `.cpp` | Reprodução e parada de áudio original/processado com Qt Multimedia |
| `visualizacao_onda.hpp` | Representação gráfica de amplitude, calculada a partir das amostras |
| `identidade_visual.qrc` / `assets/logo_na_escuta.png` | Logo transparente incorporada ao executável |
| `TESTES_GUI.md` | Plano de testes manuais, critérios de aprovação e evidências |

A interface utiliza os contratos públicos de `DadosAudio`, `GerenciadorArquivoWav` e `CadeiaFiltros`, mas não implementa as regras de leitura e escrita WAV nem os algoritmos de processamento digital.

## Ferramentas

- Qt 6 com módulo **Widgets** (compilador compatível com a mesma instalação do Qt).
- Qt **Multimedia** para ouvir os áudios na própria aplicação. O projeto permite compilar sem esse módulo, mas a reprodução fica indisponível nesse caso.
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

No Windows usado no desenvolvimento, o alvo `gui` compilou com sucesso e o executável abriu. Se houver erro em uma nova versão do núcleo, ele ainda poderá impedir a geração de `gui.exe`.

### Verificar somente a compilação da interface

O alvo `gui_verificacao` permite compilar
os arquivos da GUI separadamente, com o compilador e as bibliotecas Qt6
configurados pelo CMake:

```bash
cmake --build build-gui --target gui_verificacao -- -k 0
```

Esse alvo verifica `main.cpp`, `interfacegrafica.cpp`, `ponte_filtros.cpp` e a
geração Qt MOC. **Não compila `src/`, não vincula o programa e não executa
testes de processamento ou exportação**. Um resultado sem erros confirma
somente a compilação isolada das unidades da GUI, não o funcionamento final.

Para compilar e testar o programa completo, utilize o alvo normal:

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

O roteiro de casos de teste está em [`TESTES_GUI.md`](TESTES_GUI.md). Registre os resultados reais, principalmente em carregamento, aplicação de cada filtro, reprodução, formas de onda, estados da interface e exportação/reabertura de WAV. Os testes exploratórios relatados durante o desenvolvimento não substituem o preenchimento de resultados e evidências no roteiro.

### Escopo da branch

A branch `interface-qt` e responsavel apenas pelos arquivos da interface. Problemas encontrados no núcleo devem ser comunicados a quem mantem `src/` e `include/`, sem alterações nesses diretorios.

## Identidade visual

A interface se chama **Na Escuta?**. Seu cabeçalho fixo apresenta o logo à esquerda
e os controles usam somente preto, branco e cinza, seguindo a arte da logo.
Eles estão agrupados pela sequência de uso: entrada, reprodução,
filtros, cadeia, processamento/exportação e formas de onda. O formulário possui
rolagem vertical; a barra de status permanece visível.

A arte original está em `assets/logo_na_escuta.png` e é incorporada ao executável
por `identidade_visual.qrc` (Qt Resource System), sem depender de caminhos
externos ao distribuir o programa.
