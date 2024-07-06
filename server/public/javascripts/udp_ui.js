"use strict";
// Client-side interactions with the browser.

var socket = io.connect();
let logs = ["<br>"];
let logs_search = ["<br>"];
var searchID = "";
var count = 0;

/**
 * true - normal mode
 * false - search mode
 */
var flag = true;
$(document).ready(function () {

	$("#rfid_submit").click(function () {
		var rfidText = $("#rfid_id").val();
		if (rfidText.trim() == "") {
			// do nothing
		}
		else {
			flag = false;
			searchID = rfidText;
		}
	})

	$("#rfid_all").click(function () {
		flag = true;
	});

	$("#rfid_clear").click(function () {
		clearLog();
	});

	window.setInterval(function () {
		sendCommand("upTime", "getUptime");
	}, 1000);

	window.setInterval(function () {
		sendCommand("history", "getHistory");
	}, 500);

	// handle data coming back from the server
	socket.on("Get History", function (result) {
		appendLog(result);
	});

	socket.on("Uptime Info", function (result) {
		changeText("#upTime", result);
	});

});

function appendLog(value) {

	value = value.trim();
	var len = value.length
	if (value !== undefined && (value[len - 3] == 'T' || value[len - 3] == 'I')) {
		let paddedCount = (++count).toString().padStart(3, '_');
		var newVal = paddedCount + ": " + value;
		logs.push(newVal);

	}
	else if (value[len - 3] == " ") {
		// do nothing
	}
	else {
		// do nothing
	}

	if (flag) {
		showResult("#log_text", logs);
	}
	else {
		logs_search.length = 0;
		logs_search = ["<br>"];
		for (var i = 0; i < logs.length; i++) {
			if (compareIDs(searchID, logs[i])) {
				logs_search.push(logs[i]);
			}
		}
		showResult("#log_text", logs_search);
	}


}

function clearLog() {
	logs.length = 0;
	count = 0;
	logs = ["<br>"];
	logs_search.length = 0;
	logs_search = ["<br>"];
}

function changeText(key, value) {

	$(key).val(value);
}

function searchLog(id) {
	$(key).html("SEARCH CALLED");
}

function showResult(key, arr) {
	if (arr.length == 1) {
		$(key).html("No Logs.");
	}
	else {
		$(key).html(arr.join("<br>"));
	}

}

function compareIDs(a, b) {
	a = appendZero(a);
	var idB = b.slice(30, 38);
	console.log(idB);
	if (a !== null && idB !== null && a == idB) {
		return true;
	} else {
		return false;
	}
}

function appendZero(a) {
	let strA = String(a);

	while (strA.length < 8) {
		strA = '0' + strA;
	}

	return strA;
}

function sendCommand(command, message) {
	socket.emit(command, message);
};
