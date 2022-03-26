document.addEventListener('DOMContentLoaded', function () {
    const XHRURL = "/raw_adc";

    // id=button_door_up ==> value  von id=door_up
    // id=button_door_mid ==> value  von id=door_mid
    // id=button_door_down ==> value  von id=door_down

// *** genarl fuction to read and set values ***
    function get_door_value(UpMidDown) {
        if (UpMidDown=='door_up' || UpMidDown=='door_mid' || UpMidDown=='door_down') {
            var ret = document.querySelector('#'+UpMidDown).value
            var request = new XMLHttpRequest();
            request.open("GET", XHRURL);
            request.addEventListener('load', function(event) {
                if (request.status >= 200 && request.status < 300) {
                    console.log(request.responseText);
                    ret = request.responseText;
                } else {
                    //console.warn(request.statusText, request.responseText);
                }
            document.querySelector('#' + UpMidDown).value = ret;
            });
            request.send();
        }
    }
    
// *** Request and set value for DOOR_UP ***
    function get_door_up_value() {
        get_door_value('door_up');
    }
// *** Request and set value for DOOR_MID ***
    function get_door_mid_value() {
        get_door_value('door_mid');
    }
// *** Request and set value for DOOR_DOWN ***
    function get_door_down_value() {
        get_door_value('door_down');
    }

// *** Set Button event Handlers ***    
    document.querySelector('#button_door_up').addEventListener('click', get_door_up_value);
    document.querySelector('#button_door_mid').addEventListener('click', get_door_mid_value);
    document.querySelector('#button_door_down').addEventListener('click', get_door_down_value);

});