Info
==========
This the code for a custom PCB that shows in real time what TriMet MAX stations have a train at them. Details regarding the board can be found <a href="https://github.com/HailStorm32/Portland-Transit-Tracker_PCB">here</a>.

For those looking for how to flash their boards, please see below.

## Setup:

<br>

### Setting up the IDE
<ol>
  <li>Install <a href="https://www.arduino.cc/en/software">Arduino IDE</a></li>
  <li>Add the ablity to install the ESP32 board info</li>
    <ol>
      <li>Open the Arduino IDE and open the prefrences window <code>File->Prefrences</code></li>
      <li>Paste <code>https://dl.espressif.com/dl/package_esp32_index.json</code> into the <code>Additinal Board Manager URLs</code> section</li>
      <img src="https://i.imgur.com/czT0oRL.png" style="width:400px;height:258px;"></img>
      <li>Close the IDE and reopen it</li>
    </ol>
  <li>Install the board info for the ESP32</li>
    <ol>
      <li>Open the board manager <code>Tools->Board->Board Manager</code></li>
      <li>Search for <code>esp32</code> and install the latest version</li>
      <img src="https://i.imgur.com/iXe5yis.png" style="width:400px;height:258px;"></img>
    </ol>
  <li>Select the target board <code>Tools->Board->ESP32 Arduino->DOIT ESP32 DEVKIT V1</code></li>
  <li>Install the NeoPixel library</li>
    <ol>
      <li>Open the library manager <code>Sketch->Include Library->Manage Librarys</code></li>
      <li>Search for <code>NeoPixel</code> and install the latest version</li>
      <img src="https://i.imgur.com/Wmp3QsP.png" style="width:400px;height:258px;"></img>
    </ol>
</ol>

<br>

### Setting up the code:
<ol>
  <li>Download this repository as a zip</li>
   <i>For those that know how to do so, you can also clone the repository</i>
    <ol>
      <li>See below photo</li>
      <img src="https://i.imgur.com/Zp1LY9E.png" ></img>
    </ol>
  <li>Unzip the zip folder to <code>C:\Users\[USR-NAME]\Documents\Arduino</code></li>
  <li>Open the IDE and open the <code>TransitTracker.ino</code> 
    <ol>
      <li><code>File->Open</code> and navigate to the folder <code>C:\Users\[USR-NAME]\Documents\Arduino\Portland-Transit-Tracker_Code-master\TransitTracker</code></li>
      <li>Click on the file called <code>TransitTracker.ino</code> and click <code>Open</code></li>
    </ol>
  <li>Once the code is open, click the little down arrow at the top right and click <code>New Tab</code></li>
  <img src="https://i.imgur.com/vmC6uKa.png" ></img>
  <li>Name the tab <code>config.h</code></li>
  <li>Go to the tab that was just created, and paste in the following code</li>
  <pre><code>
    const char* WIFI_SSID = "WIFI-SSID-HERE";
    const char* WIFI_PSWD = "WIFI-PASSWORD-HERE";
    const char* API_KEY = "API-KEY-HERE";
  </code></pre>
  <li>Replace <code>WIFI-SSID-HERE</code> with the name of your WIFI network name</li>
  <li>Replace <code>WIFI-PASSWORD-HERE</code> with your WIFI password</li>
  <li>Replace <code>API-KEY-HERE</code> the API key you got earlier</li>
  <li>Your code should look similar to this</li>
  <pre><code>
    const char* WIFI_SSID = "WifiName";
    const char* WIFI_PSWD = "Password123";
    const char* API_KEY = "ABCDEF1234";
  </code></pre>
  <li>Now save by clicking the check mark in the top left or by hitting <code>CTRL+S</code></li>
</ol>

<br>

### Uploading the code:
<ol>
  <li>Plug the ESP32 into the computer</li>
  <li>Open the IDE and select the proper COM port <code>Tools->Port</code></li>
    <ol>
      <li>If you have multiple options to pick from, you can disconnect the ESP32 and re-open the menu; the entry that disappears should be the ESP32. Reconnect the board and select that serial port.</li>
    </ol>
  <li>Click the upload button (shown below)</li>
  <img src="https://i.imgur.com/2ha3b5o.png"></img>
  <li>A message like below will indicate a successful flash</li>
  <img src="https://i.imgur.com/xM6FPdj.png" ></img>
  <li>Unplug the ESP32 from the computer and remove the cable from the ESP32</li>
</ol>

<br>

### Starting the board:
<ol>
  <li>Follow the instructions that came with the board</li>
    <ol>
      <li>Back up instructions can be found LINK TBD</li>
    </ol>
</ol>
  
  
  
      
