#include "PythonBridge.h"

// Inicialização da variável estática que vai receber o script integrador
py::module PythonBridge::toolkit_module;

void PythonBridge::Init() {
    try {
        // Adiciona o diretório atual (".") ao sys.path do Python
        py::module sys = py::module::import("sys");
        // Acessa um membro de um objeto. Usado para manipular listas do sistema
        sys.attr("path").attr("append")("."); 

        // Importa o script Python unificado "news_toolkit.py"
        // Este script atua como uma fachada (Facade) para o scraper e a análise
        toolkit_module = py::module::import("news_toolkit");

        std::cout << "[C++] Modulo 'news_toolkit' carregado com sucesso." << std::endl;

    } catch (const py::error_already_set& e) {
        // pybind11 já possúi uma exceção específica
        std::cerr << "[Erro Python Init]: " << e.what() << std::endl;
        std::cerr << "Dica: Verifique se 'news_toolkit.py', 'scraper.py' e 'analise.py' estao na pasta." << std::endl;
        exit(1);
    }
}

std::vector<NewsStructure> PythonBridge::BuscarManchetes(std::string tema, int limite) {
    // Vetor para armazenar as notícias buscadas
    std::vector<NewsStructure> resultados;

    try {
        // Verifica se o módulo toolkit foi carregado corretamente
        if (toolkit_module.is_none()) {
             std::cerr << "[Erro] Modulo toolkit nao carregado." << std::endl;
             return resultados;
        }
        
        // Obtem a função buscar_manchetes no news_toolkit.py
        py::object func = toolkit_module.attr("buscar_manchetes");
        
        // Executa a função em python passando os parâmetros já convertidos
        py::object retorno_py = func(tema, limite);

        // Itera por todos os dicionários da lista de manchetes
        for (auto item : retorno_py) {
            // Trata cada item da lista como um dicionário
            py::dict noticia_dict = item.cast<py::dict>();
            // Inicia uma struct
            NewsStructure news;
            
            // Verifica e extrai cada campo, adaptando as chaves do Python para o C++
            if (noticia_dict.contains("titulo")) 
                news.titulo = noticia_dict["titulo"].cast<std::string>();
            
            if (noticia_dict.contains("fonte")) 
                news.fonte = noticia_dict["fonte"].cast<std::string>();
            
            if (noticia_dict.contains("data")) 
                news.data = noticia_dict["data"].cast<std::string>();
                
            // O Python envia "link", mas a struct espera "url"
            if (noticia_dict.contains("link")) 
                news.url = noticia_dict["link"].cast<std::string>();
            else if (noticia_dict.contains("url"))
                news.url = noticia_dict["url"].cast<std::string>();

            resultados.push_back(news);
        }
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
        if (toolkit_module.is_none()) return news_completa;

        // Chama a função 'ler_noticia' no news_toolkit.py
        py::object func = toolkit_module.attr("ler_noticia");

        // Executa a função com o parâmetro já convertido
        py::object retorno_py = func(url); 

        // Caso o retorno da função .py seja vazio ou None
        if (retorno_py.is_none()) return news_completa;

        // Trata o retorno como um dicionário
        py::dict dados = retorno_py.cast<py::dict>();

        // Verifica o status retornado pelo Python
        if (dados.contains("status")) {
            std::string status = dados["status"].cast<std::string>();
            if (status != "OK") {
                std::cerr << "[Aviso] Python retornou status de erro na leitura." << std::endl;
            }
        }

        // O Python envia "texto_completo", mas a struct espera "conteudo"
        if (dados.contains("texto_completo"))
            news_completa.conteudo = dados["texto_completo"].cast<std::string>();
        else if (dados.contains("conteudo"))
             news_completa.conteudo = dados["conteudo"].cast<std::string>();

        // Adiciona o resumo na struct
        if (dados.contains("resumo"))
            news_completa.resumo = dados["resumo"].cast<std::string>();
        
        // Atualiza título se o scraper tiver conseguido um melhor
        if (dados.contains("titulo")) 
            news_completa.titulo = dados["titulo"].cast<std::string>();
        
    } catch (const py::error_already_set& e) {
        std::cerr << "[Erro Python LerNoticia]: " << e.what() << std::endl;
    }
    // O retorno é a struct criada
    return news_completa;
}

std::vector<std::pair<std::string, double>> PythonBridge::ExtrairKeywords(std::string texto, int quantidade) {
    // Inicia um vetor de palavras-chave vazio
    std::vector<std::pair<std::string, double>> keywords;
    
    // Se o texto estiver vazio, retorna um vetor vazio
    if (texto.empty()) return keywords;

    try {
        // Chama a função 'extrair_keywords' no news_toolkit.py
        // Nota: Nome da função corrigido em relação ao código antigo
        py::object func = toolkit_module.attr("extrair_keywords");
        
        // Executa a função com os parâmetros já convertidos
        py::object retorno = func(texto, quantidade); 

        // Converte a lista de tuplas do Python [(str, float)] para vector<pair> do C++
        keywords = retorno.cast<std::vector<std::pair<std::string, double>>>();

    } catch (const py::error_already_set& e) {
        std::cerr << "[Erro Python Keywords]: " << e.what() << std::endl;
    }

    // Retorna o vetor de palavras-chave
    return keywords;
}

void PythonBridge::Finalize() {
    // Substitui o módulo por objeto vazio
    // Força a liberação da memória do Python
    toolkit_module = py::module();
}