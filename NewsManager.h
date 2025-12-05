#pragma once

#include <vector>
#include <string>
#include <map> // Usado nas estatísticas
#include "NewsStructure.h"
#include "PythonBridge.h"

class NewsManager {
private:
    // O "Banco de Dados" em memória
    // Vetor de notícias
    std::vector<NewsStructure> loteAtual;

public:
    // Construtor
    NewsManager();

    // Busca manchetes e já carrega o conteúdo delas
    // Retorna true se encontrou notícias, false se falhou
    bool BuscarNovasNoticias(std::string tema);

    // Lógica para exibir uma notícia específica 
    // Se o input for um número (índice), exibe a informação, já existente, do vetor
    // Se for URL, chama a função em python e baixa na hora
    NewsStructure* GetNoticia(int indice);
    NewsStructure LerNoticiaExtra(std::string url);

    // Extração das palavras-chave
    void ProcessarKeywords(int indice);

    // Filtros e Estatísticas 
    void FiltrarPorFonte(std::string fonte);
    void ExibirEstatisticas(); 
    
    // Retorna o número de notícias existentes nos dados
    int GetTotalNoticias() const;

    // Getter para o lote atual
    const std::vector<NewsStructure>& GetLote() const;
};