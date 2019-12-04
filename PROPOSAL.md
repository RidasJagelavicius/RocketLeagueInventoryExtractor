# Project Idea
  The proposed project is to create an application that, given a screen-shot of a player's Rocket League inventory taken from in-game, will:

  1. **Classify each item**
     1. Gather name, quantity, paint (color), certification, class
  1. **Determine the price of each item**
     1. Range of price in keys presented as LOW-HIGH
  1. **Output/Store aggregated data** in a readable format
     1. For pasting on trade forums
     1. For quickly scanning one's inventory
     1. For determining one's total inventory worth
     1. For data analysis

# Background Knowledge / Motivation
  **Background Knowledge**
    I used to trade on Rocket League. I began with trading crates (20 for 1 key) and ended up trading Black Market Decals and Goal Explosions. I rode the early wave of Goal Explosions to sell Electroshock for 55 keys when it first came out. Then I started actively trading: keeping track of prices, posting my items on trade forums, and seeking out profitable deals. I eventually obtained more items than I could manage myself, and created an alternate account and repurchased the game with the intention to one day create a trade bot.

  **Motivation**
  1. I'm fueled by greed to become Rocket-League-rich
  1. I know the resources for looking up prices and seeing what's HOT
  1. I already have another account, with the game purchased, that was intended to serve as a trade bot.
     1. This project could be the precursor to an automated trade bot

# Proposed Libraries for Use
  **Image Recognition / Processing**
  1. *[OpenCV](https://opencv.org/)*
     1. A computer-vision library
     1. OCR can be used to read images
     1. More options than ofxCV or ofxOpenCV

  **GUI**
  1. *[ofxDatGui](http://github.com/braitsch/ofxDatGui)*
     1. Allegedly simple to use, fully-customizable, high-resolution GUI
  1. *[ofxGuiExtended](http://github.com/frauzufall/ofxGuiExtended) (Maybe)*
     1. GUI derived from ofxGui

# Timeline
  **Week 1**
  1. Set up OpenCV to work with Visual Studio, begin a prototype to see if project is possible
     1. Create Python script to grab prices of objects from  [RL Insider](https://rl.insider.gg/)

  **Week 2/3**
  1. Finish setting up OpenCV; Utilize OpenCV/OCR to classify an image and its name, class, certification, paint, and quantity
     1. Create an object based on this data
     1. Set up API to grab item prices from the database stored as a JSON file

  **Week 4**
  1. Create an inventory object and implement API
     1. Search functions, GetAllInClass, GetHottestItems, etc.
     1. (If time permits) Output to file in text format (maybe JSON for storage, maybe pretty-print for readability)

# Stretch Goals
  **GUI**
  1. Support searching for items
  1. Support filtering by class, cert, paint, etc.
  1. Support setting favorite lists (hot items, all painted, in demand, black market, must go)

  **Autonomy**
  1. Implement the ability to launch the game and generate screen-shots of a full inventory
  1. Implement the automatic classification of a full inventory based on multiple screen-shots
  1. Implement a bot to advertise on trade forums automatically
     1. Implement an alert on friend-request / trade-request

  **Other**
  1. Implement storing data to file and output to GUI for readability
      1. GUI should have options to generate something to paste in trade forums
  1. GUI should have option to output tables to easily scan inventory
