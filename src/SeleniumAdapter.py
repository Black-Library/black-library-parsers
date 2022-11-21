import sys
import time
import pathlib

from selenium import webdriver

def NetworkRequest(url, wait_time =1) :
    driverpath = pathlib.Path(__file__).parent.resolve()
    driverpath = str(driverpath)
    driverpath = driverpath[0:-4] + "/ext/geckodriver"
    
    options = webdriver.FirefoxOptions()
    options.add_argument("--disable-blink-features=AutomationControlled")
    driver = webdriver.Firefox(executable_path=driverpath, options=options)
    # Get all available cookies
    # print(driver.get_cookies())

    driver.get(url)
    try:
        wait_time_int = int(wait_time)
    except ValueError:
        wait_time_int = 1
    time.sleep(wait_time_int)
    page_raw = driver.page_source
    driver.quit()
    return page_raw

