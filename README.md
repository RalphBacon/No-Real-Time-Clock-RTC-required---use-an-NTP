# No Real Time Clock (RTC) required - use an NTP!
Forget using a RTC module, even though they are accurate and cheap. Use an NTP server instead.

## See https://www.youtube.com/ralphbacon video #139
#### (Direct link to video: https://youtu.be/RCDBPsi01_c)

### Sponsored by LCSC Electronics - More Asian Brands, Lower Prices, 4 Hours Ready for Shipping
```
Founded in 2011, after 6-year striving and accumulation of experience, LCSC has become the fastest
developing online store of electronic components in China, serving 530,000 customers and processing
around 2,000 online orders per day by 2017. LCSC has a strong commitment to offering a wide selection
of genuine, high-quality electronic components at best price.
```
#### More Asian Brands Lower Price http://lcsc.com

I need to know the **date & time** for my project - but no **DS3231** RTC (Real Time Clock) is required.

So my current Home Alone project requires the Date and Time to work properly. I immediately ordered a small **RTC shield** for my Wemos D1 mini - only to quickly realise I should not have bothered as I can get the date and time very accurately from a Network Time Protocol server and keep a local, accurate clock that drifts very infrequently thanks to a wonderful library, courtesy of Arduino.cc

And the local clock can be updated every so often by another call to the NTP pool, perhaps once a day or so.

Works like a dream. Even better, should the call to the NTP fail there is no need to panic. Eventually, it will resync and all will be well again.

Here we dive deep into **the simple C++ code** that is required to do all this with the help of a couple of libraries.

In a future video, I'll show you how you can convert the UTC (universal coordinated time) to local time. Easy to do, and works worldwide.

#### Links (these may be affiliated which means that they might help my channel at no cost to you)  

Wemos D1 Mini ESP8266 One is cheap but better value if you get 3 or 5. Also check out the different warehouses, in USA, CN and UK.  
https://www.banggood.com/Wemos-D1-Mini-V3_0_0-WIFI-Internet-Of-Things-Development-Board-Based-ESP8266-4MB-p-1264245.html?p=FQ040729393382015118&utm_campaign=25129675&utm_content=3897

What I'm using for parameter and data, a micro SD card shield:  
https://www.banggood.com/3Pcs-WeMos-Micro-SD-Card-Shield-For-WeMos-D1-Mini-TF-WiFi-ESP8266-Compatible-SD-Wireless-Module-p-1211052.html?p=FQ040729393382015118&utm_campaign=25129675&utm_content=3897

A power shield so you can it from a 7-12v supply (but the 5v phone charger on the ESP8266 board works too):  
https://www.banggood.com/3Pcs-WeMos-DC-Power-Shield-V1_0_0-For-WeMos-D1-Mini-p-1189337.html?p=FQ040729393382015118&utm_campaign=25129675&utm_content=3897

All about the NTP protocol:   
https://tttapa.github.io/ESP8266/Chap15%20-%20NTP.html

List of all ports in use:  
https://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers

More on time zones (I will implement this in my project code):  
https://github.com/JChristensen/Timezone

If you like this video please give it a thumbs up, share it and if you're not already subscribed please consider doing so and joining me on my Arduinite journey!

My channel and blog are here:  
\------------------------------------------------------------------  
https://www.youtube.com/RalphBacon  
https://ralphbacon.blog  
\------------------------------------------------------------------  








