from cgitb import text
import requests
import time
from datetime import datetime


def atualiza_dados():
    while 1:
        resposta = requests.get('http://192.168.100.21/')
        if resposta.status_code == 200:
                
            time.sleep(2)
            
            dados = resposta.text
            horas = datetime.now().strftime('%H:%M:%S')
            dados_separados = dados.split("e")

            print(f'\nTemperatura: {dados_separados[1]} °C')
            print(f'Umidade: {dados_separados[0]} %')
            print(f'Horário: {horas}')
        
        else:
            print('Erro ao atualizar dados')
            time.sleep(2)

atualiza_dados()