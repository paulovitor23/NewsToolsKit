#pragma once // Substitui o #ifndef

#include <string>
#include <vector>
#include <utility> // Para std::pair

// Implementa toda a estrutura de uma notícia.
struct NewsStructure       {
    // Dados Básicos (Vindos da Busca)
    std::string titulo;
    std::string fonte;
    std::string data;
    std::string url;

    // Dados Detalhados (Vindos da Leitura)
    // Apesar de detalhados, serão preenchidos na criação do objeto.
    std::string conteudo; 
    std::string resumo;
    
    // Lista de palavras-chave: (Vinda da busca por palavras-chave)
    // Modelo: (Palavra, Quantidade)
    std::vector<std::pair<std::string, double>> keywords;

    // Construtor da struct
    // std::move() transfere a posse da memória das variáveis temporárias 
    // para dentro da struct, evitando cópias desnecessárias.
    // Pode ser substituído passando uma referência para o valor original
    // mas esta ação gasta mais memória e é mais lenta.
    NewsStructure(std::string t, std::string f, std::string d, std::string u,
                std::string c, std::string r, 
                std::vector<std::pair<std::string, double>> k)
        : titulo(std::move(t)), 
          fonte(std::move(f)), 
          data(std::move(d)), 
          url(std::move(u)),
          conteudo(std::move(c)), 
          resumo(std::move(r)), 
          keywords(std::move(k)) {}
 
    // Construtor vazio
    // Serve para iniciar a struct sem passar parâmetros
    NewsStructure() = default;
};

