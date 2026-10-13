# Interface gráfica (Qt Widgets)

Interface do **Processador Digital de Áudio**, feita em C++17 com **Qt 6 Widgets**.

> **Estado atual:** a tela está pronta e funciona sozinha, mas ainda **não lê, não processa e não grava áudio**. Esses pontos estão marcados com `TODO` no código e aguardam as classes do grupo (`GerenciadorArquivoWav`, `DadosAudio`, `CadeiaFiltros` e os filtros). O que a interface espera de cada uma está em [`docs/interface-gui.md`](../docs/interface-gui.md).

## O que já funciona

- Escolha dos arquivos de entrada e saída (`.wav`), com validação de caminho: vazio, inexistente, pasta, extensão errada, pasta de destino inexistente e confirmação antes de sobrescrever (inclusive quando a saída é o próprio arquivo de entrada).
- Menu de filtros com sliders de parâmetros (ganho, distorção, eco/delay, reforço de frequência e limpeza de fala).
- Cadeia de filtros: adicionar, remover, subir, descer e limpar.
- Botão **Processar**, que só habilita com arquivo carregado e ao menos um filtro na cadeia.
- Mensagens de erro e aviso em pop-ups e status na barra inferior.

## Requisitos

- **Qt 6** (módulo Widgets). Testado com Qt 6.12 (Windows, MinGW 64-bit) e compila com Qt 6.4 (Ubuntu).
- **CMake** 3.16 ou superior
- **Ninja**
- Um compilador C++17 (MinGW no Windows, g++ no Linux)

### Windows

1. Instale o Qt pelo *Qt Online Installer* (qt.io). Em *Custom installation*, marque **Qt 6.x → MinGW 64-bit** e, em *Developer and Designer Tools*, **MinGW**, **CMake** e **Ninja**.
2. No Git Bash, deixe as ferramentas no `PATH` (ajuste as versões para as que você instalou):

```bash
export PATH="/c/Qt/Tools/CMake_64/bin:/c/Qt/Tools/Ninja:/c/Qt/Tools/mingw1310_64/bin:/c/Qt/6.12.0/mingw_64/bin:$PATH"
```

### Linux (Ubuntu/Debian)

```bash
sudo apt install qt6-base-dev cmake ninja-build g++
```

## Compilar e executar

Dentro da pasta `gui/`:

```bash
# Windows (informe a pasta do Qt)
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=C:/Qt/6.12.0/mingw_64
cmake --build build
./build/gui.exe

# Linux
cmake -S . -B build -G Ninja
cmake --build build
./build/gui
```

Depois de alterar só código, basta repetir `cmake --build build`. Quando um arquivo novo é adicionado ao `CMakeLists.txt`, rode o primeiro comando de novo.

## Arquivos

| Arquivo | Função |
|---------|--------|
| `main.cpp` | Cria a aplicação Qt e a janela principal |
| `interfacegrafica.hpp` / `.cpp` | Classe `InterfaceGrafica` (a janela e toda a sua lógica) |
| `catalogo_filtros.hpp` | Descrição dos filtros exibidos na tela (nome, parâmetros, faixas e unidades). Para um filtro novo aparecer na interface, é só acrescentar uma linha |
| `CMakeLists.txt` | Configuração do build |

## Onde ligar as classes do grupo

Procure por `TODO` em `interfacegrafica.cpp`:

- `carregar()`: ler o arquivo com o `GerenciadorArquivoWav`.
- `exportar()`: gravar o `DadosAudio` processado.
- `aoProcessar()`: montar a `CadeiaFiltros` a partir da lista e executá-la.
