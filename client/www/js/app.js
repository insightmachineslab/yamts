const yamtsUrlBase = 'http://192.168.1.1/things/yamts/properties/'
const deviceScanInterval = 2000
const inputTimeoutMillis = 2000;

function bindSlidersAndValueFields( valueHandler = () => {} ) {
  const inputs = document.getElementsByTagName('input');
  for ( let i=0; i<inputs.length; i++ ) {
    let input = inputs[i];
    if ( input.type === "range" ) {
      let valueTagId = input.id.split('-')[0]+"-value";
      let valueTag = document.getElementById(valueTagId);      
      input.addEventListener('input', () => {
        valueTag.value = input.value
        valueHandler();
      }, false);
      valueTag.value = input.value;
    } else if ( input.type === "number" ) {
      let sliderTagId = input.id.split('-')[0]+"-slider";
      let sliderTag = document.getElementById(sliderTagId);
      let min = sliderTag.min;
      let max = sliderTag.max;
      let timeout = null;
      input.addEventListener('focus', () => {
        input.select();
      });
      input.addEventListener('input', () => {
        if ( timeout ) clearTimeout( timeout )
        disableStartButton()
        timeout = setTimeout( () => {
          let value = Math.max( min, Math.min( input.value, max ) )
          sliderTag.value = value
          input.value = value          
          timeout = null
          enableStartButton()
          valueHandler()
        }, inputTimeoutMillis );
        
      }, false);
    }
  }
  valueHandler();
}

async function setYamtsProperty( propertyName = '', value = '') {
  let data = {}
  data[propertyName] = value;
  const response = await fetch(yamtsUrlBase + propertyName, {
    method: 'PUT',
    cache: 'no-cache',
    body: JSON.stringify(data),
  });
  return await response.json(); // parses JSON response into native JavaScript objects 
}

async function isDeviceAvailable() {
  return new Promise((resolve) => {
      $.ajax( 
        { 
          url: yamtsUrlBase + 'started', 
          type: "GET", 
          timeout: 1000
        } 
      )
      .done(function() {
        console.log( "Found a YAMTS device" );
        resolve(true)
      })
      .fail(function() {
        console.log( "Could not find a YAMTS device" );
        resolve(false)
      })    
  })  
}

function enableStartButtonWhenDeviceIsAvailable() {  
  const deviceScan = () => {
    isDeviceAvailable().then( available => {
      if ( available ) {
        enableStartButton()
        clearMessage()
      } else {
        disableStartButton()
        displayMessage("Searching for device...")
        setTimeout( deviceScan, deviceScanInterval )
      }
    })
  }
  setTimeout( deviceScan, deviceScanInterval )
}

function enableStartButton() {
  $("#startButton").removeClass("disabled") 
}

function disableStartButton() {
  $("#startButton").addClass("disabled") 
}

function displayMessage(text) {
  $("#messageText").text(text)
}

function clearMessage() {
  $("#messageText").text("")
}