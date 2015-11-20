# esp8266-telnet-csv
A simple unauthenticated telnet server for exporting CSV data, Arduino+ESP8266

The quickest path between a mass of sensor readings and a spreadsheet might be a CSV file, and this simple telnet server running on an Arduino + ESP8266 dumps data formatted as CSV.

<img src="http://static.efetividade.net/img/captura-de-tela-2015-11-20-as-18.44.09-13274-39990.png">

It runs on port 2222, and you could get a foo.csv file by running something like <tt>telnet 192.168.0.235 2222 > foo.csv</tt> on your command prompt.

I explained it all (but only in portuguese) here: http://br-arduino.org/2015/11/servidor-esp8266-telnet-csv.html
