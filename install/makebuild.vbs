SetLocale(1033)
WScript.Echo Right(Year(Date()), 2) & MonthName(Month(date), true) & Right("0" & Day(date), 2) & " " & Right("0" & Hour(time), 2) & Right("0" & Minute(time), 2)
