import sys
import time

from selenium import webdriver

def NetworkRequest(url) :
    driver = webdriver.Firefox(executable_path="../ext/geckodriver")
    driver.get(url)
    time.sleep(1)
    page_raw = driver.page_source
    driver.quit()
    print(page_raw)
    return page_raw

try:
    url = sys.argv[1]
    NetworkRequest(url)
except IndexError:
    raise Exception("Please provide url")
except KeyError:
    raise Exception("Function {} hasn't been found".format(function))
