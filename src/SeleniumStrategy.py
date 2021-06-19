from selenium import webdriver

def SeleniumGetPageHTML(url):
    driver = webdriver.Firefox()
    driver.get('http://www.ubuntu.com/')
    print(url)
    sleep(2)
    page_raw = driver.page_source
    return page_raw
