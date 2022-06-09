import requests
import time
from datetime import datetime


def atualiza_dados():
    while 1:
        resposta = requests.get('http://192.168.100.21/')
        if resposta.status_code == 200:
            
            dados = resposta.json()
            horas = datetime.now().strftime('%H:%M:%S')
            print(f'\nHorário: {horas}')
            print(f'Temperatura: {dados["temperatura"]} °C')
            print(f'Umidade: {dados["umidade"]} %')
            print(f'Agua: {dados["agua"]} ')


            with open('dados.txt', 'a') as arquivo:
                arquivo.write(f'{horas} - {dados["temperatura"]} - {dados["umidade"]} - {dados["agua"]}\n')

            time.sleep(60)
        
        else:
            print('Erro ao atualizar dados')
            time.sleep(60)

atualiza_dados()
print('Testando Branch')