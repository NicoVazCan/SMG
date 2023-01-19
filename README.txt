AUTOR
  Nicolás Vázquez Cancela

TÍTULO
  Generador de máquinas de estado finito mediante un lenguaje de especificación dentro de C.

ABREVIATURAS
 · SM: máquinas de estado finito.

DESCRIPCIÓN
  El objetivo de este programa es construir automáticamente ficheros código fuente C estructurados siguiendo las características de las SM por medio de un lenguaje de especificación que se incrusta en el código C. A mayores, se generará un fichero ‘sm_loop.c’ que implementa la función ‘sm_loop’ necesaria para actualizar el estado de todas las SM que se le pasen por iteraciones hasta que todas finalicen.

  Este lenguaje definirá las características que compartirán todas las instancias de la misma clase de SM. Comienza cuando se encuentra el símbolo ‘$’ y termina cuando se vuelve a encontrar sin seguirlo ningún carácter reservado, considerándose código C todo lo demás, pudiéndose referenciar dentro del lenguaje de especificación cuando proceda. Las características a definir son:

· Datos, variables con sus tipos de C correspondientes que pertenecen a cada instancia y permanecen activas durante toda su duración. Su sintaxis es “data ${ ‘tipo’ ‘id’; … $}”;

· Estados, son por los que puede transitar una SM entre las iteraciones, tienen adjunto un identificado único entre estados ,útil para la especificación de otras características y comprobar en tiempo de ejecución, si una SM se encuentra en este, haciendo uso de la macro SM_STATE_’identificador’, y un bloque de código C con azúcar sintáctico para acceder a las variables definidas en Datos con la sintaxis “’código C’->’id SM’.’id variable’”, y cambiar de estado que se explicará en detalle más adelante. Su sintaxis es “state ‘id state’ ${ ‘código C azucarado’ $}”;

· Acciones, son las que permiten cambiar de un estado a otro, le pertenece un identificador único entre acciones, útil para la especificación de otras características y para decir que la realice una instancia de SM que la definiera, pudiendo hacerlo a si misma o desde otra clase de SM, unos parámetros C que necesita la acción para realizarse, y un bloque de código C con azúcar sintáctico en el aparte de lo mencionado, se podrá llamar a la acción de una instancia con la sintaxis “’código C’->’id acción’$( ‘código C’, … $)”. Su sintaxis es “act ‘act id’ $( ‘tipo’ ‘id’, … $) ${ ‘código C azucarado’ $}”;

· Arcos, definen las transiciones posibles entre estados y las acciones que requieren para activarse. Su sintaxis es “’id0 state’ -> ‘id1 state’ $[ ‘id act’, … $]”, adicionalmente se pude omitir el ‘id0 state’ para definir como estado inicial ‘id1 state’, y viceversa para el estado final. También sirve para construir los mensajes de error en ejecución cuando se llame a una acción desde un estado que no la requiere;

·Importar, importa las variables de Data de una clase de SM, estados, y sus acciones, a mayores especifica que la clase SM a definir depende de otra clase, lo que posiciona a todas las instancias de la primera justo después de las de la segunda en la secuencia de actualización de estados o Pipeline de Sms, esto es para que cuando se acceda a las variables de Data de una instancia de la segunda clase estén actualizadas para usarse desde la otra. Su sintaxis es “import ‘SM id’”. Se puede usar varias veces para importar varias clases de SMs diferentes;

  Una vez definidas las características de la clase SM en un fichero, el programa lo transforma en tokens con ‘scanner.l’, luego se parsea con ‘parser.y’, obteniendo el árbol gramatical correspondiente a la clase SM para finamente evaluarse con la función ‘terms_eval’ del fichero ‘terms.c’ y ‘terms.h’ que genera el fichero código C y header correspondiente a la clase SM. Esto se realiza con cada uno de los ficheros de definición que se le entreguen al programa para terminar generando la función ‘sm_loop’ en ‘sm_loop.c’ que incluye en el bucle las instancias de las clases de SM definidas en los ficheros.

  También fue necesario escribir una librería de utilidades para SMs ‘sm_lib.c’ y ‘sm_lib.h’ en la que se definen varias funciones que usarán los ficheros SMs código C que el programa generó y se declara la función ‘sm_loop’ definida en el fichero ‘sm_loop.c’.

  Con los ficheros código C equivalentes a las clases  de SMs, ‘sm_loop.c’, ‘sm_lib.c’, y un fichero código C que use la función ‘sm_loop’ y cree instancias de las clases de SMs, solo resta compilarlos para generar un nuevo programa que use las SMs.

INSTRUCCIONES DE COMPILACIÓN
  Para construir y testear el programa, se facilitó un ‘Makefile’ que contiene las reglas: all, para construir el programa con el nombre ‘smg’; test, para generar un programa de prueba que use ‘smg’ llamado ‘test’; y clean, para eliminar ficheros objeto y código C generado.

INSTRUCCIONES DE EJECUCIÓN
  Modo de empleo: smg [OPCIÓN]... [FICHERO]...
  Pruebe 'smg --help' para más información.

  El programa 'test' se encuentra en la carpeta 'test' y se ejecuta usando dos SMs, la primera pregunta si se quiere especificar uno o dos números, se debe responder con '1' o '2', en otro caso se preguntará de nuevo. Luego dependiendo de la opción escogida, se leerá uno o dos número, y finalmente la primera termina. Tras esto, la segunda SM itera el número de veces que se le haya pasado a la primera, y finamente itera de nuevo con el segundo número, que en el caso de solo haber especificado uno, será iterá de nuevo con el primero.

HERRAMIENTAS ADICIONALES
  Para el desarrollo de la librería de utilidades para máquinas de estado finito ‘sm_lib.c’ y ‘sm_lib.h’, se ha usado la librería de estructuras de datos ‘SGLIB’ (https://github.com/stefanct/sglib) que entre otras cosas permite el manejo de listas de forma genérica mediante macros.

INFORMACIÓN ADICIONAL
  Cabe destacar que el programa y los ficheros código C que generan, no producen ningún leak de memoria. Se hizo free a todos los malloc, excepto a 3 malloc por culpa de FLEX o Bison.
