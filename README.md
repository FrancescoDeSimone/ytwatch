<div align="center">
  <img src="./ytwatch.png" width="450" />
</div>

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
- run ./ytwatch

Alternative build the appimage with the script:
```
./build_appimage.sh
```

# USAGE

 - Download YouTube subscription csv:
   - got here [Google Takedown](https://takeout.google.com/takeout/custom/youtube)
   - Log in (for the last time I hope)
   - Click on "All data included", then on "Deselect all", then select only "subscriptions" and click "OK"
   - Click on "Next step" and then on "Create export"
   - Click on the "Download" button after it appears
   - From the downloaded zip extract the .csv in the subscriptions folder
 - Save the csv in the default path ```XDG_CONFIG_HOME/ytwatch/subscriptions.csv``` or specify a path with the environment variable

  ```ytwatch [-p] {channel name}```

# Dependency

* curl
* fzf
* ueberzug
* fmt
* devour (optional)

# Configuration

Ytwatch script use the current env variable

|Environment Variable|Description|Default Value
---------------------|-----------|------------
|YTWATCH_SUBSCRIPTIONS_CSV|Path to the subscriptions CSV file|$XDG_CONFIG_HOME/ytwatch/subscriptions.csv
|YTWATCH_PLAYER|Video player|mpv
|YTWATCH_PLAYER_OPT|Video player options|"--playlist=-"
|YTWATCH_SWALLOWER|Swallowed program|	devour
|YTWATCH_SCRAPER|Ytscraper path|	"./ytscraper"
|YTWATCH_UEBERZUGPP|Path to UeberzugPP binary|	/usr/local/bin/ueberzugpp
|YTWATCH_FZF|Path to FZF binary|	fzf
