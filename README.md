Przydatne komendy:

Budowanie obrazu:
docker build . -t pvm_img_1 -f Dockerfile

Inicjalizacja docker swarm mode:
docker swarm init --advertise-addr 127.0.0.1 --default-addr-pool 10.1.0.0/16
docker swarm leave --force
docker stack deploy --compose-file docker-compose.yaml planner

Wejście do kontenera:
docker exec -it -u1000 {nazwa_kontenera_} bash

Kompilacja:
cd $PVM_PATH
g++ -o planner_manager matrix.cpp planner_manager.cpp -lpvm3
g++ -o planner_worker matrix.cpp planner_worker.cpp -lpvm3

g++ -o planner_manager matrix.cpp planner_manager.cpp -lpvm3 && g++ -o planner_worker matrix.cpp planner_worker.cpp -lpvm3

Uruchomienie
spawn -> planner_manager
