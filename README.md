# Select_Pthread_Server

  A simple multiple server that combines pthread and select, **for homework** :)
  

## Execute

For **select_server.c**:

- `gcc -g -Wall select_server.c -o select_server -lpthread`

For **select_client.c**:

- `gcc -g -Wall select_client.c -o select_client`


## Test File Transfer

- If you want to tansfer file, you can key in the following string in client side.
  - `require test.txt`

  After keying `require test.txt`, server will send the **[test.txt]** file to Client folder.
