# commands used to build and run firefox in a container

docker build -t firefox-container c:/users/mike/documents/distributedComputing/socket2ndproject/clients_folder

docker run -it --privileged --name navegador --network distributedCompNetwork --rm -e DISPLAY=172.20.32.1:0.0 -v c:/users/mike/documents/distributedComputing/socket2ndproject/clients_folder/:/Clients_Folder firefox-container

# command to run the index.html in linux

firefox "file:///Clients_Folder/index.html"
OR
lynx "file:///Clients_Folder/index.html"     # (you need to install lynx first)
