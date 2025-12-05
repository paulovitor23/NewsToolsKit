#include <iostream>
#include <limits> // Para limpar o buffer de entrada (cin.ignore)
#include "PythonBridge.h"
#include "NewsManager.h"

namespace py = pybind11;

// Função auxiliar para esperar o usuário apertar Enter
void Pausar() {
    std::cout << "\nPressione ENTER para continuar...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

int main() {
    // Inicializa o Python
    py::scoped_interpreter guard{}; 

    // Carrega os scripts Python
    PythonBridge::Init();

    // Instancia o gerenciador de notícias
    NewsManager manager;

    int opcao = -1;

    while (opcao != 0) {
        std::cout << "=========================================" << std::endl;
        std::cout << "      NEWS TOOLKIT CLI - MENU" << std::endl;
        std::cout << "=========================================" << std::endl;
        std::cout << " [1] Buscar Noticias por Tema" << std::endl;
        std::cout << " [2] Ler Noticia (Detalhes + Resumo)" << std::endl;
        std::cout << " [3] Extrair palavras-chave" << std::endl;
        std::cout << " [4] Filtrar Noticias por Fonte" << std::endl;
        std::cout << " [5] Ver Historico e Estatisticas" << std::endl;
        std::cout << " [0] Sair" << std::endl;
        std::cout << "=========================================" << std::endl;
        std::cout << " Escolha uma opcao: ";
        
        if (!(std::cin >> opcao)) {
            // Tratamento básico se digitar letra
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            opcao = -1;
        }
        
        // Limpa o buffer do enter após ler o número
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "\n";

        switch (opcao) {
            case 1: {
                // BUSCAR
                std::string tema;
                std::cout << "Digite o tema da busca: ";
                std::getline(std::cin, tema);
                
                if (manager.BuscarNovasNoticias(tema)) {
                    // Lista as manchetes encontradas
                    auto lote = manager.GetLote();
                    std::cout << "\n--- Resultados ---" << std::endl;
                    for (size_t i = 0; i < lote.size(); i++) {
                        std::cout << "[" << i << "] " << lote[i].titulo 
                                  << " (" << lote[i].fonte << ")" << std::endl;
                    }
                }
                Pausar();
                break;
            }

            case 2: {
                // LER NOTÍCIA
                // Se o usuário digitar um número, abre do vetor.
                // Se digitar texto (URL), busca fora.
                std::string entrada;
                std::cout << "Digite o ID da noticia (ex: 0) ou uma URL completa: ";
                std::getline(std::cin, entrada);

                NewsStructure* n = nullptr;
                NewsStructure n_externa;

                // Tenta verificar se é um número (índice do vetor)
                bool isNumero = !entrada.empty() && std::all_of(entrada.begin(), entrada.end(), ::isdigit);

                if (isNumero) {
                    int id = std::stoi(entrada);
                    n = manager.GetNoticia(id);
                    if (n == nullptr) {
                        std::cout << "Erro: ID invalido." << std::endl;
                    }
                } else {
                    // Assume que é URL
                    n_externa = manager.LerNoticiaExtra(entrada);
                    if (!n_externa.url.empty()) {
                        n = &n_externa;
                    } else {
                        std::cout << "Erro: Nao foi possivel carregar a URL." << std::endl;
                    }
                }

                if (n != nullptr) {
                    std::cout << "=== " << n->titulo << " ===" << std::endl;
                    std::cout << "Fonte: " << n->fonte << " | Data: " << n->data << "\n" << std::endl;
                    std::cout << "[RESUMO]:\n" << n->resumo << "\n" << std::endl;
                    std::cout << "[CONTEUDO COMPLETO]:\n" << n->conteudo << "\n" << std::endl;
                    
                    if (!n->keywords.empty()) {
                        std::cout << "[PALAVRAS-CHAVE]: ";
                        for (auto& k : n->keywords) {
                            std::cout << k.first << "(" << k.second << ") ";
                        }
                        std::cout << std::endl;
                    }
                }
                Pausar();
                break;
            }

            case 3: {
                int id;
                std::cout << "Digite o ID da noticia para analisar: ";
                if (std::cin >> id) {
                    manager.ProcessarKeywords(id);
                } else {
                    std::cout << "Entrada invalida." << std::endl;
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
                Pausar();
                break;
            }

            case 4: {
                // FILTRAR
                std::string fonte;
                std::cout << "Digite o nome da fonte para filtrar (ex: G1): ";
                std::getline(std::cin, fonte);
                manager.FiltrarPorFonte(fonte);
                Pausar();
                break;
            }

            case 5: {
                // ESTATÍSTICAS
                manager.ExibirEstatisticas();
                Pausar();
                break;
            }

            case 0:
                std::cout << "Encerrando sistema..." << std::endl;
                break;

            default:
                std::cout << "Opcao invalida!" << std::endl;
                Pausar();
                break;
        }
    }

    // 4. Finalização Manual das funções .py
    PythonBridge::Finalize();

    return 0;
}