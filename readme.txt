================================================================================
PROJETO: NewsToolkit CLI - Sistema Híbrido de Análise de Notícias
================================================================================

AUTORES:
- Paulo Vitor Couto Doederlein
- Erik Branco Queiroz

DISCIPLINA: Linguagens de Programação
DATA: Dezembro/2025

================================================================================
1. VISÃO GERAL
================================================================================

O NewsToolkit CLI é uma ferramenta de linha de comando desenvolvida para buscar, 
ler, resumir e analisar notícias em tempo real. 

O sistema utiliza uma Arquitetura Híbrida (Embedding):
1. C++ (Core): Responsável pelo gerenciamento de memória, interface com usuário, 
   estruturas de dados e lógica de filtros/estatísticas.
2. Python (Worker): Atua como módulo de inteligência, realizando web scraping,
   processamento de linguagem natural (NLP) e conexão com IA Generativa (Google Gemini).

O objetivo é permitir o consumo rápido de informação sem a necessidade de acessar 
navegadores pesados, entregando resumos e palavras-chave processados automaticamente.

================================================================================
2. ESTRUTURA DE ARQUIVOS
================================================================================

O projeto é dividido em dois núcleos de linguagem:

A. NÚCLEO C++ (Gerenciamento e Interface)
-----------------------------------------
- main.cpp:
  Ponto de entrada. [cite_start]Inicializa o interpretador Python e exibe o menu interativo.
  
- NewsManager.h / .cpp:
  O "cérebro" do sistema. Gerencia o vetor de notícias (`loteAtual`), controla o 
  fluxo de dados e executa a lógica de negócios (filtros e cálculos).

- PythonBridge.h / .cpp:
  A camada de integração. Utiliza a biblioteca `pybind11` para traduzir dados entre
  C++ e Python. Nenhuma outra classe acessa o Python diretamente.

- NewsStructure.h:
  Define a `struct` principal que transita pelo sistema (contém título, url,
  conteúdo, resumo, etc.).

B. MÚDULOS PYTHON (Inteligência e Coleta)
-----------------------------------------
- news_toolkit.py:
  Fachada (Facade) que unifica as funções do Python para o C++. É o único arquivo
  que o `PythonBridge` importa.

- scraper.py:
  Responsável por conectar a Feeds RSS (CNN, G1, BBC) e realizar o download do
  HTML bruto das páginas (Web Scraping).

- analise.py:
  Responsável pelo processamento pesado. Contém a conexão com a API de IA para
  gerar resumos e utiliza NLTK para extração estatística de palavras-chave.

================================================================================
3. FUNCIONALIDADES E USO (MENU)
================================================================================

O sistema é operado através de um menu numérico no terminal. Abaixo, a descrição
técnica de cada opção:

[1] BUSCAR NOTÍCIAS POR TEMA
    - Input: Uma palavra-chave (ex: "Política", "Tecnologia").
    - Processo: O Python busca em feeds RSS e retorna manchetes básicas.
    - Output: Uma lista numerada contendo Título, Fonte e Data.
    - Nota: Nesta etapa, o conteúdo completo ainda não foi baixado (Prefetching leve).

[2] LER NOTÍCIA (DETALHES + RESUMO)
    - Input: O número da notícia (ID) listada na opção 1 ou uma URL externa.
    - Processo: O sistema baixa o texto completo, limpa a formatação HTML e
      envia para a IA gerar um resumo de 2-3 frases.
    - Output: Texto completo limpo e um resumo gerado por IA.

[3] EXTRAIR PALAVRAS-CHAVE
    - Input: O ID de uma notícia já carregada.
    - Processo: Utiliza processamento de linguagem natural (NLTK) para identificar
      os termos mais relevantes (unigramas e bigramas) baseados em frequência,
      ignorando "stopwords" (artigos, preposições).
    - Output: Lista de palavras-chave com seus respectivos scores de relevância.

[4] FILTRAR NOTÍCIAS POR FONTE
    - Input: Nome da fonte (ex: "G1").
    - Processo: Executado puramente em C++. Percorre as notícias em memória e
      remove aquelas que não correspondem ao filtro (Case Insensitive).
    - Output: Atualização da lista de notícias ativa.

[5] VER HISTÓRICO E ESTATÍSTICAS
    - Input: Nenhum.
    - Processo: Analisa o vetor de notícias atual em memória.
    - Output: 
      * Total de notícias carregadas.
      * Média de caracteres dos títulos (análise de concisão).
      * Distribuição de notícias por fonte (ex: G1: 2, CNN: 3).

================================================================================
4. OBSERVAÇÕES TÉCNICAS
================================================================================

- API Externa: O módulo de análise depende de uma chave de API do Google Gemini.
- Persistência: O sistema opera em memória (RAM). Ao fechar o programa, o
  lote de notícias é descartado.
- Dependências: Requer as bibliotecas Python listadas no manual de instalação
  (requests, feedparser, newspaper3k, nltk).

================================================================================
5. COMPILAÇÃO E EXECUÇÃO
================================================================================

PRÉ-REQUISITOS GERAIS
---------------------
1. Compilador C++ compatível com C++17 (g++ ou clang).
2. Python 3.8 ou superior instalado.
3. Ferramenta 'Make'.

================================================================================
5.1 AMBIENTE LINUX (Ubuntu / Debian / Mint)
================================================================================

PASSO 1: Instalar dependências do sistema
Abra o terminal na pasta do projeto e rode:
$ sudo apt update
$ sudo apt install build-essential python3-dev python3-venv python3-pip

PASSO 2: Criar e ativar o Ambiente Virtual (Obrigatório)
Para evitar erros de permissão e conflitos de versão, isolamos o Python:
$ python3 -m venv .venv
$ source .venv/bin/activate

(Seu terminal deve mostrar "(.venv)" no início da linha após esse comando).

PASSO 3: Instalar as bibliotecas Python necessárias
Com o ambiente ativado, instale os pacotes:
$ pip install pybind11 requests feedparser newspaper3k nltk lxml_html_clean

PASSO 4: Baixar os dados de IA (NLTK)
O sistema precisa de dicionários para resumir textos. Rode este comando para baixá-los:
$ python3 -c "import nltk; nltk.download('punkt'); nltk.download('punkt_tab'); nltk.download('stopwords')"

PASSO 5: Compilar o projeto
Limpe versões antigas e compile o executável C++:
$ make clean
$ make

PASSO 6: Executar
Como as bibliotecas estão isoladas no ambiente virtual, o C++ precisa saber onde elas estão.
Rode o comando abaixo (exatamente como está) para iniciar:

$ PYTHONPATH=$(ls -d .venv/lib/python3.*/site-packages) ./NewsToolkitCLI

================================================================================
5.2 AMBIENTE MACOS
================================================================================

PASSO 1: Instalar ferramentas básicas
Recomendamos o uso do Homebrew.
$ xcode-select --install
$ brew install python3 make

PASSO 2: Configurar o Ambiente Virtual
$ python3 -m venv .venv
$ source .venv/bin/activate

PASSO 3: Instalar bibliotecas e dados
$ pip install pybind11 requests feedparser newspaper3k nltk lxml_html_clean
$ python3 -c "import nltk; nltk.download('punkt'); nltk.download('punkt_tab'); nltk.download('stopwords')"

PASSO 4: Compilar e Executar
$ make clean
$ make
$ PYTHONPATH=$(ls -d .venv/lib/python3.*/site-packages) ./NewsToolkitCLI

================================================================================
5.3 AMBIENTE WINDOWS
================================================================================

NOTA: Devido ao uso de Makefiles e integração nativa com Python (python3-config),
a forma mais robusta de rodar este projeto no Windows é via WSL (Windows Subsystem for Linux).

PASSO 1: Instalar o WSL
Abra o PowerShell como Administrador e digite:
> wsl --install

Reinicie o computador se solicitado. Ao abrir o "Ubuntu" no menu iniciar, você terá um terminal Linux completo.

PASSO 2: Executar
Dentro do terminal do WSL, siga EXATAMENTE as instruções da seção "1. AMBIENTE LINUX" acima.

================================================================================
5.4 SOLUÇÃO DE PROBLEMAS
================================================================================

ERRO: "externally-managed-environment" ao rodar pip install
CAUSA: Você esqueceu de ativar o ambiente virtual ou está tentando instalar no sistema global.
SOLUÇÃO: Rode "source .venv/bin/activate" antes de usar o pip.

ERRO: "ModuleNotFoundError: No module named 'feedparser'" (ou similar)
CAUSA: O programa C++ não achou as pastas do ambiente virtual.
SOLUÇÃO: Não rode apenas "./NewsToolkitCLI". Use o comando completo com PYTHONPATH descrito no Passo 6.

ERRO: "Resource punkt not found"
CAUSA: O download automático dos dados de linguagem falhou.
SOLUÇÃO: Rode manualmente o comando do Passo 4 (import nltk...).

ERRO: "pybind11/pybind11.h: No such file or directory"
CAUSA: A biblioteca pybind11 não foi instalada ou o C++ não a encontrou.
SOLUÇÃO: Verifique se rodou "pip install pybind11" dentro do ambiente virtual.