FROM ubuntu:20.04

RUN apt-get update \
    && apt-get install -y firefox mesa-utils libgl1-mesa-dri libgl1-mesa-glx libpci3 libegl1-mesa \
    && rm -rf /var/lib/apt/lists/*

CMD ["firefox"]

#Install curl
RUN apt-get update
RUN apt-get install sudo -y

RUN apt-get upgrade -y

RUN apt-get update
RUN apt-get install curl -y

RUN curl -sL https://deb.nodesource.com/setup_14.x | sudo -E bash -

#Install node
RUN apt-get install -y nodejs

RUN apt-get upgrade -y

RUN apt-get update
