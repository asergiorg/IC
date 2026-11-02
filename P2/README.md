# Ejercicio propuesto
Este ejercicio deben hacerlo al menos, en parejas, con dos dispositivos MKR WAN 1310. En el sistema a implementar uno de los dispositivos tendrá la función de supervisor y el otro la función de dispositivo terminal sensor. Ambos van a comunicarse entre ellos a través del puerto serie asíncrono, análogamente a lo realizado en el apartado 4.<br>
El dispositivo supervisor enviará comandos a través del puerto serie al dispositivo sensor. El dispositivo sensor será responsable del acceso a través del puerto I2C de al menos dos sensores de ultrasonidos SRF02, atenderá a los comandos que le lleguen por el puerto serie desde el supervisor, los llevará a cabo y confirmará su realización, o bien, notificará cualquier incidencia en caso de
error. El dispositivo supervisor aceptará comandos introducidos a través del monitor serial por el usuario, los interpretará y enviará el comando correspondiente al dispositivo sensor, esperando su respuesta, y mostrando a través del monitor serial el resultado de su ejecución.<br>
El formato de los comandos a aceptar por el dispositivo supervisor serán cadenas escritas por el usuario. Al menos deben implementarse los siguientes comandos.
- help: se muestra información acerca de los comandos aceptados y la operativa correcta del sistema.
- us <srf02> {one-shot | on <period_ms> | off}: se comanda un único disparo del sensor de ultrasonidos (one-shot), o bien se establece que se dispare con un periodo específico de manera continuada (on <period_ms>), o que se cese de disparar el sensor de manera periódica si lo estuviera (off). En el comando debe identificarse qué sensor SRF02 quiere dispararse (<srf02>).
- us <srf02> unit {inc | cm | ms}: este comando permite modificar la unidad de medida devuelta por un sensor SRF02 específico (<srf02>).
- us <srf02> delay <ms>: este comando establece el tiempo de espera o retardo mínimo que debe haber entre dos disparos consecutivos del sensor (<srf02>).
- us <srf02> status: este comando debe proporcionar información de configuración del sensor, en concreto, su dirección I2C, retardo mínimo entre disparos, su configuración de unidades de medida, y su estado de disparo periódico, en el caso de que éste esté activado o no.
- us: este comando debe proporcionar la relación de sensores de ultrasonidos disponibles en el
dispositivo sensor. <br>
Todos estos comandos deben tener un mensaje de respuesta, o bien, de confirmación, o bien de
error, o con la información requerida en cada comando específico, en su caso.
## Formato de los mensajes
El formato de mensajes a intercambiar a través del puerto serie, así como sus respuestas, es binario
y debe seguir el siguiente esquema:
- primer byte: código de operación del mensaje, los valores específicos de este código de operación se dejan abiertos a los que quieran proponer.
- resto de bytes del cuerpo del mensaje: se deja abierto al formato que quieran proponer.

<table border="1">
  <tr>
    <th>1 byte</th>
    <th>resto de bytes</th>
  </tr>
  <tr>
    <td>código</td>
    <td>cuerpo del mensaje</td>
  </tr>
</table>

## Sugerencias y puntos de especial atención
- Tengan en cuenta que los sensores de ultrasonidos cuándo se activen de manera periódica enviaran al supervisor de manera asíncrona mensajes con el periodo especificado, indicando la medida tomada en cada disparo. Esta información debe mostrarse en el supervisor, en el monitor serial, sin interferir con lo que en ese momento el supervisor esté realizando.
- Para tener dos o más sensores de ultrasonidos en el bus I 2C deben modificar la dirección I2C de al menos uno de ellos, puesto que de fábrica vienen con la misma dirección preestablecida.

## Apartado opcional bus CAN
Aquellas parejas que quieran pueden solicitar realizar este ejercicio utilizando los shields MKR CAN en lugar del puerto serie. En este caso, las comunicaciones entre dispositivos se llevarán a cabo mediante bus CAN.

## Apartado opcional pantalla OLED
Incorporar en el bus I2C del dispositivo sensor un dispositivo de pantalla OLED o LCD. Incorpore al sistema los comandos que considere necesarios para mostrar información en el dispositivo de pantalla durante la ejecución del sistema. Como sugerencia podría siempre mostrarse lo que el dispositivo sensor está haciendo en cada momento, así como su configuración y estado mostrada en pantalla en todo momento.

## Apartado opcional ChibiOS
Utiliza ChibiOS tanto en el dispositivo supervisor como en el sensor. Será especialmente valorable el uso de recursos del sistema operativo para la implementación del firmware de cada dispositivo, i.e., uso de hilos, cerrojos, semáforos, etc.