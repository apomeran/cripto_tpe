TPE 2 Criptografía y Seguridad

Requerimientos;

libreria SSL DEV

Modo de instalacion:
apt-get install libssl-dev


Ejecutar luego 'make'

Luego ejecutar ./sss

Parametros obligatorios:
        -r o -d: Indica que se desea recuperar (-r) o distribuir (-d) una imagen.
        -k <número>: Indica la cantidad minima de sombras necesarias para recuperar el secreto.
        -secret <archivo>: Nombre de la imagen a encriptar o el nombre de la salida a la hora de desencriptar.

Parametros optativos:
        --dir <directorio>: Directorio de donde obtener las imágenes.
        -n <número>: Cantidad de sombras en las que se distribuye.

