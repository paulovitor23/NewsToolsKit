# news_toolkit.py
import scraper
import analise
import time

# --- Funções para Interface com C++ ---

def buscar_manchetes(tema: str, limite: int):
    """
    Função chamada pelo Menu C++ Opção 1.
    """
    print(f"[Python] Buscando até {limite} notícias sobre '{tema}'...")
    return scraper.buscar_rss_google(tema, limite)

def ler_noticia(url: str):
    """
    Função chamada pelo Menu C++ Opção 2.
    """
    print(f"[Python] Baixando e processando notícia...")
    dados = scraper.baixar_e_extrair(url)
    
    if dados["sucesso"]:
        print(f"[Python] Enviando para Inteligência Artificial...")
        resumo_ia = analise.gerar_resumo_ia(dados["texto"])
        
        return {
            "titulo": dados["titulo"],
            "texto_completo": dados["texto"],
            "resumo": resumo_ia,
            "url_real": dados["url_real"],
            "status": "OK"
        }
    else:
        return {
            "titulo": "Erro",
            "texto_completo": "",
            "resumo": f"Erro: {dados.get('erro', 'Desconhecido')}",
            "status": "ERRO"
        }

def extrair_keywords(texto: str, k: int):
    """
    Função chamada pelo Menu C++ Opção 3.
    """
    if not texto: return []
    return analise.calcular_palavras_chave(texto, k)

# --- Bloco de Teste Local ---
if __name__ == "__main__":
    print("--- Teste de Integração ---")
    
    tema = "Política"
    limite = 1 
    
    lista = buscar_manchetes(tema, limite)
    print(f"Encontradas: {len(lista)}")
    
    for i, item in enumerate(lista):
        # MUDANÇA AQUI: Agora mostramos Data e Fonte no print
        print(f"\n--- Notícia {i+1}: {item['titulo']} ---")
        print(f"    Data: {item['data']} | Fonte: {item['fonte']}") 
        
        conteudo = ler_noticia(item['link'])
        
        if conteudo['status'] == "OK":
            print(f"Link: {conteudo['url_real']}")
            time.sleep(1)
            print("-" * 40)
            print(f"RESUMO IA:\n{conteudo['resumo']}")
            print("-" * 40)
            
            time.sleep(1)
            kws = extrair_keywords(conteudo['texto_completo'], 5)
            print(f"Keywords: {kws}")
        else:
            print(f"FALHA: {conteudo['resumo']}")