#Install these packages or you might get errors
from bs4 import BeautifulSoup as BS
import requests

#loading code
url = "https://coinmarketcap.com/"
response = requests.get(url)
doc = BS(response.text, "html.parser")

tbody = doc.tbody
trs = tbody.contents

prices = {}

for tr in trs[:10]:
    name, price = tr.contents[2:4]
    fixed_name = name.p.string
    fixed_price = price.span.string
    prices[fixed_name] = fixed_price

print(prices)
