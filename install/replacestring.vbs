path = WScript.Arguments(0)
oldstr = WScript.Arguments(1)
newstr = WScript.Arguments(2)
set filesystem = CreateObject("Scripting.FileSystemObject")
set file = filesystem.OpenTextFile(path)
do until file.AtEndOfStream
    line = file.ReadLine
    if InStr(line, oldstr) > 0 then line = Replace(line, oldstr, newstr) end If 
    WScript.Echo line
loop
