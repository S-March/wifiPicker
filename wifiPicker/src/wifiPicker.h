void setDebugPrint(bool ONOFF);
void debugPrint(String PRINT);
bool attemptAutoConnect(void);
bool attemptManualConnect(const char* NEW_SSID, const char* NEW_PW);
bool wifiPicker(const char* AP_SSID, const char* AP_PW);
void handleWifiPickerRoot();
void handleWifiPickerDone();
void handleWifiPickerNotFound();
String wifiScan(void);

static const char PROGMEM WIFIPICKER_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html>

<head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0, user-scalable=no">
    <meta name="apple-mobile-web-app-capable" content="yes">
    <style>
        html {
            width: 100vw;
            overflow-y: auto;
            overflow-x: hidden;
        }

        body {
            width: 100vw;
            padding: 0;
            margin: auto;
            background-color: white;
        }

        input {
            -webkit-appearance: none;
        }

        h1 {
            font-family: Helvetica;
            font-size: 6vw;
            color: #4e4e56;
            margin: 0;
            text-align: center;
        }

        h2 {
            font-family: Helvetica;
            font-size: 3vw;
            color: #4e4e56;
            margin: 0;
            text-align: center;
        }

        a {
            color: inherit;
            text-decoration: none;
        }

        a:hover {
            color: inherit;
            text-decoration: none;
            cursor: pointer;
        }

        h1.homeContentSectionHeader {
            font-size: 6vw;
            color: white;
        }

        h1.homeIntroHeader {
            font-size: 8vh;
        }

        h2.homeIntroHeader {
            font-size: 4vh;
        }

        .homeContainer {
            width: 80vw;
            padding: 0;
            margin: auto;
            background-color: white;
        }

        .topBar {
            position: relative;
            top: 0;
            left: 0;
            margin: 0;
            padding: 0;
            height: 10vh;
            width: 100%;
            background-color: #DA635D;
        }

        ::-webkit-input-placeholder {
            text-transform: uppercase;
            border-style: none;
            display: block;
            color: #4e4e56;
            font-family: Helvetica;
            font-size: 4vh;
            font-weight: bold;
            margin: 0;
            padding: 0;
            text-align: center;
            vertical-align: middle;
            align-items: center;
            justify-content: center;
            line-height: 5vh;
        }

        .bottomBar {
            position: relative;
            bottom: 0;
            left: 0;
            margin: 0;
            margin-top: 10vh;
            padding: 0;
            height: 10vh;
            width: 100%;
            background-color: #DA635D;
        }

        .homeIntro {
            display: flex;
            flex-direction: column;
            position: relative;
            overflow-y: auto;
            left: 0;
            top: 0;
            width: 100%;
            min-height: 30vh;
            background-color: white;
            margin-bottom: 1.8vh;
            text-align: center;
            vertical-align: middle;
            align-items: center;
            justify-content: center;
        }

        #networksContainer {
            position: relative;
            left: 0;
            top: 0;
            display: block;
            width: 100%;
        }

        .homeContentSection {
            position: relative;
            left: 0;
            top: 0;
            display: block;
            width: 100%;
            height: 15vh;
            margin-bottom: 1.2vh;
            background-color: white;
        }


        .homeContentSectionSmall {
            display: inline-flex;
            overflow: hidden;
            width: 100%;
            height: 100%;
            vertical-align: middle;
            align-items: center;
            justify-content: center;
            background-color: #4e4e56;
        }

    </style>
    <script>
        var wsMessageArray = "";

        function websocketBegin() {
            websock = new WebSocket('ws://' + window.location.hostname + ':81/');
            websock.onopen = function(evt) {
                console.log('websock open');
                websock.send("READYFORNETWORKS");
            };
            websock.onclose = function(evt) {
                console.log('websock close');
            };
            websock.onerror = function(evt) {
                console.log(evt);
            };
            websock.onmessage = function(evt) {
                console.log(evt);
                wsMessageArray = evt.data.split("\n");
                for (var numberOfNetworks = 0; numberOfNetworks < (wsMessageArray.length - 1); numberOfNetworks++) {
                    buildButton(wsMessageArray[numberOfNetworks]);
                }
            };
        }

        function buildButton(x) {
            var newButton = "";
            networkList = document.getElementById("networksContainer");
            newButton += "<div class=\"homeContentSection\" onmouseover=\"fadeBackground(this)\"";
            newButton += "onmouseout=\"unFadeBackground(this)\" onclick=\"requestPassword('";
            newButton += x;
            newButton += "')\"><div class=\"homeContentSectionSmall\"><h1 ";
            newButton += "class=\"homeContentSectionHeader\">"
            newButton += x;
            newButton += "</h1></div></div>";
            networkList.innerHTML += newButton;
        }

        function fadeBackground(x) {
            x.style.opacity = 0.5;
        }

        function unFadeBackground(x) {
            x.style.opacity = 1;
        }

        function requestPassword(SSID) {
            var password = prompt("Please enter the password", "PASSWORD123");
            if (password != null) {
                var url = window.location.href;
                url += "DONE?SSID=" + SSID + "&PW=" + password;
                location.href = url;
            }
        }

    </script>
</head>

<body onload="websocketBegin()">
    <div class="topBar">
    </div>
    <div class="homeContainer">
        <div class="homeIntro">
            <div>
                <h1 class="homeIntroHeader">
                    WiFi Picker
                </h1>
            </div>
            <div>
                <h2 class="homeIntroHeader">
                    Select your desired network from below
                </h2>
            </div>
        </div>
        <div id="networksContainer">
        </div>
    </div>
    <div class="bottomBar">
    </div>
</body>

</html>
)rawliteral";
