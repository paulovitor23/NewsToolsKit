#include "PythonBridge.h"

// Inicialização das variáveis estáticas que vão receber os scripts
py::module PythonBridge::searcher_module;
py::module PythonBridge::processor_module;

void PythonBridge::Init() {
    try {
        // Adiciona o diretório atual (".") ao sys.path do Python
        py::module sys = py::module::import("sys");
        // Acessa um membro de um objeto. Usado para pegar funções dentro dos módulos ou manipular listas do sistema
        sys.attr("path").attr("append")("."); 

        // Importa os scripts Python
        // O Python vai procurar por "news_searcher.py" e "text_processor.py" na pasta do executável
        searcher_module = py::module::import("news_searcher");
        processor_module = py::module::import("text_processor");

        std::cout << "[C++] Modulos Python carregados com sucesso." << std::endl;

    } catch (const py::error_already_set& e) {
        // pybind11 já possúi uma exceção específica
        std::cerr << "[Erro Python Init]: " << e.what() << std::endl;
        std::cerr << "Dica: Verifique se 'news_searcher.py' e 'text_processor.py' estao na pasta." << std::endl;
        exit(1);
    }
}

std::vector<NewsStructure> PythonBridge::BuscarManchetes(std::string tema, int limite) {
    // Vetor para armazenar as notícias buscadas
    std::vector<NewsStructure> resultados;

    try {
        // Verifica se o módulo de pesquisa foi carregado corretamente
        if (searcher_module.is_none()) {
             std::cerr << "[Erro] Modulo searcher nao carregado." << std::endl;
             return resultados;
        }
        // Obtem a função buscar_manchetes em python
        // py::object serve para armazenar um tipo qualquer 
        // .attr() puxa a função
        py::object func = searcher_module.attr("buscar_manchetes");
        // Executa a função em python passando os parâmetros já convertidos
        py::object retorno_py = func(tema, limite);

        // Itera por todos os dicionários da lista de manchetes
        for (auto item : retorno_py) {
            // Trata cada item da lista como um dicionário
            py::dict noticia_dict = item.cast<py::dict>();
            // Inicia uma struct
            NewsStructure news;
            
            // Verifica e extrai cada campo
            if (noticia_dict.contains("titulo")) 
                news.titulo = noticia_dict["titulo"].cast<std::string>();
            
            if (noticia_dict.contains("fonte")) 
                news.fonte = noticia_dict["fonte"].cast<std::string>();
            
            if (noticia_dict.contains("data")) 
                news.data = noticia_dict["data"].cast<std::string>();
                
            if (noticia_dict.contains("url")) 
                news.url = noticia_dict["url"].cast<std::string>();

            resultados.push_back(news);
        }
        // Trata de qualquer possível erro
    } catch (const py::error_already_set& e) {
        std::cerr << "[Erro Python BuscarManchetes]: " << e.what() << std::endl;
    }

    return resultados;
}

NewsStructure PythonBridge::LerNoticia(std::string url) {
    // Cria uma struct temporária
    NewsStructure news_completa;
    // Garante a existência da url na struct
    news_completa.url = url;

    try {
        // Caso onde o script não foi carregado
        if (processor_module.is_none()) return news_completa;

        // Chama a função em python
        py::object func = processor_module.attr("ler_noticia");

        // Executa a função com o parâmetro já convertido
        py::object retorno_py = func(url); 

        // Caso o retorno da função .py seja vazio ele retorna, em cpp, o a struct somente com a url
        if (retorno_py.is_none()) return news_completa;

        // Trata o retorno como um dicionário
        py::dict dados = retorno_py.cast<py::dict>();

        // .cast força a conversão de tipo
        // Adiciona o conteúdo na struct
        if (dados.contains("conteudo"))
            news_completa.conteudo = dados["conteudo"].cast<std::string>();
        // Adiciona o resumo na struct
        if (dados.contains("resumo"))
            news_completa.resumo = dados["resumo"].cast<std::string>();
        
        // Atualiza título/fonte se necessário
        if (dados.contains("titulo")) news_completa.titulo = dados["titulo"].cast<std::string>();
        if (dados.contains("fonte")) news_completa.fonte = dados["fonte"].cast<std::string>();
        
        // Trata dos erros
    } catch (const py::error_already_set& e) {
        std::cerr << "[Erro Python LerNoticia]: " << e.what() << std::endl;
    }
    // O retorno é a struct criada
    return news_completa;
}

// As palavras-chave serão um vetor de pares com a palavra em questão e a quantidade da mesma
std::vector<std::pair<std::string, double>> PythonBridge::ExtrairKeywords(std::string texto, int quantidade) {
    // Inicia um vetor de palavras-chave vazio
    std::vector<std::pair<std::string, double>> keywords;
    
    // Se o texto estiver vazio, retorna um vetor vazio
    if (texto.empty()) return keywords;

    try {
        // Chama a função em python
        py::object func = processor_module.attr("extrair_palavras_chave");
        
        // Executa a função com os parâmetros já convertidos
        py::object retorno = func(texto, quantidade); 

        // Converte a lista de tuplas do Python [(str, float)] para vector<pair> do C++
        keywords = retorno.cast<std::vector<std::pair<std::string, double>>>();

        // Trata dos erros
    } catch (const py::error_already_set& e) {
        std::cerr << "[Erro Python Keywords]: " << e.what() << std::endl;
    }

    // Retorna o vetor de palavras-chave
    return keywords;
}

void PythonBridge::Finalize() {
    // Substitui os módulos por objetos vazios
    // Força a liberação da memória do Python
    searcher_module = py::module();
    processor_module = py::module();
}