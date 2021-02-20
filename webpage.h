const char webpage_gz[] PROGMEM = R"(
<html lang="en">
  <head>
    <title>Rocket control</title>
    <meta charset="utf-8">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.0-beta2/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-BmbxuPwQa2lc/FVzBcNJ7UAyJxM6wuqIj61tLrc4wSX0szH/Ev+nYRRuWlolflfl" crossorigin="anonymous">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
div.trace {
  display: inline-block;
  margin: 0 10 10 0;
  width: 150px;
}
div.response {
  border: 1px solid #bbb;
  min-height: 40px;
  margin: 5;
  padding: 4;
  width: calc(100vw - 10);
}
    </style>
  </head>
  <body>
    <div class="container">
      <div class="row">    
        <h1>Rocket control</h1>
        <div id="connection" />
      </div>
      <br />
      <div class="row">    
        <div id="connection" class="status"></div>
      </div>
      <div class="row">  
        <div>
          <div class="btn-group" role="group" arial-label="Buttons" id="buttons">
          </div>
        </div>  
      </div>
      <br />
      <br />
      <div class="row">    
        <div id="traces">    
        </div>
      </div>
      <div class="row" id="response">    
      </div>
    <script>
var trace_name_to_number = {}
var trace_name_to_value = {}

// Function called to click buttons
function handleClickButton(name) {
    xhr.open('GET', server + name, false);
    xhr.onreadystatechange = function () {
        if (xhr.readyState == 4) {
            if (xhr.status == 200) {
                var data = xhr.responseText;
                let responseDiv = document.getElementById("response");
                responseDiv.innerHTML = "<span>" + data + "</span>";
            }
        }
    };
    xhr.send(null);
}

// Function called to handle event stream errors
function handleSSEerror(err) {
  console.error("SSE error occurred.")
  console.error(err);

	let condiv = document.getElementById("connection");
	condiv.innerHTML = '<span class="badge bg-danger">Disonnected</span>';
}


var server = "http://192.168.1.253:80/"
const evtSource = new EventSource(server);

// Create a handler called when the SSE connection is made
evtSource.onopen = function(m) {
	let condiv = document.getElementById("connection");
	condiv.innerHTML = '<span class="badge bg-success">Connected</span>';
	for (let i = 0; i < 32; ++i) {
		let tracediv = document.getElementById("card-trace" + i);
		if (tracediv) tracediv.remove();
	}
}

// Create a handler for connection errors
evtSource.onerror = handleSSEerror;

// Create handlers for each trace
for (let i = 0; i < 32; ++i) {
	let traceId = "trace" + i;
  evtSource.addEventListener(traceId, function(m) {
    let parts = m.data.split("=");
    let name = parts[0];
    let value = parts[1];
    trace_name_to_number[name] = i;
    trace_name_to_number[name] = value;
    let traceValueDiv = document.getElementById(traceId);
    if (!traceValueDiv) {
      let traceDiv = document.createElement("div");
      traceDiv.className = "card trace";
      traceDiv.id = "card-" + traceId;
      document.getElementById("traces").appendChild(traceDiv);
      let traceBodyDiv = document.createElement("div");
      traceBodyDiv.className = "card-body";
      traceDiv.appendChild(traceBodyDiv);
      let traceTitleDiv = document.createElement("h5");
      traceTitleDiv.className = "card-title";
      traceTitleDiv.innerHTML = name;
      traceBodyDiv.appendChild(traceTitleDiv);
      traceValueDiv = document.createElement("p");
      traceValueDiv.className = "card-text";
      traceValueDiv.id = traceId;
      traceBodyDiv.appendChild(traceValueDiv);
    }
    traceValueDiv.innerHTML = value;
  });
}

// Create a message handler
evtSource.onmessage = function(m) {
    console.log(m.data);
}

// Ask the server what buttonts it supports
var xhr = new XMLHttpRequest();
xhr.open('GET', server + "buttons", false);
xhr.onreadystatechange = function () {
    if (xhr.readyState == 4) {
        if (xhr.status == 200) {
            var data = xhr.responseText;
            for (let line of data.split("\n")) {
                if (line.trim().length > 0) {
                  buttonDiv = document.createElement("button");
                  buttonDiv.className = "btn btn-primary";
                  buttonDiv.id = line;
                  buttonDiv.innerHTML = line;
                  buttonDiv.onclick = () => { handleClickButton(line); }
                  document.getElementById("buttons").appendChild(buttonDiv);
                }
            }
            console.log(data);
        }
    }
};
xhr.send(null);

// Send a POST message before leaving this page to tell the server
// side to disconnect
window.addEventListener("beforeunload", closeAll, false);
function closeAll() {
  evtSource.close();
  var xhr = new XMLHttpRequest();
  xhr.open("POST", server, false);
  try {
    xhr.send(null);
  } catch (e) {}
}
    </script>
  </body>
</html>
)";