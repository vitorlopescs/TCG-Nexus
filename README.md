# TCG Nexus

Sistema de catálogo e integração para lojas de Trading Card Games (Pokémon, Magic, entre outros). Permite que desenvolvedoras de jogos disponibilizem metadados oficiais das cartas, que lojistas gerenciem estoque e preços, e que compradores naveguem por uma vitrine com filtros técnicos.

Projeto desenvolvido para a disciplina de **Engenharia de Software I (BCC322)**.

## Visão geral

O TCG Nexus é dividido em três módulos principais, mapeados na arquitetura MVC do sistema:

| Módulo | Perfil de acesso | Responsabilidade |
|---|---|---|
| **DevPortal** | Admin | Ingestão de metadados JSON/CSV oficiais das desenvolvedoras; cadastro de usuários |
| **StoreDash** | Lojista | Gerenciamento de estoque e preços das cartas |
| **Market** | Comprador | Vitrine com filtros técnicos de busca (planejado para sprints futuras) |

## Arquitetura

- **Padrão**: MVC
- **Linguagem**: C++
- **Interface gráfica**: Qt
- **Persistência**: SQLite (local)
- **Testes**: QTest
- **Documentação**: Doxygen

### Estrutura de pastas

```
TCGNexus/
├── TCGNexus.pro                  # Abrir este arquivo no Qt Creator
├── Doxyfile                      # Configuração de geração de documentação
├── TCGCore/                      # DLL com regras de negócio (Singleton)
│   ├── TCGCore.pro
│   ├── tcgcore_global.h
│   ├── nexusdbmanager.h
│   └── nexusdbmanager.cpp
├── TCGGUI/                       # Executável (interface gráfica)
│   ├── TCGGUI.pro
│   ├── main.cpp
│   ├── logindialog.h
│   ├── devportalwindow.h         # Perfil ADMIN
│   └── storedashwindow.h         # Perfil LOJISTA
└── TCGTests/                     # Testes automatizados (QTest)
    ├── TCGTests.pro
    └── test_storedash.cpp

## Como executar

### Pré-requisitos

- Qt Creator (ou Qt + qmake instalado)
- Compilador C++ compatível (MinGW, MSVC ou GCC)

### Passos

1. Clone o repositório:
   ```bash
   git clone <url-do-repositorio>
   cd TCGNexus
   ```
2. Abra `TCGNexus.pro` no Qt Creator.
3. Compile e execute o projeto (`Ctrl+R`).

### Login de teste

| Usuário | Senha | Perfil | Acessa |
|---|---|---|---|
| `admin` | `123` | Admin | DevPortal |

Pelo DevPortal é possível importar o JSON oficial de cartas e cadastrar um usuário Lojista (ex: `vendedor` / `123`), que dará acesso ao StoreDash.

## Testes

Os testes automatizados usam o framework QTest e cobrem os cenários de busca por atributo e adição de cartas ao estoque.

Para rodar:
```bash
cd TCGTests
qmake && make
./TCGTests
```

## Documentação técnica

A documentação do código é gerada via Doxygen a partir dos comentários no código-fonte.

```bash
doxygen Doxyfile
```

O HTML gerado fica disponível na pasta `docs/html` (ou conforme configurado no `Doxyfile`).
