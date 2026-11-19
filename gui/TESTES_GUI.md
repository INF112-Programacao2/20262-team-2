# Plano de testes manuais - Interface Qt6

Projeto: **INF112 - Processador Digital de Áudio**
Escopo: **somente `gui/`**
Estado inicial: **PENDENTE - testes não executados neste documento**

Este roteiro define verificações e resultados esperados. Não é um relatório de aprovação: marque cada caso apenas depois de executar a aplicação e observar o comportamento.

## Pré-condições

1. Compilação do programa concluída (inclusive biblioteca do núcleo).
2. Qt 6 Widgets disponível no ambiente.
3. Arquivos WAV de teste preservados fora da pasta de saída: mono e estéreo, WAV curto e WAV mais longo; incluir exemplos inválidos para testar erros.
4. Destino de exportação em uma pasta com permissão de escrita e arquivo original preservado.
5. Ao usar filtros, aplicar volumes moderados; evitar reprodução em nível elevado.

**Bloqueador atualmente conhecido:** `src/gerenciador_arquivo_WAV.cpp` chama `e_valido()` embora a classe exponha `e_valida(const char*)`. A resolução é responsabilidade do núcleo. Não editar arquivos de `src/` ou `include/` para executar estes testes.

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
- Nenhuma funcionalidade da interface apresenta sucesso ficticio quando a operação falha.
- Pendencias do núcleo e pendencias da GUI estão registradas separadamente.
- Nenhum artefato de compilação (`build-gui/`) e incluído no commit.
