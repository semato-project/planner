Kroki:

1: zainstaluj docker i docker-compose

2: Zbuduj obraz - w katalogu projektu:

docker build . -t pvm_img_1 -f Dockerfile

3: Odpal obrazy - w tym samym katalogu

docker-compose up

4: Wejdź do kontenerów od 1 do 5 w 5 różnych oknach rerminala:

docker exec -it -u1000 planer2_pvm{ tu cyfra od 1 do 5}_1 bash


Przydatne komendy:

gcc master.c -o master -lpvm3

gcc slave.c -o slave -lpvm3