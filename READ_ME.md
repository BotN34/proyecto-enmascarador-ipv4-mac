Bienvenidos a la documentacion del enmascarador de IPV4 y MAC en Linux

La funcion del codigo es crear una direccion IP y direccion MAC falsas, para "burlar" un ping o solicitud arp, lo que se hace es guardar las direcciones originales, se apagan los servicios, se establecen las nuevas direcciones y se vuelven a encender y el codigo se ejecuta por un tiempo determinado con funcoines de pcap, para despues restablecer las direcciones originales


-Las funciones getMacAddress y getIPv4Address, guardan las direcciones originales del dispositivo local, con comandos de Linux, son puestos como strings

-Las funciones changeMacAddress y changeIPv4Address, apagan los servicios de IP y MAC, y cambian las direcciones con la variable newIP y newMAC, con comandos de Linux, son puestos como strings y puestos en funcinoes system() para ser ejecutados en la consola

-Las funciones resetMacAddress y resetIPv4Address, recrean las mismas operaciones que changeMacAddress y changeIPv4Address, pero establecen las direcciones originales del equipo con las variables originalIP y originalMAC

-Para la creacion y guardado de los datos en un archivo, se usa la funcion generateLogFileName que establece el nombre con la fecha y hora de ejecucion, 

-En la funcion principal se crea un buffer de error y se establecen cuales son las direcciones IP y MAC nuevas, a la vez que le decimos cual es el dispositivo de red del equipo local

-Se tiene una secuencia con las funciones
 1- Establecemos la interfaz y nuevas direcciones
 2- Guardamos las direcciones originales
 3- Cambiamos las direcciones
 4- Llamamos a la funcoin sleep que detiene el programa despues de 40 segundos
 5- Despues de que termine el tiempo se restauran las direcciones
 6-Se crea el atchivo con la fecha y tiempo de ejecucion, y se guardan los datos LogFile para que comprobemos que la ejecucion y el enmascarado tuvo exito
 7- Se cierran las funciones pcap, y el logFile
