# Building
* Download Steinberg VST3 and ASIO SDKs from https://www.steinberg.net/en/company/developers.html
* Place the contents of the SDKs into their respective folders under `lib`
* Open one of the plugin projects in Visual Studio (tested with 2017, but earlier versions may work) and build or run
  * The `<plugin>-app` project runs the plugin as a standalone application using the ASIO SDK for audio
  * The `<plugin>-vst2` project opens the `reaper-test.rpp` project in REAPER with the plugin loaded. If you don't have REAPER installed,
  you can grab it from http://reaper.fm/download.php (or just build the plugin and open it in another host).
  
# Plugins
* DigitalDistortion is a simple digital clipping plugin based on the first part of the wonderful tutorial series from Martin Finke 
(http://martin-finke.de/blog/articles/)
* Synthesis is a subtractive synthesizer which supports multiple types of waveforms, multiple voices, and modulation. It is also based
on the tutorial series from Martin Finke.
