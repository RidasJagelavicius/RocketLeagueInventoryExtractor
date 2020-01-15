# Rocket League Inventory Extractor by Ridas Jagelavicius
Rocket League Inventory Extractor generates an Inventory by extracting an item's traits (color, certification, name) from an image of a Rocket League item using OpenCV and Tesseract. Inventory API allows users to view the current value of the user's full inventory, or display the inventory in a readable format for quick analysis or for trade purposes.

## Description
### Background
Rocket League is a popular video-game created by Psyonix and described simply as "soccer but with cars".
The game features lootboxes and random drops that the community can trade with other players or trade-up in-game.
As of the time of this project's creation, the active currency used by traders were Keys.
Keys gain their value from having to be purchased with real currency in order to open lootboxes for a chance to obtain rare or interesting items.
*[RL Insider](https://rl.insider.gg)* is a popular website used by the trading community to gauge item value in keys.

Rocket League neither has a built-in system of evaluating the total price of a user's inventory, nor an efficient way to view an inventory once it has become massive. Currently, a user would have to lookup and calculate the price of each item in their inventory to estimate their inventory value, and scroll through potentially hundreds of images to grasp the physical makeup of their Rocket League inventory. This program serves to simplify both price evaluation and inventory analysis.

### Features
This project features *four* different APIs that work in conjunction.
1. InventoryItem
   1. Represents an actual Rocket League Inventory Item
   1. Can store and retrieve item name, certification, paint, rarity, tradability, type, quantity, and price
1. Inventory
   1. Represents a Rocket League Inventory (composed of multiple InventoryItems)
   1. Can add and remove items, update a particular item's price, and most importantly, pretty print the inventory or "buy" or "sell" lists
   1. Can be saved and loaded without reclassifying all items
1. ItemDatabase
   1. Can be queried for detailed information on an item
   1. Can return the price of an item of a particular color, an item's rarity, its type, or it's full name
   1. Relies on a particularly formatted JSON (See Prices.json)
1. ItemClassifier
   1. Extracts and manipulates the text from an image of a single inventory item
   1. Can detect the text in the image and extract it, or extract the item color, certification, or full name
   1. Accuracy varies due to the presentation of inventory items and extraction implementation
These can be used in unison to generate an inventory from images of rocket league items and create a condensed and readable output or to print lists of desired items and their prices.

## Visuals
### Item Classification
Given this image:

![](https://i.imgur.com/tPO6yzt.png)

The ItemClassifier was able to detect this text:

![](https://i.imgur.com/BD9DjxA.png)

The ItemClassifier was able to extract this text:
> SIENNA
> BURNT
> FGSP

The ItemClassifier was also able to extract the following information upon running:
> Certification = ""
> Color = "Burnt Sienna"
> Name = "FGSP"
> Current Price = "0.33-1"

### Inventory Output
After classifying a few images and creating an Inventory of generated InventoryItems,
**Inventory::PrettyPrint()** produced this output:
>==============================  Wheels  ==============================
>
> Burnt Sienna  FGSP (1) 0.33-1
>
> Saffron  Spiralis (1) 0.5-1
>
>==============================  Toppers  ==============================
>
> Cobalt  Wildcat Ears (1) 1-1.5
>
>==============================  Cars  ==============================
>
> Default  Dominus GT (1) 0.75-1.5
>
>==============================  Decals  ==============================
>
>  Default  Octane - MG-88 (1) 0.5-1

And **Inventory::PrintSellingList()** produced this output:
>                              SELLING ITEMS
>==============================  Wheels  ==============================
>
>H: Burnt Sienna  FGSP
>   W: 1k or offer
>
>H: Saffron  Spiralis
>   W: 1k or offer
>
>==============================  Toppers  ==============================
>
>H: Cobalt  Wildcat Ears
>   W: 2k or offer
>
>==============================  Cars  ==============================
>
>H: Default  Dominus GT
>   W: 2k or offer
>
>==============================  Decals  ==============================
>
>H: Default  Octane - MG-88
>   W: 1k or offer

## Installation
This project has the following dependencies:
1. OpenCV (confirmed working on 4.1.2)
1. OpenCV DNN Module (confirmed working on 4.1.2)
1. Tesseract trained with tessdata-best
1. jsoncpp

OpenCV can be difficult to setup due to the requirement of the DNN module.
I found the following two videos very useful:
1. *[Build and Install OpenCV 4 on Windows 10 | Adam Hacks](https://www.youtube.com/watch?v=x5EWlNQ6z5w&)*
1. *[Create an OpenCV 4 Project in Visual Studio [C++] | Adam Hacks](https://www.youtube.com/watch?v=p-6rG6Zgu4U&)*

For installing and setting up Tesseract and jsoncpp, I suggest using *[vcpkg](https://vcpkg.readthedocs.io/en/latest/)* as it is a command-prompt package installer, like those shipped in linux distributions

This was compiled on Visual Studio 2019

## Usage
The provided main illustrates the general setup of a project.
1. Download the *[EAST Text Detection Model](https://github.com/oyyd/frozen_east_text_detection.pb)* or a similar text detection model, and create a JSON like that of Prices.json. You may update or amend PriceScraper.py for this.
1. Extract text from images using the ItemClassifier
   1. Populate a vector of strings that are full paths to images
   1. Initialize an ItemClassifier with the path to the model and the path to the database json
   1. For each element in the vector of images, pass the image to **ItemClassifier::DetectText(image_path)**
   1. Extract the text with **ItemClassifier::ExtractText()**
1. Manipulate the extracted text to gain more data using ItemClassifier and ItemDatabase
   1. Extract item certification using **ItemClassifier::ExtractCertification(text extracted from 2.4)**
   1. Extract item paint color using **ItemClassifer::ExtractColor(text extracted from 2.4)**
   1. Extract full item name using **ItemClassifier::MatchTextToItemName(text extracted from 2.4)**
   1. Obtain item price using **ItemDatabase::GetPriceOf(item name from 2.3, item paint color from 3.2)**
1. Create an InventoryItem using the data extracted from step 3
   1. **InventoryItem item(name from 3.3, certification from 3.1, paint color from 3.2, item price from 3.4)** is sufficient
1. Create an Inventory and add the InventoryItem to the Inventory
   1. Initialize an Inventory object and pass the path to the database
   1. Add the InventoryItem to the Inventory using **Inventory::AddItem(instance of InventoryItem)**
1. Print desired list
   1. **Inventory::PrettyPrint()** will generate a list of each item by type, outputting the color, certification, name, quantity, and price range of each item
   1. **Inventory::PrintSellingList()** will generate a list similar to that of PrettyPrint(), but with the additional header "SELLING ITEMS" and list the items as what you have (H:) and the upper bound of the item's price range as what you want (W:) in keys (k)
   1. **Inventory::PrintBuyingList()** generates a list similar to PrintSellingList() but with the header "BUYING ITEMS" and the lower bound of an item's price in keys rounded down ***NOTE: This may result in an output of "W: 0k"***
1. Save Inventory to prevent long run-times of reclassification from images
   1. **Inventory::WriteInvToFile()** creates a *saved* folder and an *inventory.txt* within that folder that can be loaded in at a later time to have access to an Inventory without having to generate the Inventory by classifying a series of images again. This is especially useful for larger inventories. This function returns true if successful.
   1. **Inventory::ReadInvFromFile()** can be used to load in a saved Inventory. Simply create an Inventory object and call the function. If the process was successful (*inventory.txt* exists and could be read) then the function returns true.

## Contact
If you've got questions or suggestions, I can be reached at:
**RidasJagelavicius@gmail.com**

## Important Notes
1. The ItemClassifier may not be able to classify all items with 100% accuracy. This is being improved. Suggestions are also appreciated.
1. Rocket League is currently undergoing a shift in currency from *keys* to *credits*. Once the prices have been fully converted, more improvements will be made.

## License
*[MIT](https://choosealicense.com/licenses/mit/)*
