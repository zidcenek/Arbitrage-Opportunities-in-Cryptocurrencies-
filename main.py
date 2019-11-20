from binance.client import Client
from binance.websockets import BinanceSocketManager
import dateparser

def process_message(msg):
    try:
        print(msg)
    except:
        print('e')
    return None
    
def startClient():
    api_key = 'aKNYXtMDVEsVyJCMNwcLGpAG9KVK9d5V568QCRADjDn5kumLeN9joKhnA4zIpBPd'
    api_secret = 'ziY288jGcFEdOv81RoWrAbAUf08LKwxpJ2EmKt1M2pHlhcB9UCaciGXHeIcZVFCZ'
    return Client(api_key, api_secret)

def startDepthSocketManager(client):
    bm = BinanceSocketManager(client)
    depth = bm.start_depth_socket('BNBBTC', process_message, depth=BinanceSocketManager.WEBSOCKET_DEPTH_5)
    bm.start()
    
client = startClient()
startDepthSocketManager(client)
