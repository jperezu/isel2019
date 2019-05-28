RETO - ISEL

Jorge Miguel P�rez Utrera
Marcos Sanchez Her�ndez
Juli�n Chich�n Guti�rrez
Alberto Tato G�mez

Implementaci�n b�sica basada en las especificaciones propuestas para el reto:

- Indicador LED RGB conectado a los GPIOs 12 (verde), 13 (azul), 15 (rojo), c�todo a GND.
- Potenciometro simulador de bater�a conectado al ADC0 (pin central del potenci�metro, los otros a 3V3 y GND).
- Bot�n para introducir contrase�a, conectado al GPIO14 y GND.

- Tramas UDP enviadas en formato ID : BAT_LEVEL mV - (OK/NO_OK) : RENTED/NOT_RENTED : TIMESTAMP
*El ID se ha obtenido a partir del Chip_Id del ESP8266 para asegurar que sea un identificador �nico asociado autom�ticamente.
- En caso de que se pierda la conexi�n el sistema se queda buscando nuevas conexiones y reenv�a las tramas atrasadas a un ritmo de 1 trama/s, aunque lleve m�s retraso que el caso de conexi�n constante. Es importante no descartar estas tramas para poder analizar posibles casos de malfuncionamiento del sistema que se diesen mientras no hab�a conexi�n y de esta forma identificarlos y corregirlos de forma m�s sencilla.

- El lenguaje del sistema en C esta contenido en el modelo de verificaci�n. Esto se consigui� gracias al modelado de las m�quinas de estados mediante diagramas de bolas que permitieron su facil traducci�n a ambos modelos sin esfuerzo. De esta forma tanto m�quinas de estados, variables, funciones de condici�n, de salida y estados estan comparidos entre promela y C como puede comprobarse. Aunque eliminando la dependencia temporal en el modelo Promela para simplificar la verificaci�n.

- Hemos decidido emplear EJECUTIVO CICLICO para llevar a cabo la planificaci�n debido a que evita la exclusi�n mutua por lo que los distintos procedimientos pueden compartir recursos y datos. Adem�s, los periodos empleados son arm�nicos luego la planificaci�n empleando ejecutivo ciclico ser� optima y correcta por construcci�n. Por otro lado, al no existir concurrencia en la ejecuci�n nos ha sido m�s f�cil depurar el sistema en busca de fallos. Finalmente, este m�todo de planificaci�n optimiza la memoria y el consumo, b�sico en un sistema empotrado como el planteado en el reto, donde debe estar largas horas sin acceso a carga para dar servicio.

*Para la verificaci�n formal puede utilizarse el MAKFILE implementado o bien el VERIFIER (.bat para Windows) para realizar el make y clean.

*En caso de utlizar las secuencias de spin en la terminal debe tenerse en cuenta de tener mas de 6 process debe compilarse con el par�metro DNFAIR=3 y es recomendable limitar la profundidad con -m200 para reducir la carga en memoria.