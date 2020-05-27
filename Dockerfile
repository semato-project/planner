FROM ubuntu:18.04

ENV PVM_LIB /usr/lib/libpvm3.a
ENV PVM_ROOT /usr/lib/pvm3
ENV PVM_ARCH LINUX64
ENV XPVM_ROOT /usr/bin/xpvm
ENV PVM_RSH /usr/bin/ssh
ENV PVM_PATH /home/pvm/pvm3/bin/$PVM_ARCH

RUN apt-get update && apt-get upgrade -y
RUN apt-get install -y make nano libpvm3 pvm pvm-dev gcc vim gnuplot mc iputils-ping ssh
RUN apt-get install -y --reinstall build-essential

RUN cd /root && \
 touch start.sh && \
 chmod 777 start.sh && \
 echo "#!/bin/bash\nservice ssh start && sleep 9999999">>start.sh && \
 useradd -m pvm && \
 echo "pvm:pvm" | chpasswd && \
 cd /home/pvm && \
 mkdir -p $PVM_PATH && \
 chown -R pvm:pvm pvm3 && \
 touch .rhosts && \
 echo "pvm1 pvm\npvm2 pvm\npvm3 pvm\npvm4 pvm\npvm5 pvm">>.rhosts && \
 chown pvm:pvm .rhosts

EXPOSE 22
CMD ["/root/start.sh"]
