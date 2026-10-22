# ciot_c

## Versionamento e release

A versão fica em `CIOT_VER` (`include/ciot.h`), no formato `MAJOR,MINOR,PATCH,BUILD`, seguindo semver mais um build number.

A cada push em uma dessas branches, `.github/workflows/pipeline.yml` cria a tag e o release no GitHub:

| Branch | Tag |
|---|---|
| `develop` | `vX.Y.Z.B-alpha` |
| `release` | `vX.Y.Z.B-beta` |
| `main` | `vX.Y.Z.B` (stable) |

O corpo do release é o `changes.md` inteiro.

### Antes de subir commits e abrir um PR
- **Atualize o `changes.md`** com o que o PR muda. Use as seções existentes (`### Changes`, blocos em negrito por tema) e cite os arquivos afetados entre crases.
- **Confirme se a versão precisa mudar e pergunte ao usuário em caso de dúvida.** Faça o bump em um commit separado: `chore(ciot): update version to X.Y.Z.B`.
  - Durante o ciclo alpha, cada PR que vai para `develop` incrementa o **BUILD**: `0.27.0.0` → `0.27.0.1` → …
  - MAJOR, MINOR e PATCH refletem o conjunto de mudanças da versão em andamento:
    - quebra de API ou de protocolo → MAJOR;
    - funcionalidade nova compatível → MINOR;
    - só correções → PATCH.
  - Se um PR traz uma mudança maior que a já prevista para o ciclo, suba o componente correspondente e zere os da direita.
  - Depois de um release stable, o primeiro PR do novo ciclo sobe o componente adequado e zera o BUILD. Por exemplo: stable `v0.26.0.5` → `0.27.0.0`.
- **Depois de um release stable, reinicie o `changes.md`** no primeiro PR do novo ciclo, deixando só as mudanças posteriores ao stable. O pipeline não limpa o arquivo. Se ele ainda tiver as notas do ciclo anterior, o novo alpha é publicado com notas erradas.
- **Não reaproveite uma tag que já existe.** O pipeline falha se a tag já estiver criada.
- **Mantenha cada PR focado.** Não inclua alterações locais que não fazem parte do escopo; confira o `git status` antes de commitar.

## Convenções de commit
Use Conventional Commits com escopo, por exemplo:
- `feat(ciot): ...`
- `fix(ciot_eth): ...`
- `chore(ciot): update version to ...`
- `doc(changes): updated changes`

PRs vão para `develop`.
