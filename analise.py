# analise.py
import requests 
import json
import nltk
from nltk.corpus import stopwords
from nltk.tokenize import word_tokenize
from collections import Counter


API_KEY = "Minha-Chave" 

def configurar_nltk():
    recursos = ['punkt', 'stopwords', 'punkt_tab']
    for r in recursos:
        try:
            nltk.data.find(f'tokenizers/{r}')
        except LookupError:
            nltk.download(r, quiet=True)

configurar_nltk()

# 2. FUNÇÕES DA INTELIGÊNCIA ARTIFICIAL (RESUMO)

def _descobrir_modelo_disponivel():
    """
    Pergunta ao Google qual modelo está ativo para evitar erro 404.
    """
    url_list = f"https://generativelanguage.googleapis.com/v1beta/models?key={API_KEY}"
    try:
        response = requests.get(url_list)
        if response.status_code == 200:
            dados = response.json()
            # Prioridade: Flash > Pro > 1.5
            for modelo in dados.get('models', []):
                nome = modelo['name']
                metodos = modelo.get('supportedGenerationMethods', [])
                if 'generateContent' in metodos:
                    if 'flash' in nome: return nome
                    if 'gemini-pro' in nome: return nome
    except:
        pass
    return "models/gemini-1.5-flash" # Fallback padrão

# Cache para não ficar perguntando toda hora
NOME_MODELO_ATUAL = None

def gerar_resumo_ia(texto_completo: str):
    global NOME_MODELO_ATUAL
    
    if "COLE_SUA" in API_KEY:
        return "ERRO: A chave da API sumiu! Cole ela de volta no analise.py"

    if not texto_completo or len(texto_completo) < 50:
        return "Texto muito curto para resumir."

    # Configura o modelo na primeira vez
    if NOME_MODELO_ATUAL is None:
        NOME_MODELO_ATUAL = _descobrir_modelo_disponivel()
        print(f"[Python] Usando modelo de IA: {NOME_MODELO_ATUAL}")

    modelo_limpo = NOME_MODELO_ATUAL.replace("models/", "")
    url = f"https://generativelanguage.googleapis.com/v1beta/models/{modelo_limpo}:generateContent?key={API_KEY}"
    headers = {"Content-Type": "application/json"}

    prompt_text = (
        "Você é um assistente de notícias. Resuma o texto a seguir em português, "
        "com 2 a 3 frases fluidas. Ignore legendas e créditos. "
        "Texto:\n\n" + texto_completo
    )

    payload = {"contents": [{"parts": [{"text": prompt_text}]}]}

    try:
        response = requests.post(url, headers=headers, data=json.dumps(payload))
        if response.status_code == 200:
            resultado = response.json()
            if 'candidates' in resultado and resultado['candidates']:
                return resultado['candidates'][0]['content']['parts'][0]['text'].strip()
            else:
                return "A IA bloqueou o conteúdo (Safety Filter)."
        else:
            return f"Erro na API ({response.status_code}): {response.text}"
    except Exception as e:
        return f"Falha de conexão: {str(e)}"

# 3. FUNÇÕES DE PALAVRAS-CHAVE (COMPOSTAS/BIGRAMAS)

def calcular_palavras_chave(texto: str, quantidade: int):
    if not texto: return []
    try:
        # Configura stopwords e adiciona algumas comuns em jornalismo
        lista_stopwords = set(stopwords.words('portuguese'))
        lista_stopwords.update(['disse', 'afirmou', 'segundo', 'sobre', 'após', 'entre', 'para', 'pelo'])
        
        palavras = word_tokenize(texto.lower())
        
        # Filtra palavras simples
        palavras_limpas = [
            p for p in palavras 
            if p not in lista_stopwords and p.isalnum() and len(p) > 2
        ]
        
        # Contagem de Unigramas (palavras soltas)
        contador = Counter(palavras_limpas)
        
        # Contagem de Bigramas (palavras compostas)
        bigramas = zip(palavras_limpas, palavras_limpas[1:])
        bigramas_strings = [" ".join(bg) for bg in bigramas]
        contador_bigramas = Counter(bigramas_strings)
        
        # Estratégia: Junta os Top Unigramas + Top Bigramas (se frequência >= 2)
        top_simples = contador.most_common(quantidade)
        top_compostas = [
            (bg, count) for bg, count in contador_bigramas.most_common(quantidade)
            if count >= 2
        ]
        
        # Combina e ordena tudo pela contagem
        tudo = top_simples + top_compostas
        tudo.sort(key=lambda x: x[1], reverse=True)

        tudo_float = [(k, float(v)) for k, v in tudo]
        
        return tudo_float[:quantidade]

    except Exception as e:
        print(f"Erro NLP: {e}")
        return []