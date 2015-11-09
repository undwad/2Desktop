$(function ()
{
    if (!window.localStorage.port) window.localStorage.port = 2711
    var host = $('#host')
    var port = $('#port')
    host.val(window.localStorage.host || '')
    port.val(window.localStorage.port || '')
    host.change(function () { window.localStorage.host = host.val() })
    port.change(function () { window.localStorage.port = port.val() })
    host.keyup(function () { window.localStorage.host = host.val() })
    port.keyup(function () { window.localStorage.port = port.val() })
});