from binance.client import Client
from binance.websockets import BinanceSocketManager
import dateparser
from time import time
from twisted.internet import reactor
import pandas as pd
from datetime import datetime
from datetime import date
import copy

class BinanceWebsocket():
    def __init__(self):
        self.DEPTH = BinanceSocketManager.WEBSOCKET_DEPTH_5
        self.CURRENT_DATE = date.today()
        self.DIRECTORY = './data/'
        self.CURRENCIES = [
            {'currency': 'LTCBTC', 'file': None}, 
            {'currency': 'BTCUSDT', 'file': None},
            {'currency': 'LTCUSDT', 'file': None}
        ]
    
    
    def calculate_files(self):
        # todo - přepsat 
        tmp_list = self.CURRENCIES
        for i in range(len(self.CURRENCIES)):
            self.CURRENCIES[i]['file'] = open(self.DIRECTORY + self.CURRENCIES[i]['currency'] + "-" + str(self.CURRENT_DATE),"a+")

            
    def make_lambda(self, curr):
        return lambda msg: self.process_message(msg, curr)
    
    
    def update_current_date(self):
        self.CURRENT_DATE = date.today()


    def process_message(self, msg, currency=None):
        if currency is None:
            return None
        try:
            msg['timestamp'] = time()
            if not self.CURRENT_DATE == date.today():
                self.update_current_date()
                self.calculate_files()
            for item in msg.values():
                currency['file'].write(str(item) + ";")
            currency['file'].write("\n")
        except Exception as e:
            print(e)
        return None


    def startClient(self):
        api_key = 'aKNYXtMDVEsVyJCMNwcLGpAG9KVK9d5V568QCRADjDn5kumLeN9joKhnA4zIpBPd'
        api_secret = 'ziY288jGcFEdOv81RoWrAbAUf08LKwxpJ2EmKt1M2pHlhcB9UCaciGXHeIcZVFCZ'
        return Client(api_key, api_secret)


    def startDepthSocketManager(self, client):
        bm = BinanceSocketManager(client)
        queries = []
        for currency in self.CURRENCIES:
            queries.append(bm.start_depth_socket(currency['currency'], self.make_lambda(currency), depth=self.DEPTH))
        bm.start()
        return bm, queries

try:
    binance = BinanceWebsocket() 
    binance.calculate_files()
    client = binance.startClient()
    socket_manager, queries = binance.startDepthSocketManager(client)
except Exception as e:
    print(e)
    