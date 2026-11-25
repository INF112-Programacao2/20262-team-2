# Na Escuta? — melhorias da interface (INF112)

## Escopo

Este pacote altera apenas arquivos de `gui/`. Não muda algoritmos, cabeçalhos ou implementação dos filtros do núcleo.

A implementação usa conceitos das aulas de Programação II: classes e encapsulamento, composição com widgets Qt, métodos e eventos, arquivos, exceções e gerenciamento de memória com `std::unique_ptr`. A atividade permanece em C++17 e Qt6; não foram introduzidas threads ou bibliotecas extras.

## Roteiro de validação no Windows

Execute o programa com Qt Multimedia disponível. Teste também a compilação sem Qt Multimedia, se necessário; nessa configuração a reprodução fica desativada, mas os demais recursos continuam acessíveis.

1. **Reprocessar sem selecionar o WAV de novo:** carregue um WAV estéreo, aplique Volume, processe, altere a intensidade, processe outra vez; confira o novo resultado, o áudio original disponível e a exportação somente do último resultado.
2. **Validação de canais:** carregue um WAV mono, escolha Ganho por Canal e tente adicionar. A GUI deve alertar que exige estéreo. Se a cadeia já continha Ganho por Canal, tentar processar o WAV mono também deve apresentar aviso.
3. **Indicação de processamento:** ao clicar Processar, deve aparecer a janela "Processamento em andamento" e a mensagem de estado, e os controles devem ficar indisponíveis até acabar. Como o processamento é síncrono, essa janela não mede percentual e pode ficar sem animação durante tarefas demoradas.
4. **Destino sugerido:** ao carregar `exemplo.wav`, confira a sugestão `exemplo_processado.wav` na mesma pasta; ao digitar destino próprio, confira que a próxima carga não substitui sua escolha automaticamente.
5. **Player:** ouça o original, observe `MM:SS / MM:SS`, pause no meio e confirme que áudio e tempo param; clique **Continuar áudio** e verifique que retoma do mesmo ponto (sem reiniciar). Repita com áudio processado. Arraste a barra enquanto está pausado; ela deve continuar pausada na nova posição. Verifique que **Parar áudio** encerra a reprodução e que o áudio pode ser ouvido novamente.
6. **Arrastar e soltar:** solte um arquivo `.wav` do Explorador sobre a janela; ele deve ser carregado. Arquivos de outras extensões não devem ser aceitos.
7. **Atalhos:** `Ctrl+O` escolhe a entrada; `Ctrl+Enter` processa; `Ctrl+Shift+S` exporta (se houver resultado).
8. **Recentes:** carregue WAVs diferentes, abra o botão **Recentes** e selecione um deles. Confirme que a lista mantém até cinco caminhos, inclusive ao fechar e reabrir a aplicação.
9. **Regressão:** teste adicionar/remover/reordenar/editar filtros, exportar WAV, confirmar sobrescrita e reproduzir original/processado após mudar a cadeia.
10. **Exceção:** remova ou renomeie o arquivo de entrada no disco depois de carregá-lo e tente reprocessar. A GUI deve informar a falha sem descartar o áudio original que já estava em memória.

11. **Sequência da tela:** confira que os grupos aparecem na ordem 01 Arquivo de entrada, 02 Ouvir e comparar, 03 Configurar filtros, 04 Cadeia de filtros, 05 Processar e exportar, 06 Ondas do áudio.

## Notas

- Reprocessar não pede novo arquivo, mas **relê internamente** o mesmo WAV do disco para criar uma instância separada. O arquivo original deve continuar acessível e inalterado.
- A barra de reprodução faz busca por aproximação ao quadro PCM; o tempo exibido depende do dispositivo de áudio.
- O histórico de arquivos recentes usa `QSettings` no perfil do usuário; não é salvo no repositório.
- A validação aqui descrita é um roteiro; é necessário executá-lo no Windows com Qt6. O patch, por si só, não prova que os testes funcionais passaram.
