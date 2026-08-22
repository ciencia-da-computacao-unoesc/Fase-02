# 💻 Arquitetura de Computadores — 2ª Fase

Este repositório centraliza os materiais teóricos, listas de exercícios, simulações de circuitos e códigos de programação desenvolvidos na disciplina de **Arquitetura de Computadores** do curso de Ciência da Computação (Unoesc Maravilha).

---

## 📖 Ementa da Disciplina

O conteúdo programático está dividido em três grandes pilares de conhecimento:

1. **Arquitetura de Computadores:** 
   - Funções Lógicas e Circuitos Digitais
   - Conjunto de Instruções (ISA)
   - Modos de Endereçamento de Memória
2. **Organização de Computadores:** 
   - Barramentos (Estruturas de interconexão)
   - Processadores (Unidade de Controle e ULA)
   - Hierarquia de Memórias (Cache, RAM, Armazenamento)
   - Dispositivos de Entrada e Saída (E/S)
   - Arquiteturas RISC vs. CISC
3. **Multiprocessamento:** 
   - Sistemas Multiprocessadores
   - Arquiteturas Paralelas e Pipeline
   - Modelos de Computação Não Convencionais

---

## 🚀 Projeto Prático: Desenvolvimento com Arduino (ATmega328P)

Como parte das atividades práticas do semestre, a turma desenvolverá um projeto integrando conceitos de baixo nível, mapeamento de memória e eletrônica digital utilizando o microcontrolador **ATmega328P** embarcado na plataforma Arduino.

### 🛠️ Especificações Técnicas:
* **Microcontrolador:** ATmega328P (Arquitetura RISC de 8 bits)
* **Linguagem Utilizada:** C++ / C (Acessando diretamente os registradores quando necessário)
* **Abordagem da Disciplina:** O foco não será apenas o uso de bibliotecas prontas da IDE do Arduino, mas sim entender o comportamento do hardware por trás do código (manipulação de portas por registradores como `DDRD`, `PORTD`, interrupções de hardware e timers).

---

## 📂 Organização das Pastas no Repositório

Sugerimos que as atividades sejam organizadas da seguinte forma para manter o ambiente limpo:

```text
├── 📂 Exercicios-Logica/       # Portas lógicas, álgebra booleana e simplificação de circuitos
├── 📂 Simulacoes-Circuitos/    # Arquivos do Logisim, Proteus ou Tinkercad
├── 📂 Assembly-ISA/            # Práticas de conjuntos de instruções e Assembly
└── 📂 Projeto-Arduino/         # Pasta do trabalho principal com o ATmega328P
    ├── 📂 src/                 # Códigos-fonte (.ino, .cpp, .h)
    ├── 📂 schematics/          # Diagramas de circuito, esquemáticos e conexões
    └── 📄 documentacao.md      # Relatório técnico do grupo
```

---

## 📝 Boas Práticas para o Projeto com Arduino

- **Evite poluir o Git:** Não suba pastas geradas automaticamente por IDEs (como `.build`, `.vscode`). Configure um arquivo `.gitignore` adequado.
- **Documente as Conexões:** No arquivo do seu projeto, inclua uma tabela mapeando quais componentes estão conectados em quais pinos do microcontrolador.
- **Código Limpo:** Comente as manipulações de bits e registradores para que o professor e os colegas compreendam a lógica de hardware aplicada.

---
