#pragma once

#include <pybind11/embed.h> // Interpreta o python dentro do c++
#include <pybind11/stl.h>   // Faz a conversão de tipos
#include <vector>
#include <string>
#include <iostream>
#include "NewsStructure.h"

namespace py = pybind11;

class PythonBridge {
private:
    // Armazenamento do script principal em python (news_toolkit.py)
    // static garante que ele continue existindo durante toda a execução
    static py::module toolkit_module;

public:
    // O uso de static nos métodos garante a desvinculação dos métodos com o objeto

    // Carrega o script 'news_toolkit.py'
    static void Init();
    
    // Chama a função em python e retorna um vector de notícias (ainda incompletas)
    static std::vector<NewsStructure> BuscarManchetes(std::string tema, int limite = 5);

    // Chama a função em python e retorna uma notícia completa (texto + resumo)
    static NewsStructure LerNoticia(std::string url);

    // Chama a função em python e retorna as palavras-chave
    static std::vector<std::pair<std::string, double>> ExtrairKeywords(std::string texto, int quantidade = 5);

    // Libera a memória do módulo Python
    static void Finalize();
};