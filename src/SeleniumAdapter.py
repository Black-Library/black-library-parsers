import sys
import time
import pathlib

from selenium import webdriver

def NetworkRequest(url) :
    driverpath = pathlib.Path(__file__).parent.resolve()
    driverpath = str(driverpath)
    driverpath = driverpath[0:-4] + "/ext/geckodriver"
    driver = webdriver.Firefox(executable_path=driverpath)
    driver.get(url)
    time.sleep(1)
    page_raw = driver.page_source
    driver.quit()
    return page_raw

