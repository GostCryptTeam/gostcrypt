FROM debian
RUN apt-get update
RUN apt-get install -y qt5-default qtdeclarative5-dev
RUN apt-get install -y g++ make pkg-config
RUN apt-get install -y libblkid-dev libfuse-dev
RUN apt-get install -y rubygems ruby-dev && \
    gem install --no-ri --no-rdoc fpm
VOLUME /gostcrypt
COPY build.sh .
WORKDIR /gostcrypt
ENTRYPOINT /build.sh
