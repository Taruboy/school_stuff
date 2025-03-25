import requests as req
from bs4 import BeautifulSoup

date = 20250325
url = f"http://www.ulsan-hs.hs.kr/ulsan-hs-h/M01030702/list?ymd={date}";
res = req.get(url)

soup = BeautifulSoup(res.text, 'html.parser')
data = soup.select('dd.tch-lnc > ul > li')

for li in data:
    print(li.string)
