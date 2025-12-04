# analise.py
import requests 
import json
import nltk
from nltk.corpus import stopwords
from nltk.tokenize import word_tokenize
from collections import Counter


API_KEY = "AIzaSyAGVOIPV3bPhx4YKYf5ZKsqqmbox509Aek" 

def configurar_nltk():
    recursos = ['punkt', 'stopwords', 'punkt_tab']
    for r in recursos:
        try:
            nltk.data.find(f'tokenizers/{r}')
        except LookupError:
            nltk.download(r, quiet=True)

configurar_nltk()

def _descobrir_modelo_disponivel():
    """
    Função auxiliar inteligente:
    Consulta a API para ver qual nome de modelo está ativo para sua chave.
    Isso evita o erro 404 se o nome mudar (ex: gemini-pro vs gemini-1.0-pro).
    """
    url_list = f"https://generativelanguage.googleapis.com/v1beta/models?key={API_KEY}"
    try:
        response = requests.get(url_list)
        if response.status_code == 200:
            dados = response.json()
            # Procura por modelos preferidos na lista
            for modelo in dados.get('models', []):
                nome = modelo['name'] # ex: models/gemini-1.5-flash
                metodos = modelo.get('supportedGenerationMethods', [])
                
                # Queremos um modelo que gere texto (generateContent)
                if 'generateContent' in metodos:
                    # Damos preferência ao Flash ou Pro
                    if 'flash' in nome: return nome
                    if 'gemini-pro' in nome: return nome
                    if 'gemini-1.5' in nome: return nome
            
            # Se não achou os preferidos, pega o primeiro que gera texto
            for modelo in dados.get('models', []):
                if 'generateContent' in modelo.get('supportedGenerationMethods', []):
                    return modelo['name']
                    
    except Exception as e:
        print(f"[Aviso] Erro ao listar modelos: {e}")
    
    # Fallback (Se a listagem falhar, tentamos o mais provável)
    return "models/gemini-1.5-flash"

# Variável global para não buscar toda vez (cache)
NOME_MODELO_ATUAL = None

def gerar_resumo_ia(texto_completo: str):
    global NOME_MODELO_ATUAL
    
    if "COLE_SUA" in API_KEY:
        return "ERRO: Cole a chave do Google no arquivo analise.py"

    if not texto_completo or len(texto_completo) < 50:
        return "Texto muito curto para resumir."

    # 1. Descobre o modelo certo na primeira execução
    if NOME_MODELO_ATUAL is None:
        NOME_MODELO_ATUAL = _descobrir_modelo_disponivel()
        print(f"[Python] IA Configurada usando modelo: {NOME_MODELO_ATUAL}")

    # 2. Monta a URL com o modelo descoberto (remove 'models/' se vier duplicado)
    modelo_limpo = NOME_MODELO_ATUAL.replace("models/", "")
    url = f"https://generativelanguage.googleapis.com/v1beta/models/{modelo_limpo}:generateContent?key={API_KEY}"
    
    headers = {"Content-Type": "application/json"}

    prompt_text = (
        "Você é um assistente de notícias. Resuma o texto a seguir em português, "
        "com 2 a 3 frases fluidas. Ignore legendas e créditos. "
        "Texto:\n\n" + texto_completo
    )

    payload = {
        "contents": [{
            "parts": [{"text": prompt_text}]
        }]
    }

    try:
        response = requests.post(url, headers=headers, data=json.dumps(payload))
        
        if response.status_code == 200:
            resultado = response.json()
            if 'candidates' in resultado and resultado['candidates']:
                return resultado['candidates'][0]['content']['parts'][0]['text'].strip()
            else:
                return "A IA não retornou resposta (bloqueio de segurança)."
        else:
            return f"Erro na API ({response.status_code}): {response.text}"
            
    except Exception as e:
        return f"Falha de conexão: {str(e)}"

def calcular_palavras_chave(texto: str, quantidade: int):
    # Função mantida idêntica
    if not texto: return []
    try:
        lista_stopwords = set(stopwords.words('portuguese'))
        palavras = word_tokenize(texto.lower())
        palavras_filtradas = [
            p for p in palavras 
            if p not in lista_stopwords and p.isalnum() and len(p) > 4
        ]
        contador = Counter(palavras_filtradas)
        return contador.most_common(quantidade)
    except:
        return []
