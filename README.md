# YTWATCH

Fetch youtube url from your subscription

# INSTALL

 - clone the repo:

  ```
  git clone https://github.com/FrancescoDeSimone/ytwatch
  ```

 - compile:

  ```
  make -j$(nproc)
  ```

# USAGE

 - Download YouTube subscription csv:
   - got here [Google Takedown](https://takeout.google.com/takeout/custom/youtube)
   - Log in (for the last time I hope)
   - Click on "All data included", then on "Deselect all", then select only "subscriptions" and click "OK"
   - Click on "Next step" and then on "Create export"
   - Click on the "Download" button after it appears
   - From the downloaded zip extract the .csv in the subscriptions folder
 - Run the application passing the csv (or save the csv in the default path ```XDG_CONFIG_HOME/ytwatch/subscriptions.csv```)

  ```ytwatch [-p] subscriptions.csv```

# Dependency

* curl
* fzf
* ueberzug
* fmt

# Configuration

Ytwatch script use the current env variable

|      ENV                |                     DESCRIPTION                       |
|-------------------------|-------------------------------------------------------|
|YTWATCH_SUBSCRIPTIONS_CSV|CSV subscriptions path                                 |
|YTWATCH_PLAYER           |Video player (mpv default)                             |
|YTWATCH_PLAYER_OPT       |Video player options                                   |
|YTWATCH_SWALLOWER        |Swallow program (empty string for none, default devour)|
|YTWATCH_SCRAPER          |Ytscraper path (current dir by default)                |
