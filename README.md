# YTWATCH

Fetch youtube url from your subscription

# INSTALL

 - clone the repo:

  ```git clone https://github.com/FrancescoDeSimone/ytwatch```

 - compile:

  ```make -j$(nproc)```

# USAGE

 - Download YouTube subscription json:
   - got here [Google Takedown](https://takeout.google.com/takeout/custom/youtube)
   - Log in (for the last time I hope)
   - Click on "All data included", then on "Deselect all", then select only "subscriptions" and click "OK"
   - Click on "Next step" and then on "Create export"
   - Click on the "Download" button after it appears
   - From the downloaded zip extract the .json in the subscriptions folder
 - Run the application passing the json

  ```ytwatch subscription.json```

