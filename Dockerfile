FROM ubuntu:latest AS builder
LABEL maintainer="Ronan Le Meillat"

RUN apt-get update && apt-get install -y git git-core apache2 php build-essential libsystemd-dev
COPY . /xlxd/src/
RUN cd /xlxd/src && \
    make && \
    make install

FROM ubuntu:latest AS runtime
RUN apt-get update && apt-get install -y apache2 php supervisor iproute2 net-tools && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Copier les binaires compilés
COPY --from=builder /xlxd/src/src/xlxd /usr/local/bin/xlxd
COPY --from=builder /xlxd/src/echo/xlxecho /usr/local/bin/xlxecho

# Configurer le dashboard
RUN rm -rf /var/www/html/*
COPY --from=builder /xlxd/src/dashboard2/ /var/www/html/
COPY --from=builder /xlxd/src/dashboard/ /var/www/html/dashboard/
# Copier la configuration Supervisor
COPY supervisord.conf /etc/supervisor/conf.d/supervisord.conf
COPY log_forwarder.sh /usr/local/bin/log_forwarder.sh
RUN chmod +x /usr/local/bin/log_forwarder.sh

# Créer les répertoires de logs
RUN mkdir -p /var/log/supervisor && \
    mkdir -p /var/log/xlxd
COPY config/* /etc/xlxd/

# Exposer les ports
EXPOSE 80 10001 10002 42000 8880 62030 21110

# Démarrer Supervisor
CMD ["/usr/bin/supervisord", "-c", "/etc/supervisor/conf.d/supervisord.conf"]
