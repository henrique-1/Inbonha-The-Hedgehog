# 🦔 Inbonha The Hedgehog

Um jogo de plataforma 2D em C, inspirado no clássico _Sonic the Hedgehog_, desenvolvido com a biblioteca [Raylib](https://www.raylib.com/). Este projeto foi construído focando tanto na recriação de mecânicas nostálgicas quanto na aplicação de boas práticas de engenharia de software e código limpo.

## 🎯 Objetivos e Entregas do Projeto

O projeto segue um cronograma estrito de entregas, expandindo iterativamente as funcionalidades do motor de jogo base. Abaixo está o status das implementações exigidas:

### 🏁 Roadmap e Status

**Fase 1: Interface Clássica (Entrega: 28/05)**

- [x] **HUD Fiel ao Original:** Sistema de exibição de Anéis, Pontuação, Tempo e Vidas, consultado e renderizado de forma idêntica ao jogo original.

**Fase 2: Expansão de Conteúdo (Entrega: 11/06)**

- [ ] **Inimigos (4 no total):** Implementação de IA e padrões de movimento para inimigos terrestres (como o Motobug e Spikes) e voadores.
- [ ] **Itens (4 no total):** Sistema modular de coleta. Além dos clássicos anéis dourados e azuis, inclui powerups como Escudo e Estrelinha de invencibilidade.
- [ ] **Múltiplos Níveis:** Duas fases completas.
  - _Fase 1:_ Baseada na clássica Green Hill, com terrenos e obstáculos familiares.
  - _Fase 2:_ Novo bioma, com conjunto de _tiles_ (terreno), plano de fundo e trilha sonora exclusivos.

**Fase 3: Funcionalidades Extras e Polimento (Entrega: 18/06 e Apresentação: 25/06)**

- [ ] **Mecânica de Spin/Dash:** - Ao andar e pressionar para baixo, o personagem entra em estado de rolamento (_spin_).
  - Parado, ao segurar para baixo e pressionar o pulo, o personagem carrega o _spin dash_.
- [ ] **Fluxo de Telas:** Implementação de máquina de estados para transições fluídas entre Tela Inicial, Tela de Transição (com contagem de pontos) e Tela de Continue.

## 🏗️ Arquitetura e Qualidade de Código

O desenvolvimento do Inbonha The Hedgehog prezou por uma arquitetura de software escalável, garantindo um código em C legível, seguro e de fácil manutenção:

- **Separação de Responsabilidades (SoC):** A lógica do jogo está fortemente modularizada. Entidades como `Jogador`, `Inimigo`, `Item`, `Mapa` e `GameWorld` possuem domínios isolados, com seus próprios arquivos de cabeçalho (`.h`) e implementação (`.c`).
- **Gerenciamento Eficiente de Memória (`ResourceManager`):** Sistema centralizado para carregar, armazenar em cache e liberar texturas, efeitos sonoros (`.wav`) e músicas (`.mp3`). Isso previne vazamentos de memória (_memory leaks_) e elimina o recarregamento redundante de _assets_ do disco.
- **Estruturação de Dados:** Uso de estruturas de dados (Structs) e passagem de ponteiros otimizada para manipular o estado global do jogo e da janela (`GameWindow`) sem comprometer a performance.
- **Build Automatizado:** Scripts de compilação cross-platform prontos para uso, facilitando a montagem e a integração do projeto.

## 🚀 Como Compilar e Executar

O projeto já inclui a biblioteca pré-compilada da Raylib (`lib/libraylib.a`) e scripts de automação para facilitar o processo.

### Pré-requisitos

- Compilador C (GCC ou Clang).
- `make` (opcional, mas recomendado para ambientes Unix).

### Compilando

**No Windows:**
Você pode utilizar os scripts PowerShell ou Batch fornecidos na raiz:

```cmd
build.bat
:: ou
.\build.ps1
```
