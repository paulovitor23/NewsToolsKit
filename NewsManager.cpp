#include "NewsManager.h"
#include <iostream>
#include <cctype>
#include <algorithm>
#include <iomanip> // Para std::setprecision

// Construtor vazio
NewsManager::NewsManager() {
}

bool NewsManager::BuscarNovasNoticias(std::string tema) {
    std::cout << "Buscando manchetes sobre '" << tema << "'..." << std::endl;
    
    // Extrair os dados básicos
    // PythonBridge::BuscarManchetes retorna vector<NewsStructure> incompleto
    // Limitado a 3 notícias para a apresentação
    std::vector<NewsStructure> manchetes = PythonBridge::BuscarManchetes(tema, 6);

    // Garante que a busca foi realizada com sucesso
    if (manchetes.empty()) {
        std::cout << "Nenhuma noticia encontrada." << std::endl;
        return false;
    }

    // Limpa o lote anterior para trazer novas notícias
    loteAtual.clear();

    std::cout << "Carregando conteudos completos..." << std::endl;

    // 2. Loop de carregamento completo (Prefetching)
    // Para cada manchete, chama o Python de novo para pegar o texto completo
    for (const auto& m : manchetes) {
        std::cout << " -> Baixando: " << m.titulo << "..." << std::endl;
        
        // Chama LerNoticia passando a URL que veio na manchete
        NewsStructure completa = PythonBridge::LerNoticia(m.url);
        
        // Se falhar o download, adiciona somente oque veio da busca
        if (completa.url.empty()) {
            loteAtual.push_back(m); 
        } else {
            // Garante que título, fonte e data sejam preservados
            if (completa.titulo.empty()) completa.titulo = m.titulo;
            if (completa.data.empty()) completa.data = m.data;
            if (completa.fonte.empty()) completa.fonte = m.fonte;

            // Isso ajuda no cache futuro e na precisão dos dados
            if (!completa.url.empty() && completa.url != m.url) {
                // Imprime o arquivo de debug
                std::cout << " [Redirect] " << m.url << " -> " << completa.url << std::endl;
            }
            
            loteAtual.push_back(completa);
        }
    }
    
    std::cout << "Sucesso! " << loteAtual.size() << " noticias carregadas na memoria." << std::endl;
    return true;
}

// Pega uma notícia do lote atual pelo índice
NewsStructure* NewsManager::GetNoticia(int indice) {
    // Verifica se é um índice válido
    if (indice >= 0 && indice < loteAtual.size()) {
        // Retorna o endereço da notícia
        return &loteAtual[indice];
    }
    // Retorna um ponteiro vazio 
    return nullptr;
}

NewsStructure NewsManager::LerNoticiaExtra(std::string url) {
    // Verifica se já existe no lote atual (busca por URL)
    for (const auto& news : loteAtual) {
        if (news.url == url) {
            std::cout << "[Cache] Noticia ja esta na memoria." << std::endl;
            // Retorna o endereço da notícia
            return news;
        }
    }

    // Se não existe, chama a função em python
    std::cout << "[Web] Buscando noticia externa..." << std::endl;
    return PythonBridge::LerNoticia(url);
}

void NewsManager::ExibirEstatisticas() {
    // Verifica se o lote atual está vazio
    if (loteAtual.empty()) {
        std::cout << "Nenhuma estatistica disponivel (lote vazio)." << std::endl;
        return;
    }

    // Variáveis das estatísticas
    std::map<std::string, int> contagemPorFonte;
    double somaTamanhoTitulos = 0;

    // Itera pelo lote atual para pegar as estatísticas
    for (const auto& news : loteAtual) {
        contagemPorFonte[news.fonte]++;
        somaTamanhoTitulos += news.titulo.length();
    }

    std::cout << "\n=== Estatisticas do Lote Atual ===" << std::endl;
    std::cout << "Total de Noticias: " << loteAtual.size() << std::endl;
    std::cout << "Media de caracteres nos titulos: " << (somaTamanhoTitulos / loteAtual.size()) << std::endl;
    
    std::cout << "Distribuicao por Fonte:" << std::endl;
    for (const auto& par : contagemPorFonte) {
        std::cout << " - " << par.first << ": " << par.second << std::endl;
    }
    std::cout << "==================================" << std::endl;
}

int NewsManager::GetTotalNoticias() const {
    return loteAtual.size();
}

const std::vector<NewsStructure>& NewsManager::GetLote() const {
    return loteAtual;
}

std::string ToLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return s;
}

// Filtro por fonte (remove o que não bate)
// Fácil de modificar para não remover
void NewsManager::FiltrarPorFonte(std::string fonteAlvo) {
    std::vector<NewsStructure> filtrado;
    std::string alvoLimpo = ToLower(fonteAlvo);

    // Itera pelas notícias e separa pela fonte desejada
    for (const auto& news : loteAtual) {
        std::string fonteAtual = ToLower(news.fonte);
        // Busca substring
        // .find garante essa busca parcial
        if (fonteAtual.find(alvoLimpo) != std::string::npos) {
            filtrado.push_back(news);
        }
    }
    // Feedback visual importante se zerar a lista
    if (filtrado.empty()) {
        std::cout << "[Aviso] Nenhuma noticia encontrada para a fonte '" << fonteAlvo << "'." << std::endl;
        std::cout << "O lote atual nao foi alterado." << std::endl;
    } else {
        loteAtual = filtrado;
        std::cout << "Filtro aplicado. Restaram " << loteAtual.size() << " noticias." << std::endl;
    }
}

void NewsManager::ProcessarKeywords(int indice) {
    // Passa o endereço da notícia no índice para o ponteiro news
    NewsStructure* news = GetNoticia(indice);

    // Verifica se o índice existe
    if (news == nullptr) {
        std::cout << "Indice invalido." << std::endl;
        return;
    }

    // Se o conteúdo estiver vazio, chama a função de ler a notícia
    if (news->conteudo.empty()) {
        std::cout << "Conteudo nao carregado. Baixando noticia primeiro..." << std::endl;
        // Chama a função .py
        NewsStructure temp = PythonBridge::LerNoticia(news->url);
        // Verifica se o conteúdo foi baixado
        if (temp.conteudo.empty()) { 
            std::cout << "Erro: Falha ao baixar o conteudo da web." << std::endl;
            return; // Sai antes de alterar a notícia original ou tentar extrair as palavras-chave
        }
        // Altera o conteúdo e o resumo
        news->conteudo = temp.conteudo;
        news->resumo = temp.resumo;
        
        // Verifica se o novo conteúdo foi baixado
        if (news->conteudo.empty()) {
            std::cout << "Erro: Nao foi possivel obter o texto da noticia." << std::endl;
            return;
        }
    }

    std::cout << "Extraindo palavras-chave via Python..." << std::endl;
    
    // Chama a função de extração em python
    news->keywords = PythonBridge::ExtrairKeywords(news->conteudo, 5);

    // Verifica se encontrou palavras-chave
    if (news->keywords.empty()) {
        std::cout << "Nenhuma palavra-chave relevante identificada." << std::endl;
    } else {
        std::cout << "Palavras-chave extraidas com sucesso!" << std::endl;
        // Exibe imediatamente
        for (const auto& k : news->keywords) {
            std::cout << " - " << k.first << " (Recorrencia: " << k.second << ")" << std::endl;
        }
    }
}