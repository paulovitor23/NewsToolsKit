#pragma once

#include <pybind11/embed.h> //Interpreta o python dentro do c++
#include <pybind11/stl.h> //Faz a conversão de tipos
#include <vector>
#include <string>
#include <iostream>
#include "NewsStructure.h"

namespace py = pybind11;

class PythonBridge {
private:
    // Armazenamento dos scripts em python
    // static garante que eles continuem existindo durante toda a execução
    // Método py::module serve como um import, carregando todo o script .py 
    // para a memória do c++.
    static py::module searcher_module;
    static py::module processor_module;

public:
    // O uso de static nos métodos garante a desvinculação dos métodos com o objeto

    // Carrega os scripts em python
    static void Init();
    
    // Chama a função em python e retorna um vector de notícias(ainda incompletas)
    static std::vector<NewsStructure> BuscarManchetes(std::string tema, int limite = 5);

    // Chama a função em python e retorna uma notícia completa
    static NewsStructure LerNoticia(std::string url);

    // Chama a função em python e retorna as palavras-chave
    static std::vector<std::pair<std::string, double>> ExtrairKeywords(std::string texto, int quantidade = 5);

    // Garante o desacoplamento das funções da classe com as funções em python
    // Necessário para o fechamento correto do interpretador
    static void Finalize();
    
};