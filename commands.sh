# commands used to build and run firefox in a container

docker build -t firefox-container .
docker run -it --name navegador --rm -e DISPLAY=172.20.32.1:0.0 firefox-container
