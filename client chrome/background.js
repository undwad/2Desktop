chrome.browserAction.onClicked.addListener(function (tab)
{
    ws = new WebSocket('ws://' + window.localStorage.host + ':' + window.localStorage.port);
    ws.onopen = function () { ws.send(tab.url) }
    ws.onerror = function () { chrome.tabs.create({ url: 'chrome-extension://' + location.host + '/options.html' }); }
});
