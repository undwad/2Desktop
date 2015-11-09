var additionalInfo =
{
    "title": document.title,
    "selection": window.getSelection().toString()
};

chrome.runtime.connect().postMessage(additionalInfo);
