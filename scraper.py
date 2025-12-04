# scraper.py
import feedparser
import requests
from newspaper import Article
import re 

# Lista de feeds
FEEDS_DIRETOS = [
    {"nome": "CNN Brasil", "url": "https://www.cnnbrasil.com.br/feed/"},
    {"nome": "BBC Brasil", "url": "http://feeds.bbci.co.uk/portuguese/rss.xml"},
    {"nome": "UOL", "url": "http://rss.uol.com.br/feed/noticias.xml"},
    {"nome": "G1", "url": "https://g1.globo.com/rss/g1/"} 
]

def _limpar_texto_bruto(texto):
    if not texto: return ""
    texto = re.sub(r'(?<=[a-z])(?=[A-Z][a-z]+:)', ' ', texto)
    
    linhas = texto.split('\n')
    linhas_limpas = []
    
    for linha in linhas:
        linha = linha.strip()
        if (linha.lower().startswith("crédito") or 
            linha.lower().startswith("foto:") or 
            "via getty images" in linha.lower()):
            continue
        linhas_limpas.append(linha)
    
    return "\n".join(linhas_limpas)

def buscar_rss_google(tema: str, limite: int):
    
    resultados = []
    tema = tema.lower()
    
    for fonte in FEEDS_DIRETOS:
        if len(resultados) >= limite: break
        try:
            feed = feedparser.parse(fonte["url"])
            for entry in feed.entries:
                if len(resultados) >= limite: break
                
                titulo = entry.title.lower()
                resumo = entry.summary.lower() if 'summary' in entry else ""
                
                if tema in titulo or tema in resumo:
                    # Tenta pegar a data de publicação ou atualização
                    # Se não achar nenhum, aí sim usa "Data desconhecida"
                    data_publicacao = entry.get("published", entry.get("updated", "Data desconhecida"))
                    
                    resultados.append({
                        "titulo": entry.title,
                        "link": entry.link,
                        "data": data_publicacao, # <--- MUDANÇA AQUI
                        "fonte": fonte["nome"]
                    })
        except: continue

    if not resultados:
        print(f"[Scraper] Nenhuma notícia exata sobre '{tema}'. Pegando as últimas...")
        for fonte in FEEDS_DIRETOS:
            if len(resultados) >= limite: break
            try:
                feed = feedparser.parse(fonte["url"])
                for entry in feed.entries:
                    if len(resultados) >= limite: break
                    
                    data_publicacao = entry.get("published", entry.get("updated", "Data desconhecida"))

                    resultados.append({
                        "titulo": entry.title,
                        "link": entry.link,
                        "data": data_publicacao, # <--- MUDANÇA AQUI
                        "fonte": fonte["nome"]
                    })
            except: continue

    return resultados

def baixar_e_extrair(url: str):
    
    headers = {
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36'
    }

    try:
        response = requests.get(url, headers=headers, timeout=15)
        response.raise_for_status() 

        article = Article(url)
        article.set_html(response.text) 
        article.parse()
        
        texto_limpo = _limpar_texto_bruto(article.text)
        
        return {
            "sucesso": True,
            "titulo": article.title,
            "texto": texto_limpo, 
            "url_real": response.url
        }
    except Exception as e:
        return {"sucesso": False, "erro": str(e)}