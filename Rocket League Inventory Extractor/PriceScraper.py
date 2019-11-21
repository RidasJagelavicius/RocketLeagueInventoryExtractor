from bs4 import BeautifulSoup as bs # Our BeautifulSoup web-scraper
import requests # Converts the url to a document
import ast # Used to convert from string to dictionary
import json # Used to convert object to JSON for storage and transfer

# This script scrapes rl.insider.gg for Rocket League item prices for PC

#------------------------------------------------------------------------------------#

def main():
    # Create variables to store errors
    isError = False
    errorList = []

    # Store prices
    prices = []

    # Open a connection to the site using HTTP requests
    print('Checking sources')
    try:
        priceSource = requests.get('https://rl.insider.gg/pc')
    except(Exception):
        isError = True
        errorList.append('Error establishing connection to rl.insider.gg')

    # Create BeautifulSoup objects based on each source
    try:
        pricesBs = bs(priceSource.content,'html.parser')
    except(Exception):
        isError = True
        errorList.append('Error creating BeautifulSoup object')
    print('Sources checked and BS objects created')

    if(not isError):
        print('Generating creature lists')
        
        # Create a Python object (dict) of all prices
        for priceTable in pricesBs.find_all(class_='priceTable'):
            for item in priceTable.find_all(class_='itemRow'):
                if('emptyCell' in item['class']):
                    continue;
                table = {}
                table['name'] = item['data-itemfullname'] # Grab item name
                table['type'] = item['data-itemtype'].capitalize() # Grab item type (ex. Topper, Decal)
                rarity_sanitized = item['data-itemrarity'][1:-1].capitalize() # Grab rarity of item (ex. Rare, Limited)
                while('|' in rarity_sanitized):
                    rarity_sanitized = rarity_sanitized[:rarity_sanitized.index('|')]
                table['rarity'] = rarity_sanitized
                painted = {}
                for item_painted in item.find_all(class_='priceRange'):
                    color = item_painted['class']
                    if('invisibleColumn' in color): # Chop some junk off
                        continue
                    color = color[0][5:] # Chop some junk off
                    color_price = item_painted.text 
                    if('&hairsp-&hairsp' in color_price):
                        color_price = color_price.replace('&hairsp-&hairsp','-') # Sanitize scraped hyphens
                    if('\u2014' in color_price):
                        color_price = color_price.replace('\u2014','') # Sanitize scraped hyphens
                    painted[color] = color_price
                table['prices'] = painted
            prices.append(table)
        print("Generated prices")  
        
    #Otherwise there are errors
    else:
        print('Error')
        print(errorList)

    # Convert price object to JSON format
    toJSON = json.dumps(prices)
    
    # Write prices to file
    file = open("Prices.json", "w+")
    file.write(toJSON)
    file.close()
    print("Generated JSON")

main()
