FROM debian
RUN apt-get update && apt-get install -y --no-install-recommends \
	qt5-default qtdeclarative5-dev \
	g++ make pkg-config \
	libblkid-dev libfuse-dev \
	rubygems ruby-dev && \
    gem install --no-ri --no-rdoc fpm && \
    rm -rf /var/lib/apt/lists/*
VOLUME /gostcrypt
COPY build.sh .
WORKDIR /gostcrypt
ENTRYPOINT /build.sh
