# Plano de testes manuais - Interface Qt6

Projeto: **INF112 - Processador Digital de Áudio**
Escopo: **somente `gui/`**
Registro formal: **EM ANDAMENTO - casos não concluídos nem evidenciados neste documento**

Este roteiro define verificações e resultados esperados. Não é um relatório de aprovação: marque cada caso apenas depois de executar a aplicação e observar o comportamento.

## Pré-condições

1. Compilação do programa concluída (inclusive biblioteca do núcleo).
2. Qt 6 Widgets disponível no ambiente.
3. Arquivos WAV de teste preservados fora da pasta de saída: mono e estéreo, WAV curto e WAV mais longo; incluir exemplos inválidos para testar erros.
4. Destino de exportação em uma pasta com permissão de escrita e arquivo original preservado.
5. Ao usar filtros, aplicar volumes moderados; evitar reprodução em nível elevado.

**Histórico de integração:** a incompatibilidade antiga de `e_valido()` foi corrigida pelos responsáveis do núcleo e incorporada pela branch `interface-qt`. A GUI compilou e abriu no Windows. Uma nova falha em `src/` ou `include/` continua sendo responsabilidade dos mantenedores do núcleo.

## Casos de teste

| ID | Passos | Resultado esperado | Status |
| --- | --- | --- | --- |
| GUI-01 | Abrir a janela sem carregar arquivo | Informação de áudio vazia; Processar e Exportar desabilitados | [ ] Pendente |
| GUI-02 | Clicar em Carregar com entrada vazia | Aviso; nenhum áudio considerado carregado | [ ] Pendente |
| GUI-03 | Selecionar caminho inexistente ou pasta em vez de WAV | Erro de validação e interface consistente | [ ] Pendente |
| GUI-04 | Selecionar arquivo com extensão diferente de `.wav` | Aviso de formato | [ ] Pendente |
| GUI-05 | Carregar WAV mono válido | Metadados coerentes; estado carregado | [ ] Pendente |
| GUI-06 | Carregar WAV estéreo válido | Número de canais, amostras e duração coerentes | [ ] Pendente |
| GUI-07 | Selecionar Eco/Delay e tentar adicionar | Opção identificada como indisponivel; não entra na cadeia | [ ] Pendente |
| GUI-08 | Adicionar Volume, Distorção, Realce e Limpeza de Fala | Cada item aparece com parâmetros e ordem corretos | [ ] Pendente |
| GUI-09 | Editar parâmetros de um item da cadeia e cancelar | Valores anteriores permanecem | [ ] Pendente |
| GUI-10 | Editar parâmetros e confirmar | Descrição do item atualizada com os valores salvos | [ ] Pendente |
| GUI-11 | Reordenar, remover e limpar filtros | Ordem, seleção e botões da cadeia permanecem coerentes | [ ] Pendente |
| GUI-12 | Processar WAV válido com filtro de Volume | Sem exceção; estado de áudio processado; Exportar habilitado | [ ] Pendente |
| GUI-13 | Testar individualmente Distorção, Realce e Limpeza de Fala | Cada filtro e executado, sem ser ignorado pela ponte | [ ] Pendente |
| GUI-14 | Mudar entrada ou cadeia depois do processamento | Resultado anterior invalidado; exportação bloqueada até novo processamento | [ ] Pendente |
| GUI-15 | Exportar WAV processado para nome novo | Arquivo criado; reabertura por leitor WAV externo e/ou pelo próprio programa | [ ] Pendente |
| GUI-16 | Exportar para arquivo existente e selecionar Não | Operação cancelada; arquivo original inalterado | [ ] Pendente |
| GUI-17 | Tentar processar áudio inválido ou filtro que falhe | Mensagem de erro; nenhum resultado parcial pode ser exportado | [ ] Pendente |
| GUI-18 | Carregar caminho com acentos ou caracteres não ASCII no Windows | Verificar compatibilidade; registrar problema se não funcionar | [ ] Pendente |
| GUI-19 | Carregar WAV válido e observar forma de onda original | Gráfico visível e coerente com as amostras; sem travamento | [ ] Pendente |
| GUI-20 | Processar áudio e observar forma de onda processada | Gráfico atualizado; visualização anterior preservada para comparação | [ ] Pendente |
| GUI-21 | Ouvir original, ouvir processado e acionar Parar | Reprodução audível, alternância e interrupção sem erros | [ ] Pendente |
| GUI-22 | Alterar entrada ou filtros durante/após reprodução | Áudio anterior interrompido/invalido; não reproduzir dados desatualizados | [ ] Pendente |
| GUI-23 | Usar rolagem e reduzir janela para tamanho menor | Cabeçalho, botões e seções permanecem acessíveis sem sobreposição | [ ] Pendente |
| GUI-24 | Abrir aplicação com Qt Multimedia indisponível | Interface abre, recursos de reprodução indicam indisponibilidade | [ ] Pendente |
| GUI-25 | Exportar WAV e reabrir o arquivo salvo | Arquivo válido, metadados coerentes, conteúdo reproduzível | [ ] Pendente |
| GUI-26 | Exportar para arquivo existente e confirmar Sim | Substituição ocorre apenas após consentimento; sem dano ao original de teste | [ ] Pendente |

## Validação exploratória já relatada (19/11/2026)

No acompanhamento do desenvolvimento, houve relatos positivos de **compilação da GUI, abertura da janela, carregamento WAV, processamento básico, reprodução original/processada, botão Parar e visualização das ondas**. A interface monocromática com logo transparente e rolagem também foi conferida por capturas de tela. Esses relatos **não aprovam automaticamente os casos da tabela**: ainda é necessário registrar arquivos usados, passos e evidências para cada caso.

## Registrar evidências

Para cada teste executado, registrar **data, versão/commit, ambiente, arquivo WAV usado, resultado observado e evidência** (captura ou mensagem de erro).

```text
Caso: GUI-__
Commit:
Sistema / Qt / compilador:
Arquivo de entrada:
Passos realizados:
Resultado esperado:
Resultado observado:
Status: Aprovado / Reprovado / Bloqueado
Evidencia:
```

## Critérios de conclusão da interface

- Os testes de estado, filtros, processamento e exportação foram executados sem falhas relevantes.
- Os WAV exportados podem ser reabertos e possuem metadados coerentes.
- Reprodução original/processada, interrupção, gráficos e estados dos controles foram verificados.
- Os testes com filtros individuais, múltiplos filtros e cenários de erro possuem resultado e evidência.
- Nenhuma funcionalidade da interface apresenta sucesso ficticio quando a operação falha.
- Pendencias do núcleo e pendencias da GUI estão registradas separadamente.
- Nenhum artefato de compilação (`build-gui/`) e incluído no commit.
