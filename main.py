from binance.client import Client
from binance.websockets import BinanceSocketManager
import dateparser
from time import time
from twisted.internet import reactor
import pandas as pd
from datetime import datetime
from datetime import date
import copy

DEPTH = BinanceSocketManager.WEBSOCKET_DEPTH_5
CURRENT_DATE = date.today()
DIRECTORY = './data/'
CURRENCIES = [
    {'currency': 'LTCBTC', 'file': None}, 
    {'currency': 'BTCUSDT', 'file': None},
    {'currency': 'LTCUSDT', 'file': None}
]


def calculate_files(currencies):
    tmp_list = []
    for dct in currencies:
        tmp = {}
        tmp['currency'] = dct['currency']
        tmp['file'] = open(DIRECTORY + tmp['currency'] + "-" + str(CURRENT_DATE),"a+")
        tmp_list.append(tmp)
    currencies, tmp_list = tmp_list, currencies
    for dct in tmp_list:
        if dct['file'] is not None:
            print('wrong')
            dct['file'].close()
    return currencies


def make_lambda(curr):
    return lambda msg: process_message(msg, curr)


def update_current_date():
    CURRENT_DATE = date.today()


def process_message(msg, currency=None):
    if currency is None:
        return None
    try:
#         print(CURRENT_DATE)
#         print(currency['currency'])
        msg['timestamp'] = time()
        if not CURRENT_DATE == date.today():
            update_current_date()
            CURRENCIES = calculate_files(CURRENCIES)
        for item in msg.values():
            currency['file'].write(str(item) + ";")
        currency['file'].write("\n")
    except Exception as e:
        print(e)
    return None
    
    
def startClient():
    api_key = 'aKNYXtMDVEsVyJCMNwcLGpAG9KVK9d5V568QCRADjDn5kumLeN9joKhnA4zIpBPd'
    api_secret = 'ziY288jGcFEdOv81RoWrAbAUf08LKwxpJ2EmKt1M2pHlhcB9UCaciGXHeIcZVFCZ'
    return Client(api_key, api_secret)




def startDepthSocketManager(client):
    bm = BinanceSocketManager(client)
    queries = []
    for currency in CURRENCIES:
        queries.append(bm.start_depth_socket(currency['currency'], make_lambda(currency), depth=DEPTH))
    bm.start()
    return bm, queries


try:
    CURRENCIES = calculate_files(CURRENCIES)
    client = startClient()
    socket_manager, queries = startDepthSocketManager(client)
except Exception as e:
    display(e)
