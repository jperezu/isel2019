RETO - ISEL

Jorge Miguel Pérez Utrera
Marcos Sanchez Herández
Julián Chichón Gutiérrez
Alberto Tato Gómez

Implementación básica basada en las especificaciones propuestas para el reto:

- Indicador LED RGB conectado a los GPIOs 12 (verde), 13 (azul), 15 (rojo), cátodo a GND.
- Potenciometro simulador de batería conectado al ADC0 (pin central del potenciómetro, los otros a 3V3 y GND).
- Botón para introducir contraseña, conectado al GPIO14 y GND.

- Tramas UDP enviadas en formato ID : BAT_LEVEL mV - (OK/NO_OK) : RENTED/NOT_RENTED : TIMESTAMP
*El ID se ha obtenido a partir del Chip_Id del ESP8266 para asegurar que sea un identificador único asociado automáticamente.
- En caso de que se pierda la conexión el sistema se queda buscando nuevas conexiones y reenvía las tramas atrasadas a un ritmo de 1 trama/s, aunque lleve más retraso que el caso de conexión constante. Es importante no descartar estas tramas para poder analizar posibles casos de malfuncionamiento del sistema que se diesen mientras no había conexión y de esta forma identificarlos y corregirlos de forma más sencilla.

- El lenguaje del sistema en C esta contenido en el modelo de verificación. Esto se consiguió gracias al modelado de las máquinas de estados mediante diagramas de bolas que permitieron su facil traducción a ambos modelos sin esfuerzo. De esta forma tanto máquinas de estados, variables, funciones de condición, de salida y estados estan comparidos entre promela y C como puede comprobarse. Aunque eliminando la dependencia temporal en el modelo Promela para simplificar la verificación.

- Hemos decidido emplear EJECUTIVO CICLICO para llevar a cabo la planificación debido a que evita la exclusión mutua por lo que los distintos procedimientos pueden compartir recursos y datos. Además, los periodos empleados son armónicos luego la planificación empleando ejecutivo ciclico será optima y correcta por construcción. Por otro lado, al no existir concurrencia en la ejecución nos ha sido más fácil depurar el sistema en busca de fallos. Finalmente, este método de planificación optimiza la memoria y el consumo, básico en un sistema empotrado como el planteado en el reto, donde debe estar largas horas sin acceso a carga para dar servicio.

*Para la verificación formal puede utilizarse el MAKFILE implementado o bien el VERIFIER (.bat para Windows) para realizar el make y clean.

*En caso de utlizar las secuencias de spin en la terminal debe tenerse en cuenta de tener mas de 6 process debe compilarse con el parámetro DNFAIR=3 y es recomendable limitar la profundidad con -m200 para reducir la carga en memoria.