# INFORME DE LABORATORIO 
## Integrantes 
- Kelly Julieth Arango Henao

  kjulieth.arangoh@udea.edu.co
  
  1036657098
  
- Brayan Stiven Jaraba Alvarez

  b.jaraba@udea.edu.co

  1032178608

**Funciones implementadas en el shell wish, explicando su propósito y funcionamiento dentro del programa**

```
print_error()
```

Esta función se utiliza cada vez que ocurre algún error en el shell. Su objetivo es mostrar un único mensaje estándar en la salida de error (stderr), tal como lo exige la práctica.

El mensaje que imprime es:

**An error has occurred**

Se llama en diferentes partes del programa para asegurar que todos los errores se manejen de forma uniforme.

```
builtin_cd(char **args, int count)
```
Esta función implementa el comando cd, que permite cambiar de directorio dentro del sistema.

Primero verifica que el número de argumentos sea correcto (solo uno). Si el usuario ingresa más o menos argumentos, o si ocurre un error al intentar cambiar de directorio con chdir(), se muestra el mensaje de error.

```
 builtin_path(char **args, int count)
```

Esta función se encarga de manejar el comando path, que define en qué rutas el shell debe buscar los ejecutables.

Lo que hace es:

Eliminar las rutas que ya estaban guardadas
Guardar las nuevas rutas ingresadas por el usuario

Si el usuario no define ninguna ruta, el shell no podrá ejecutar comandos externos.

```
execute_command(char **args)
```

Esta función es la encargada de ejecutar los comandos externos (como ls, pwd, etc.).

Para lograrlo, recorre cada una de las rutas definidas en path, construye la ruta completa del comando y verifica con access() si existe y se puede ejecutar. Si lo encuentra, ejecuta el programa usando execv().

Si el comando no se encuentra en ninguna ruta, se muestra el mensaje de error.

```
parse_args(char *line, int *count)
```

Esta función se encarga de dividir la línea que ingresa el usuario en partes más pequeñas (tokens), separando el comando y sus argumentos.

Utiliza strsep() para separar por espacios, tabulaciones o saltos de línea. Además, ignora espacios vacíos y construye dinámicamente un arreglo con los argumentos que luego serán utilizados para ejecutar el comando.

```
handle_redirection(char *cmd, char **clean_cmd, char **outfile)
```
Esta función se encarga de detectar y procesar la redirección de salida usando el operador >.

Lo que hace es:

Verificar si existe el símbolo >
Validar que solo haya una redirección
Separar el comando del nombre del archivo
Comprobar que el formato sea correcto

Retorna:

0 si no hay redirección
1 si la redirección es válida
-1 si hay un error en la sintaxis

```
 main(int argc, char *argv[])
```

Esta es la función principal del programa y donde se controla todo el funcionamiento del shell.

Dentro de esta función se realiza lo siguiente:

Se inicializa la ruta por defecto (/bin)
Se determina si el programa se ejecuta en modo interactivo o batch
Se lee la entrada del usuario usando getline()
Se separan comandos paralelos con el operador &
Se procesa la redirección de salida
Se identifican y ejecutan los comandos integrados (cd, path, exit)
Se crean procesos hijos con fork() para ejecutar comandos externos
Se espera la finalización de los procesos con waitpid()

Este proceso se repite continuamente hasta que el usuario ejecuta el comando exit, momento en el cual el shell termina su ejecución.
